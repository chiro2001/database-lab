//
// Created by chiro on 23-1-7.
//
#include "questions.h"
#include "main_utils.h"

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
  while (iterator_now(r) != NULL && iterator_now(s) != NULL) {
    while (cmp_greater(iterator_now(s), iterator_now(r))) {
      iterator_next(r);
    }
    if (SEQ(iterator_now(s), iterator_now(r))) {
      iterator *r_clone = iterator_clone(r);
      while (SEQ(iterator_now(s), iterator_now(r_clone))) {
        iterator_next(r_clone);
      }
    }
    // iterator_next(r);
    // iterator_next(s);
  }
  buffer_report();
  buffer_free();
}