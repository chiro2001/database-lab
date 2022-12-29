//
// Created by chiro on 22-12-29.
//
#include <stdio.h>
#include <stdlib.h>
#include <debug_macros.h>
#include "extmem.h"

Buffer buf;

char *readBlock(unsigned int addr) {
  char *blk = NULL;
  Assert(NULL != (blk = (char *) readBlockFromDisk(addr, &buf)), "Reading Block %u Failed", addr);
  return blk;
}

void freeBlock(char *blk) {
  freeBlockInBuffer((unsigned char *) blk, &buf);
}

char *itoa(unsigned int i) {
  static char itoa_buffer[5];
  sprintf(itoa_buffer, "%d", i);
  return itoa_buffer;
}

struct buffered_queue_blk {
  char *blk;
  unsigned int addr;
  struct buffered_queue_blk *next;
  struct buffered_queue_blk *prev;
};
typedef struct buffered_queue_blk buffered_queue_blk;

typedef struct {
  Buffer *buffer;
  int total;
  unsigned int addr;
  buffered_queue_blk *linked_blk;
  int size;
  size_t offset;
} buffered_queue;

buffered_queue *buffered_queue_init(int sz, unsigned addr) {
  buffered_queue *q = malloc(sizeof(buffered_queue));
  memset(q, 0, sizeof(buffered_queue));
  q->buffer = &buf;
  q->addr = addr;
  q->total = sz;
  return q;
}

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
    strcpy(self->linked_blk->blk + self->offset, itoa(++self->addr));
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

void buffered_queue_flush(buffered_queue *self) {
  Log("buffered_queue_flush");
  buffered_queue_blk *tail = self->linked_blk;
  while (tail->next != NULL) {
    tail = tail->next;
  }
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

int main() {
  Log("Lab5 program launched!");

  Assert(initBuffer(520, 64, &buf), "Buffer Initialization Failed!\n");

  Log("Table and data struct:");
  Log("R [A, B], block [1,  16]");
  Log("S [C, D], block [17, 48]");
  Log("======================");
  Log("基于线性搜索的关系选择算法");
  Log("select S.C,S.D from S where S.C = 128");
  Log("======================");

  buffered_queue *q = buffered_queue_init(1, 100);
  unsigned int block = 17;
  while (block != 0) {
    char *blk = NULL;
    Log("Loading block %d", block);
    blk = readBlock(block);
    for (int i = 0; i < 7; i++) {
      int C = -1, D = -1;
      C = atoi(blk + i * 8);
      D = atoi(blk + i * 8 + 4);
      if (C == 128) {
        Log("(%s, %s)", blk + i * 8, blk + i * 8 + 4);
        buffered_queue_push(q, blk + i * 8);
      }
    }
    block = atoi(blk + 56);
    freeBlock(blk);
    if (block == 49) break;
  }
  buffered_queue_flush(q);
  free(q);

  Log("read results:");
  block = 100;
  while (block != 0) {
    char *blk = NULL;
    Log("Loading block %d", block);
    blk = readBlock(block);
    for (int i = 0; i < 7; i++) {
      if (*(blk + i * 8) != '\0')
        Log("(%s, %s)", blk + i * 8, blk + i * 8 + 4);
    }
    block = atoi(blk + 56);
    freeBlock(blk);
  }

  freeBuffer(&buf);

  return 0;
}