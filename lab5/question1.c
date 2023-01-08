//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "buffered_queue.h"
#include "questions.h"

void q1() {
  Log("=========================");
  Log("Q1: 基于线性搜索的关系选择算法");
  Log("select S.C,S.D from S where S.C = 128");
  Log("=========================");

  buffer_init();
  uint target = 100;
  uint count = 0;
  buffered_queue *q = buffered_queue_init(1, target, true);
  iterate_range(17, 49, lambda(bool, (char* c) {
    if (SEQ(c, "128")) {
      buffered_queue_push(q, c);
      count++;
    }
    return *c != '\0';
  }));
  buffered_queue_flush(q);
  buffered_queue_free(q);
  buffer_report_msg("线性搜索");
  Log("选择结果如下，储存于地址 %d", target);
  iterate_range_show(target, -1);
  Log("满足选择条件的元组一共 %d 个", count);
  buffer_free();
}