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
  buffer_report();
  buffer_free();
}