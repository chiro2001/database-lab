//
// Created by chiro on 23-1-7.
//

#include "cache.h"

cache *cache_init(uint total) {
  cache *p = malloc(sizeof(cache));
  memset(p, 0, sizeof(cache));
  p->total = total;
  p->cache = malloc(sizeof(cache_pair) * total);
  memset(p->cache, 0, sizeof(cache_pair) * total);
  return p;
}

uint cache_read_cnt = 0;

cache_pair *cache_find(cache *self, uint addr) {
  for (uint i = 0; i < self->size; i++) {
    if (self->cache[i].addr == addr) {
      return &self->cache[i];
    }
  }
  return NULL;
}

void cache_insert(cache *self, uint addr) {
  Log("cache_insert(%d)", addr);
  Assert(self->size < self->total, "insert failed, full");
  // self->cache = realloc(self->cache, ++self->size);
  cache_pair *cache = &self->cache[self->size];
  cache->addr = addr;
  cache->blk = read_block(addr);
  cache->visit = cache_read_cnt;
  self->size++;
}

void cache_remove_index(cache *self, int index) {
  Log("cache_remove_index(%d)", index);
  if (index < 0 || index >= self->size) return;
  cache_pair *target = &self->cache[index];
  if (target == NULL) return;
  free_block(target->blk);
  // cache_pair *cache_new = malloc(sizeof(cache_pair) * (self->total));
  // memset(cache_new, 0, sizeof(cache) * (self->total));
  // cache_pair *cache_tail = cache_new;
  // for (int i = 0; i < self->size; i++)
  //   if (i != index) {
  //     cache_tail->addr = self->cache[i].addr;
  //     cache_tail->blk = self->cache[i].blk;
  //     cache_tail++;
  //   }
  // free(self->cache);
  // self->cache = cache_new;

  // move last item to target place
  memcpy(self->cache + self->size, target, sizeof(cache_pair));
  self->size--;
}

void cache_remove(cache *self, uint addr) {
  cache_pair *target = cache_find(self, addr);
  if (target == NULL) return;
  cache_remove_index(self, (int) (target - self->cache));
}

void cache_kick(cache *self) {
  if (self->size == 0) return;
  uint size_original = self->size;
  uint oldest = -1;
  int index = -1;
  for (uint i = 0; i < self->size; i++) {
    if (self->cache[i].visit < oldest) {
      index = (int) i;
      oldest = self->cache[i].visit;
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
  if (self->size == self->total) {
    cache_kick(self);
  }
  cache_insert(self, addr);
  target = cache_find(self, addr);
  Assert(target != NULL, "cache insert failed!");
  return target->blk;
}

void cache_free(cache *self) {
  free(self->cache);
  free(self);
}