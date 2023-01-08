//
// Created by chiro on 23-1-6.
//

#include "iterator.h"
#include "cache.h"

/**
 * 迭代器指向下一个项目。当到达尾部的时候不会继续向前。
 * @param it
 */
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

/**
 * 迭代器向后移动 [[n]] 个项目
 * @param it
 * @param n
 */
void iterator_next_n(iterator *it, uint n) {
  while (n--) iterator_next(it);
}

/**
 * 迭代器向前移动一个项目。⚠️代码有bug。
 * @param it
 */
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

/**
 * 迭代器向前移动 [[n]] 个项目
 * @param it
 * @param n
 */
void iterator_prev_n(iterator *it, uint n) {
  Log("iterator_prev_n(%d), now=%d, offset=%zu", n, it->now, it->offset);
  while (n--) iterator_prev(it);
}

/**
 * 获取迭代器当前的项目。
 * @param it
 * @return 到达尾部则返回 NULL
 */
char *iterator_now(iterator *it) {
  if (iterator_is_end(it)) return NULL;
  Dbg("iterator_now, now=%d, offset=%zu", it->now, it->offset);
  Assert(it->blk, "now blk NULL");
  return it->blk + it->offset;
}

/**
 * 初始化迭代器
 * @param begin 左闭区间参数
 * @param end 右开区间参数
 * @param ca 缓存指针
 * @return 迭代器
 */
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

/**
 * 判断迭代器是否到达尾部
 * @param it
 * @return
 */
bool iterator_is_end(iterator *it) {
  if (it->offset == 56 && it->now == it->end - 1) return true;
  Assert(it->blk, "NULL blk");
  if (*(it->blk + it->offset) == '\0') return true;
  return false;
}

/**
 * 释放迭代器
 * @param it
 */
void iterator_free(iterator *it) {
  if (!it->ca && it->blk != NULL) free_block(it->blk);
  free(it);
}

/**
 * 释放被克隆出的迭代器
 * @param it
 */
void iterator_free_clone(iterator *it) {
  // if (!it->ca && it->blk) free_block(it->blk);
  if (it->blk) free_block(it->blk);
  free(it);
}

/**
 * 克隆迭代器
 * @param it
 * @return
 */
iterator *iterator_clone(iterator *it) {
  // Assert(it->ca == NULL, "cached iterator cannot clone!");
  iterator *it_new = malloc(sizeof(iterator));
  memcpy(it_new, it, sizeof(iterator));
  Dbg("iterator: creating new buffer for addr %d", it_new->now);
  // it_new->blk = it->ca ? cache_read(it->ca, it->now) : read_block(it->now);
  it_new->blk = read_block(it->now);
  // char *blk = read_block(it->now);
  // it_new->blk = alloc_block();
  // free_block(blk);
  // memcpy(it_new->blk, blk, BLK_SZ);
  return it_new;
}