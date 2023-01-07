//
// Created by chiro on 23-1-6.
//

#include "iterator.h"
#include "cache.h"

char *iterator_next(iterator *it) {
  if (it->offset == 56) {
    if (it->blk != NULL) free_block(it->blk);
    if (it->now >= it->end - 1) {
      it->now++;
      it->blk = NULL;
      return NULL;
    } else {
      Dbg("iterator load new block: %d", it->now + 1);
      it->blk = it->ca ? cache_read(it->ca, ++it->now) : read_block(++it->now);
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

iterator *iterator_init(uint begin, uint end, cache *ca) {
  Log("iterator_init(%d, %d)", begin, end);
  iterator *it = malloc(sizeof(iterator));
  memset(it, 0, sizeof(iterator));
  it->begin = begin;
  it->end = end;
  it->now = it->begin;
  it->blk = it->ca ? cache_read(it->ca, it->now) : read_block(it->now);
  it->ca = ca;
  return it;
}

void iterator_free(iterator *it) {
  if (it->blk != NULL) free_block(it->blk);
  free(it);
}

void iterator_free_clone(iterator *it) {
  free(it);
}

iterator *iterator_clone(iterator *it) {
  iterator *it_new = malloc(sizeof(iterator));
  memcpy(it_new, it, sizeof(iterator));
  return it_new;
}