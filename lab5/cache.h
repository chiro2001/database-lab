//
// Created by chiro on 23-1-7.
//

#ifndef LAB5_CACHE_H
#define LAB5_CACHE_H

#include "main_utils.h"

typedef struct {
  uint addr;
  char *blk;
  uint visit;
} cache_pair;

typedef struct {
  cache_pair *cache;
  uint size;
  uint total;
} cache;

cache *cache_init(uint total);

extern uint cache_read_cnt;

cache_pair *cache_find(cache *self, uint addr);

void cache_insert(cache *self, uint addr);

void cache_remove_index(cache *self, int index);

void cache_remove(cache *self, uint addr);

void cache_kick(cache *self);

char *cache_read(cache *self, uint addr);

void cache_free(cache *self);

#endif //LAB5_CACHE_H
