//
// Created by chiro on 23-1-6.
//

#ifndef LAB5_BUFFERED_QUEUE_H
#define LAB5_BUFFERED_QUEUE_H

#include "main_utils.h"

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

buffered_queue *buffered_queue_init(int sz, unsigned addr);
void buffered_queue_push(buffered_queue *self, char *tuple);
void buffered_queue_flush(buffered_queue *self);

#endif //LAB5_BUFFERED_QUEUE_H
