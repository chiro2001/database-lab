//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "questions.h"
#include "buffered_queue.h"
#include "iterator.h"
#include "cache.h"

void create_index_range(uint left, uint right, uint addr) {
  buffered_queue *q = buffered_queue_init(1, addr, true);
  uint cnt = 0;
  uint index = left;
  // storage primary index to secondary place
  // I = (key, block)
  iterate_range(left, right, lambda(bool, (char *s) {
      if (++cnt == 7) {
        cnt = 0;
        buffered_queue_push(q, itot(atoi3(s), index++));
      }
      return true;
  }));
}

bool block_contains_key(char *blk, uint key) {
  for (int i = 0; i < 7; i++)
    if (atoi3(blk + i * 8) == key) return true;
  return false;
}

void indexed_select_binary_search(uint left, uint right, uint key, buffered_queue *target, cache *ca) {
  if (ca == NULL) ca = cache_init(BLK - 1);
  if (right == left || right == left + 1) {
    char *blk_left = cache_read(ca, left);
    for (int i = 0; i < 7; i++) {
      char *s = blk_left + i * 8;
      if (*s != '\0')
        if (atoi3(s) == key)
          buffered_queue_push(target, s);
    }
    if (left != right) {
      char *blk_right = cache_read(ca, right);
      for (int i = 0; i < 7; i++) {
        char *s = blk_right + i * 8;
        if (*s != '\0')
          if (atoi3(s) == key)
            buffered_queue_push(target, s);
      }
    }
    return;
  }

  uint mid = (left + right) / 2;
  char *a = cache_read(ca, left);
  char *b = cache_read(ca, mid);
  char *c = cache_read(ca, right);
  uint key_first = atoi3(a);
  uint key_middle_left = atoi3(b);
  uint key_middle_right = atoi3(block_tuple_tail(b));
  uint key_last = atoi3(block_tuple_tail(c));
  Assert(key_first <= key && key <= key_last, "key %d not found in range [%d, %d]", key, left, right);
  if (key_first <= key && key < key_middle_left) {

  }
}

void indexed_select_linear(uint left, uint right, uint key, buffered_queue *target) {
  Log("indexed_select_linear(%d, %d, key=%d)", left, right, key);
  uint addr_last = -1;
  uint addrs[BLK] = {0};
  uint *addrs_pointer = addrs;
  bool addrs_ok = false;
  uint blk_count = 0;
  uint blk_offset = 0;
  iterate_range(left, right, lambda(bool, (char *s) {
      Log("-> (%s, %s)", s, s + 4);
      uint k = atoi3(s);
      uint addr = atoi3(s + 4);
      if (k >= key && !addrs_ok) {
        if (addr_last != -1) {
          Log("select addr %d", addr_last);
          *(addrs_pointer++) = addr_last;
        }
        Log("select addr %d", addr);
        *(addrs_pointer++) = addr;
      }
      if (k > key) {
        addrs_ok = true;
        Log("-> OK");
      }
      addr_last = addr;
      if ((++blk_offset) == 7) {
        blk_offset = 0;
        blk_count++;
      }
      return !addrs_ok;
  }));
  uint addrs_sz = addrs_pointer - addrs;
  for (uint *p = addrs; p != addrs_pointer; p++) {
    iterate_range(*p, *p + 1, lambda(bool, (char *s) {
        if (*s != '\0' && atoi3(s) == key)
        buffered_queue_push(target, s);
        return true;
    }));
  }
}

void q3() {
  Log("============================");
  Log("Q3: 基于索引的关系选择算法");
  Log("排序结果为关系R或S分别建立索引文件，");
  Log("模拟实现 select S.C, S.D from S where S.C = 128");
  Log("============================");
  buffer_init();
  Log("正在排序...");
  TPMMS(1, 17, 301);
  TPMMS(17, 49, 317);
  Log("正在建立索引...");
  create_index_range(301, 317, 501);
  create_index_range(317, 349, 517);
  buffer_free();

  buffer_init();
  Log("索引文件位于 [501...], [517...]");
  buffered_queue *q = buffered_queue_init(1, 600, true);
  // indexed_select_binary_search(317, 349, 128, q, NULL);
  indexed_select_linear(517, 520, 128, q);
  buffered_queue_flush(q);
  buffered_queue_free(q);
  uint count = 0;
  iterate_range(600, -1, lambda(bool, (char *s) {
    if (*s != NULL) {
      Log("-> (%s, %s)", s, s + 4);
      count++;
    }
    return true;
  }));
  Log("满足选择条件的元组一共 %d 个", count);
  buffer_report();
  buffer_free();
}