//
// Created by chiro on 23-1-7.
//

#include "cache.h"

cache *cache_init(uint capacity) {
  cache *p = malloc(sizeof(cache));
  memset(p, 0, sizeof(cache));
  p->capacity = capacity;
  p->data = malloc(sizeof(cache_pair) * capacity);
  memset(p->data, 0, sizeof(cache_pair) * capacity);
  return p;
}

uint cache_read_cnt = 0;

cache_pair *cache_find(cache *self, uint addr) {
  for (uint i = 0; i < self->size; i++) {
    if (self->data[i].addr == addr) {
      return &self->data[i];
    }
  }
  return NULL;
}

void cache_insert(cache *self, uint addr) {
  Dbg("cache_insert(%d)", addr);
  Assert(self->size < self->capacity, "insert failed, full");
  // self->cache = realloc(self->cache, ++self->size);
  cache_pair *cache = &self->data[self->size];
  cache->addr = addr;
  cache->blk = read_block(addr);
  cache->visit = cache_read_cnt;
  self->size++;
}

void cache_remove_index(cache *self, int index) {
  if (index < 0 || index >= self->size) return;
  cache_pair *target = &self->data[index];
  if (target == NULL) return;
  free_block(target->blk);
  // move last item to target place
  Dbg("cache_remove_index(%d), size=%d, move %d to %d in cache",
      index, self->size, self->data[self->size - 1].addr, target->addr);
  memcpy(self->data + self->size - 1, target, sizeof(cache_pair));
  self->size--;
}

void cache_remove(cache *self, uint addr) {
  cache_pair *target = cache_find(self, addr);
  if (target == NULL) return;
  cache_remove_index(self, (int) (target - self->data));
}

void cache_kick(cache *self) {
  if (self->size == 0) return;
  uint size_original = self->size;
  uint oldest = -1;
  int index = -1;
  for (uint i = 0; i < self->size; i++) {
    if (self->data[i].visit < oldest) {
      index = (int) i;
      oldest = self->data[i].visit;
    }
  }
  cache_remove_index(self, index);
  Assert(self->size + 1 == size_original, "cache kick failed!");
}

char *cache_read(cache *self, uint addr) {
  cache_read_cnt++;
  cache_pair *target = cache_find(self, addr);
  // hit
  if (target != NULL) {
    target->visit = cache_read_cnt;
    return target->blk;
  }
  // miss, and full
  if (self->size == self->capacity) {
    cache_kick(self);
  }
  cache_insert(self, addr);
  target = cache_find(self, addr);
  Assert(target != NULL, "cache insert failed!");
  return target->blk;
}

void cache_free(cache *self) {
  if (!self) return;
  for (uint i = 0; i < self->size; i++) {
    free_block(self->data[i].blk);
  }
  free(self->data);
  free(self);
}