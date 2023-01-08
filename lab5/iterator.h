//
// Created by chiro on 23-1-6.
//

#ifndef LAB5_ITERATOR_H
#define LAB5_ITERATOR_H

#include "main_utils.h"
#include "cache.h"

typedef struct {
  char *blk;
  size_t offset;
  uint begin, now, end;
  cache *ca;
} iterator;

iterator *iterator_init(uint begin, uint end, cache *ca);

void iterator_next(iterator *it);

void iterator_free(iterator *it);

iterator *iterator_clone(iterator *it);

void iterator_free_clone(iterator *it);

char *iterator_now(iterator *it);

#endif //LAB5_ITERATOR_H
