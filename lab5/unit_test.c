//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"

int main() {
  Log("TEST: CMP");
  Assert(CMP("123", "456"), "CMP Failed!");
  return 0;
}