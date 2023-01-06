//
// Created by chiro on 22-12-29.
//
#include "main_utils.h"
#include "questions.h"

int main() {
  Log("Lab5 program launched!");

  Assert(initBuffer(520, 64, &buf), "Buffer Initialization Failed!\n");

  Log("Table and data struct:");
  Log("R [A, B], block [1,  16]");
  Log("S [C, D], block [17, 48]");

  q1();
  q2();

  freeBuffer(&buf);
  return 0;
}