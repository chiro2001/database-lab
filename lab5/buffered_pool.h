//
// Created by chiro on 23-1-7.
//

#ifndef LAB5_BUFFERED_POOL_H
#define LAB5_BUFFERED_POOL_H

#include "main_utils.h"

typedef struct {
  uint addr;
  char *blk;
  uint visit;
} buffered_pool_pair;

typedef struct {
  buffered_pool_pair *pool;
  uint size;
  uint total;
} buffered_pool;

buffered_pool *buffered_pool_init(uint total);

extern uint buffered_pool_read_cnt;

buffered_pool_pair *buffered_pool_find(buffered_pool *self, uint addr);

void buffered_pool_insert(buffered_pool *self, uint addr);

void buffered_pool_remove_index(buffered_pool *self, int index);

void buffered_pool_remove(buffered_pool *self, uint addr);

void buffered_pool_kick(buffered_pool *self);

char *buffered_pool_read(buffered_pool *self, uint addr);

#endif //LAB5_BUFFERED_POOL_H
