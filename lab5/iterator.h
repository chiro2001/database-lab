//
// Created by chiro on 23-1-6.
//

#ifndef LAB5_ITERATOR_H
#define LAB5_ITERATOR_H

#include "main_utils.h"

typedef struct {
  char *blk;
  size_t offset;
  uint begin, now, end;
} iterator;

iterator *iterator_init(uint begin, uint end);
char *iterator_next(iterator *it);
void iterator_free(iterator *it);
char *iterator_now(iterator *it);

#endif //LAB5_ITERATOR_H
