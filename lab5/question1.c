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

  buffered_queue *q = buffered_queue_init(1, 100);
  iterate_range(17, 49, lambda(void, (char* c){
      if (SEQ(c, "128")) buffered_queue_push(q, c);
  }));
  buffered_queue_flush(q);
  free(q);

  Log("read results:");
  iterate_range(100, -1, lambda(void, (char* c){
      if (*c) Log("(%s, %s)", c, c + 4);
  }));
}