//
// Created by chiro on 22-12-29.
//
#include <cstdio>
#include <cstdlib>
#include <debug_macros.h>
#include "extmem.h"

Buffer buf;

void readBlock(unsigned int addr, char *&blk) {
  Assert(nullptr != (blk = (char *) readBlockFromDisk(addr, &buf)), "Reading Block %u Failed", addr);
}

void freeBlock(char *&blk) {
  freeBlockInBuffer((unsigned char *) blk, &buf);
  blk = nullptr;
}


int main() {
  Log("Lab5 program launched!");

  Assert(initBuffer(520, 64, &buf), "Buffer Initialization Failed!\n");

  Log("Table and data struct:");
  Log("R [A, B], block [1,  16]");
  Log("S [C, D], block [17, 48]");
  Log("======================");
  Log("基于线性搜索的关系选择算法");
  Log("select S.C,S.D from S where S.C = 128");
  Log("======================");

  unsigned int block = 17;
  while (block != 0) {
    char *blk = nullptr;
    Log("Loading block %d", block);
    readBlock(block, blk);
    for (int i = 0; i < 7; i++) {
      int C = -1, D = -1;
      C = atoi(blk + i * 8);
      D = atoi(blk + i * 8 + 4);
      if (C == 128)
        Log("(%s, %s)", blk + i * 8, blk + i * 8 + 4);
    }
    block = atoi(blk + 56);
    freeBlock(blk);
    if (block == 49) break;
  }

  freeBuffer(&buf);

  return 0;
}