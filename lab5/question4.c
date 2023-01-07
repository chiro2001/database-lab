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
  iterator *r = iterator_init(301, 317, ca);
  iterator *s = iterator_init(317, 349, ca);
  buffered_queue *q = buffered_queue_init(1, 700, true);
  while (iterator_now(r) != NULL && iterator_now(s) != NULL) {
    bool overflow = false;
    while (cmp_greater(iterator_now(s), iterator_now(r))) {
      iterator_next(r);
      if (iterator_next(r) == NULL) {
        overflow = true;
        break;
      }
    }
    if (overflow) break;
    if (SEQ(iterator_now(s), iterator_now(r))) {
      iterator *r_clone = iterator_clone(r);
      while (SEQ(iterator_now(s), iterator_now(r_clone))) {
        buffered_queue_push(q, iterator_now(s));
        buffered_queue_push(q, iterator_now(r_clone));
        iterator_next(r_clone);
      }
      iterator_free_clone(r_clone);
    }
    iterator_next(s);
  }
  buffered_queue_flush(q);
  buffered_queue_free(q);
  Log("results:");
  iterate_range(700, -1, lambda(bool, (char *s) {
    if (*s != '\0') Log("(%s, %s)", s, s + 4);
    return true;
  }));
  buffer_report();
  buffer_free();
}