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
  iterate_range(left, right, lambda(void, (char *s) {
      if (++cnt == 7) {
      cnt = 0;
      buffered_queue_push(q, itot(atoi3(s), index++));
  }
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
  buffered_queue *q = buffered_queue_init(1, 500, true);
  indexed_select_binary_search(317, 349, 128, q, NULL);
  buffered_queue_flush(q);
  buffered_queue_free(q);
  buffer_report();
  buffer_free();
}