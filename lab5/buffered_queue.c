//
// Created by chiro on 23-1-6.
// 一个写入缓存，在写满缓冲区之前仍可读取缓冲区内容
//

#include "buffered_queue.h"

/**
 * 初始化 buffered_queue
 * @param sz 缓冲区块数
 * @param addr 起始写入地址
 * @return 新的 buffered_queue 结构体
 */
buffered_queue *buffered_queue_init(int sz, uint addr, bool writeable) {
  buffered_queue *q = malloc(sizeof(buffered_queue));
  memset(q, 0, sizeof(buffered_queue));
  q->buffer = &g_buf;
  q->addr = addr;
  q->total = sz;
  q->writeable = writeable;
  return q;
}

/**
 * 向 buffered_queue 中插入新的数据
 * @param self
 * @param tuple 字符串数据，例如 "123\0456\0"
 */
void buffered_queue_push(buffered_queue *self, char *tuple) {
  Log("queue push (%s, %s)", tuple, tuple + 4);
  if (self->linked_blk == NULL) {
    Log("init linked blocks, sizeof(buffered_queue_blk) = %lu", sizeof(buffered_queue_blk));
    self->linked_blk = malloc(sizeof(buffered_queue_blk));
    self->linked_blk->blk = (char *) getNewBlockInBuffer(self->buffer);
    self->linked_blk->next = NULL;
    self->linked_blk->prev = NULL;
    self->linked_blk->addr = self->addr;
  }
  if (self->offset == 56) {
    // fill in next addr in this block
    strcpy(self->linked_blk->blk + self->offset, itoa(self->addr == -1 ? -1 : ++self->addr));
    self->offset = 0;
    if (self->size == self->total - 1) {
      buffered_queue_blk *tail = self->linked_blk;
      while (tail->next != NULL) {
        tail = tail->next;
      }
      buffered_queue_blk *prev = tail->prev;
      if (prev != NULL) {
        prev->next = NULL;
      } else {
        self->linked_blk = malloc(sizeof(buffered_queue_blk));
        self->linked_blk->blk = (char *) getNewBlockInBuffer(self->buffer);
        self->linked_blk->next = NULL;
        self->linked_blk->prev = NULL;
        self->linked_blk->addr = self->addr;
      }
      Assert(!self->writeable, "buffered queue cannot write!");
      Log("queue full, write block %d", tail->addr);
      writeBlockToDisk((unsigned char *) tail->blk, tail->addr, self->buffer);
      free(tail);
    } else {
      self->size++;
    }
    // allocate one buffer, insert to link head
    Log("new buffer for block %d", self->addr);
    char *blk = (char *) getNewBlockInBuffer(self->buffer);
    memset(blk, 0, 64);
    buffered_queue_blk *n = malloc(sizeof(buffered_queue_blk));
    Log("n=%p, self->linked_blk=%p", n, self->linked_blk);
    Assert(n != self->linked_blk, "n == self->linked_blk == %p", n);
    n->addr = self->addr;
    n->blk = blk;
    n->next = self->linked_blk;
    n->prev = NULL;
    self->linked_blk->prev = n;
    self->linked_blk = n;
  }
  memcpy(self->linked_blk->blk + self->offset, tuple, 8);
  self->offset += 8;
}

buffered_queue_blk *buffered_queue_blk_tail(buffered_queue_blk *b) {
  buffered_queue_blk *tail = b;
  while (tail != NULL && tail->next != NULL) {
    tail = tail->next;
  }
  return tail;
}

/**
 * 刷新缓冲区，将所有数据写回
 * @param self
 */
void buffered_queue_flush(buffered_queue *self) {
  Log("buffered_queue_flush");
  buffered_queue_blk *tail = buffered_queue_blk_tail(self->linked_blk);
  while (tail != NULL) {
    writeBlockToDisk((unsigned char *) tail->blk, tail->addr, self->buffer);
    buffered_queue_blk *r = tail;
    tail = tail->prev;
    free(r);
  }
  self->size = 0;
  self->linked_blk = NULL;
  self->offset = 0;
}

void buffered_queue_free(buffered_queue *self) {
  buffered_queue_blk *h = self->linked_blk;
  while (h != NULL) {
    buffered_queue_blk *t = h;
    h = h->next;
    freeBlockInBuffer((unsigned char *) t->blk, &g_buf);
    free(t);
  }
  free(self);
}

void buffered_queue_iterate(buffered_queue *self, iter_handler(handler)) {
  buffered_queue_blk *t = buffered_queue_blk_tail(self->linked_blk);
  if (t == NULL) return;
  while (t != NULL) {
    for (int i = 0; i < 7; i++) {
      if (*(t->blk + i * 8) != '\0')
        handler(t->blk + i * 8);
    }
    t = t->prev;
  }
}

uint buffered_queue_count(buffered_queue *self) {
  return self->size * 7 + (self->offset) / 8;
}

char **buffered_queue_blks(buffered_queue *self) {
  buffered_queue_blk *t = buffered_queue_blk_tail(self->linked_blk);
  char **blks = malloc(sizeof(char *) * (BLK + 1));
  memset(blks, 0, sizeof(char *) * (BLK + 1));
  if (t == NULL) return blks;
  char **p = blks;
  while (t != NULL) {
    Assert(p - blks <= BLK, "too many blocks");
    *p = t->blk;
    p++;
    t = t->prev;
  }
  return blks;
}

char *buffered_queue_get(char **blks, uint index) {
  Assert(index <= BLK * 7, "index out of range");
  return blks[index / 7] + (index % 7) * 8;
}

bool cmp2(char *a, char *b) {
  uint i = atoi3(a);
  uint j = atoi3(b);
  return i < j;
}

void buffered_queue_sort(buffered_queue *self, int order_by) {
  char **blks = buffered_queue_blks(self);
  uint sz = buffered_queue_count(self);
  // simple bubble sort
  for (uint i = 0; i < sz - 1; i++) {
    for (uint j = 0; j < sz - 1 - i; j++) {
      char *a = buffered_queue_get(blks, j);
      char *b = buffered_queue_get(blks, j + 1);
      if (order_by == 0) {
        if (cmp2(a, b)) SWAP(a, b);
      } else {
        if (cmp2(a + 4, b + 4)) SWAP(a + 4, b + 4);
      }
    }
  }
  free(blks);
}