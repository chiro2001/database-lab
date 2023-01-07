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
  // I = (key, addr), addr is where key first appears
  uint key_last = -1;
  iterate_range(left, right, lambda(bool, (char *s) {
    uint key = atoi3(s);
    if (key != key_last) {
      if (key_last != -1) Assert(key > key_last, "data not in order!");
      Dbg("push index (key=%s, addr=%d)", s, index);
      buffered_queue_push(q, itot(key, index));
      key_last = key;
    }
    if ((++cnt) == 7) {
      cnt = 0;
      index++;
    }
    return true;
  }));
  buffered_queue_flush(q);
  buffered_queue_free(q);
}

void indexed_select_linear(uint left, uint right, uint key, buffered_queue *target) {
  Dbg("indexed_select_linear(%d, %d, key=%d)", left, right, key);
  uint target_addr = -1;
  iterate_range(left, right, lambda(bool, (char *s) {
    uint k = atoi3(s);
    if (k == key) {
      target_addr = atoi3(s + 4);
      Dbg("got target addr: %d", target_addr);
    }
    return target_addr == -1;
  }));
  Assert(target_addr != -1, "cannot find key %d", key);
  bool read_started = false;
  iterate_range(target_addr, -1, lambda(bool, (char *s) {
    uint k = atoi3(s);
    if (!read_started) {
      if (k == key) {
        buffered_queue_push(target, s);
        read_started = true;
      }
      return true;
    } else {
      if (k == key) {
        buffered_queue_push(target, s);
      }
      return k == key;
    }
  }));
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
  // Log("S before sort:");
  // iterate_range_show(17, 49);
  TPMMS(17, 49, 317);
  // Log("S after sort:");
  // iterate_range_show(317, -1);
  Log("正在建立索引...");
  create_index_range(301, 317, 501);
  create_index_range(317, 349, 517);
  // Log("S indexes:");
  // iterate_range_show(517, -1);
  buffer_free();

  buffer_init();
  Log("索引文件位于 [501...], [517...]");
  buffered_queue *q = buffered_queue_init(1, 600, true);
  indexed_select_linear(517, -1, 128, q);
  buffered_queue_flush(q);
  buffered_queue_free(q);
  uint count = 0;
  q = buffered_queue_init(4, -1, false);
  iterate_range(600, -1, lambda(bool, (char *s) {
    if (*s != '\0') {
      // Log("-> (%s, %s)", s, s + 4);
      buffered_queue_push(q, s);
      count++;
    }
    return true;
  }));
  Log("满足选择条件的元组一共 %d 个", count);
  buffered_queue_sort(q, 1);
  buffered_queue_show(q);
  buffered_queue_free(q);
  buffer_report();
  buffer_free();
}