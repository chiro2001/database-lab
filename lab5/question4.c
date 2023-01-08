//
// Created by chiro on 23-1-7.
//
#include "questions.h"
#include "main_utils.h"
#include "buffered_queue.h"

void q4() {
  Log("===================================");
  Log("基于排序的连接操作算法(Sort-Merge-Join)");
  Log("select S.C, S.D, R.A, R.B from S inner join R on S.C = R.A");
  Log("===================================");
  buffer_init();
  Log("正在排序...");
  TPMMS(1, 17, 301);
  TPMMS(17, 49, 317);
  Log("排序后数据位于:");
  Log("R [A, B], block [301, 316]");
  Log("S [C, D], block [317, 348]");
  cache *ca = cache_init(4);
  // cache *ca = NULL;
  iterator *r = iterator_init(301, 317, ca);
  // iterator *r2 = iterator_init(301, 317, ca);
  iterator *s = iterator_init(317, 349, ca);
  buffered_queue *q = buffered_queue_init(1, 700, true);
  uint join_count = 0;
  while (iterator_now(r) != NULL && iterator_now(s) != NULL) {
    while (cmp_greater(iterator_now(s), iterator_now(r))) {
      iterator_next(r);
      // iterator_next(r2);
    }
    if (SEQ(iterator_now(s), iterator_now(r))) {
      iterator *r_clone = iterator_clone(r);
      // iterator *r_clone = iterator_clone(r2);
      while (SEQ(iterator_now(s), iterator_now(r_clone))) {
        Log("push (%s, %s) (%s, %s)",
            iterator_now(s), iterator_now(s) + 4,
            iterator_now(r_clone), iterator_now(r_clone) + 4);
        buffered_queue_push(q, iterator_now(s));
        buffered_queue_push(q, iterator_now(r_clone));
        join_count++;
        iterator_next(r_clone);
      }
      iterator_free_clone(r_clone);
    }
    iterator_next(s);
  }
  buffered_queue_flush(q);
  buffered_queue_free(q);
  Log("results:");
  iterate_range_show(700, -1);
  Log("连接次数: %d", join_count);
  buffer_report();
  buffer_free();
}