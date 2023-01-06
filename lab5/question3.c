//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "questions.h"
#include "buffered_queue.h"
#include "iterator.h"

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

void indexed_select_binary_search(uint left, uint right, uint key, buffered_queue *target) {
  if (right - left > BLK - 1) {
    for (uint i = left; i < right; i += BLK - 1) {
      indexed_select_binary_search(i, min(i + BLK - 1, right), key, target);
    }
  } else {

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
  indexed_select_binary_search(317, 349, 128, q);
  buffered_queue_flush(q);
  buffered_queue_free(q);
  buffer_report();
  buffer_free();
}