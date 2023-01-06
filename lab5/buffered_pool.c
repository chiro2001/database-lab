//
// Created by chiro on 23-1-7.
//

#include "buffered_pool.h"

buffered_pool *buffered_pool_init(uint total) {
  buffered_pool *p = malloc(sizeof(buffered_pool));
  memset(p, 0, sizeof(buffered_pool));
  p->total = total;
  p->pool = malloc(sizeof(buffered_pool_pair) * total);
  memset(p->pool, 0, sizeof(buffered_pool_pair) * total);
  return p;
}

uint buffered_pool_read_cnt = 0;

buffered_pool_pair *buffered_pool_find(buffered_pool *self, uint addr) {
  for (uint i = 0; i < self->size; i++) {
    if (self->pool[i].addr == addr) {
      return &self->pool[i];
    }
  }
  return NULL;
}

void buffered_pool_insert(buffered_pool *self, uint addr) {
  Log("buffered_pool_insert(%d)", addr);
  Assert(self->size < self->total, "insert failed, full");
  // self->pool = realloc(self->pool, ++self->size);
  buffered_pool_pair *pool = &self->pool[self->size];
  pool->addr = addr;
  pool->blk = read_block(addr);
  pool->visit = buffered_pool_read_cnt;
  self->size++;
}

void buffered_pool_remove_index(buffered_pool *self, int index) {
  Log("buffered_pool_remove_index(%d)", index);
  if (index < 0 || index >= self->size) return;
  buffered_pool_pair *target = &self->pool[index];
  if (target == NULL) return;
  free_block(target->blk);
  // buffered_pool_pair *pool_new = malloc(sizeof(buffered_pool_pair) * (self->total));
  // memset(pool_new, 0, sizeof(buffered_pool) * (self->total));
  // buffered_pool_pair *pool_tail = pool_new;
  // for (int i = 0; i < self->size; i++)
  //   if (i != index) {
  //     pool_tail->addr = self->pool[i].addr;
  //     pool_tail->blk = self->pool[i].blk;
  //     pool_tail++;
  //   }
  // free(self->pool);
  // self->pool = pool_new;

  // move last item to target place
  memcpy(self->pool + self->size, target, sizeof(buffered_pool_pair));
  self->size--;
}

void buffered_pool_remove(buffered_pool *self, uint addr) {
  buffered_pool_pair *target = buffered_pool_find(self, addr);
  if (target == NULL) return;
  buffered_pool_remove_index(self, (int) (target - self->pool));
}

void buffered_pool_kick(buffered_pool *self) {
  if (self->size == 0) return;
  uint size_original = self->size;
  uint oldest = -1;
  int index = -1;
  for (uint i = 0; i < self->size; i++) {
    if (self->pool[i].visit < oldest) {
      index = (int) i;
      oldest = self->pool[i].visit;
    }
  }
  buffered_pool_remove_index(self, index);
  Assert(self->size + 1 == size_original, "pool kick failed!");
}

char *buffered_pool_read(buffered_pool *self, uint addr) {
  buffered_pool_read_cnt++;
  buffered_pool_pair *target = buffered_pool_find(self, addr);
  // hit
  if (target != NULL) return target->blk;
  // miss, and full
  if (self->size == self->total) {
    buffered_pool_kick(self);
  }
  buffered_pool_insert(self, addr);
  target = buffered_pool_find(self, addr);
  Assert(target != NULL, "pool insert failed!");
  return target->blk;
}

void buffered_pool_free(buffered_pool *self) {
  free(self->pool);
  free(self);
}