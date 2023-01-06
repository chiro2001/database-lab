//
// Created by chiro on 23-1-6.
//

#include "iterator.h"

char *iterator_next(iterator *it) {
  if (it->offset == 56) {
    if (it->blk != NULL) freeBlock(it->blk);
    if (it->now >= it->end - 1) {
      it->now++;
      it->blk = NULL;
      return NULL;
    } else {
      Dbg("iterator load new block: %d", it->now + 1);
      it->blk = readBlock(++it->now);
      it->offset = 0;
      return iterator_now(it);
    }
  }
  char *r = iterator_now(it);
  it->offset += 8;
  return r;
}

char *iterator_now(iterator *it) {
  // Log("iterator [begin now end] offset: [%d %d %d] %d", it->begin, it->now, it->end, it->offset);
  if (it->offset == 56) {
    return iterator_next(it);
  }
  return it->blk + it->offset;
}

iterator *iterator_init(uint begin, uint end) {
  Log("iterator_init(%d, %d)", begin, end);
  iterator *it = malloc(sizeof(iterator));
  memset(it, 0, sizeof(iterator));
  it->begin = begin;
  it->end = end;
  it->now = it->begin;
  it->blk = readBlock(it->now);
  return it;
}

void iterator_free(iterator *it) {
  if (it->blk != NULL) freeBlock(it->blk);
  free(it);
}