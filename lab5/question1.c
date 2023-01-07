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

  buffered_queue *q = buffered_queue_init(1, 100, true);
  iterate_range(17, 49, lambda(bool, (char* c){
      if (SEQ(c, "128")) buffered_queue_push(q, c);
      return true;
  }));
  buffered_queue_flush(q);
  buffered_queue_free(q);

  Log("read results:");
  uint count = 0;
  iterate_range(100, -1, lambda(bool, (char* c){
      if (*c) {
        Log("(%s, %s)", c, c + 4);
        count++;
      }
      return true;
  }));

  Log("满足选择条件的元组一共 %d 个", count);
  buffer_report();
  buffer_free();
}