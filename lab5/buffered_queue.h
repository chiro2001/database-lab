//
// Created by chiro on 23-1-6.
//

#ifndef LAB5_BUFFERED_QUEUE_H
#define LAB5_BUFFERED_QUEUE_H

#include "main_utils.h"

struct buffered_queue_blk {
  char *blk;
  uint addr;
  struct buffered_queue_blk *next;
  struct buffered_queue_blk *prev;
};
typedef struct buffered_queue_blk buffered_queue_blk;

typedef struct {
  Buffer *buffer;
  int total;
  uint addr;
  buffered_queue_blk *linked_blk;
  int size;
  size_t offset;
  bool flushable;
} buffered_queue;

buffered_queue *buffered_queue_init(int sz, uint addr, bool flushable);

void buffered_queue_push(buffered_queue *self, char *tuple);

void buffered_queue_flush(buffered_queue *self);

buffered_queue_blk *buffered_queue_blk_tail(buffered_queue_blk *b);

void buffered_queue_free(buffered_queue *self);

void buffered_queue_iterate(buffered_queue *self, iter_handler(handler));

void buffered_queue_sort(buffered_queue *self, int order_by);

uint buffered_queue_count(buffered_queue *self);

void buffered_queue_push_blk(buffered_queue *self, uint addr, bool continuous);

#endif //LAB5_BUFFERED_QUEUE_H
