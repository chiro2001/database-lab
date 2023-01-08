//
// Created by chiro on 23-1-6.
//

#include "iterator.h"
#include "cache.h"

void iterator_next(iterator *it) {
  if (it->offset != 56)
    it->offset += 8;
  if (it->offset == 56) {
    if (it->blk != NULL && it->ca == NULL) free_block(it->blk);
    // if (it->now == it->end - 1 || it->now == it->end) {
    if (it->now == it->end - 1) {
      Dbg("ends for iterator [%d, %d) now=%d", it->begin, it->end, it->now);
      // if (it->now == it->end - 1)
      //   it->now++;
      // it->blk = NULL;
    } else {
      if (it->end == -1) {
        // infinity
        Assert(it->ca == NULL, "not implemented");
        Dbg("iterator load new block: %d", it->now + 1);
        it->blk = read_block_try(++it->now);
        if (it->blk == NULL) {
          Assert(g_buf.numFreeBlk > 0, "must caused by not found address");
        }
      } else {
        Dbg("iterator load new block: %d", it->now + 1);
        it->blk = it->ca ? cache_read(it->ca, ++it->now) : read_block(++it->now);
      }
      it->offset = 0;
    }
  }
}

void iterator_prev(iterator *it) {
  if (it->offset == 0) {
    it->now--;
    Log("iterator prev loading addr %d", it->now);
    Assert(it->now >= it->begin, "iterator prev to begin!");
    if (it->blk != NULL && it->ca == NULL) free_block(it->blk);
    it->blk = it->ca ? cache_read(it->ca, it->now) : read_block(it->now);
    Assert(it->blk, "NULL blk!");
    it->offset = 56;
  }
  it->offset -= 8;
}

void iterator_prev_n(iterator *it, uint n) {
  Log("iterator_prev_n(%d), now=%d, offset=%zu", n, it->now, it->offset);
  while (n--) iterator_prev(it);
}

char *iterator_now(iterator *it) {
  if (iterator_is_end(it)) return NULL;
  Dbg("iterator_now, now=%d, offset=%zu", it->now, it->offset);
  Assert(it->blk, "now blk NULL");
  return it->blk + it->offset;
}

iterator *iterator_init(uint begin, uint end, cache *ca) {
  Dbg("iterator_init(%d, %d)", begin, end);
  iterator *it = malloc(sizeof(iterator));
  memset(it, 0, sizeof(iterator));
  it->begin = begin;
  it->end = end;
  it->now = it->begin;
  it->blk = it->ca ? cache_read(it->ca, it->now) : read_block(it->now);
  it->ca = ca;
  return it;
}

bool iterator_is_end(iterator *it) {
  if (it->offset == 56 && it->now == it->end - 1) return true;
  Assert(it->blk, "NULL blk");
  if (*(it->blk + it->offset) == '\0') return true;
  return false;
}

void iterator_free(iterator *it) {
  if (!it->ca && it->blk != NULL) free_block(it->blk);
  free(it);
}

void iterator_free_clone(iterator *it) {
  // if (!it->ca && it->blk) free_block(it->blk);
  if (it->blk) free_block(it->blk);
  free(it);
}

iterator *iterator_clone(iterator *it) {
  // Assert(it->ca == NULL, "cached iterator cannot clone!");
  iterator *it_new = malloc(sizeof(iterator));
  memcpy(it_new, it, sizeof(iterator));
  Log("creating new buffer for addr %d", it_new->now);
  char *blk = it->ca ? cache_read(it->ca, it->now) : read_block(it->now);
  it_new->blk = alloc_block();
  memcpy(it_new->blk, blk, 48 + 4);
  return it_new;
}