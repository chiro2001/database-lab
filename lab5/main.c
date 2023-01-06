//
// Created by chiro on 22-12-29.
//
#include "main_utils.h"
#include "buffered_queue.h"

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

  buffered_queue *q = buffered_queue_init(1, 100);
  unsigned int block = 17;
  while (block != 0) {
    char *blk = NULL;
    Log("Loading block %d", block);
    blk = readBlock(block);
    for (int i = 0; i < 7; i++) {
      if (SEQ("128", blk + i * 8)) {
        Log("(%s, %s)", blk + i * 8, blk + i * 8 + 4);
        buffered_queue_push(q, blk + i * 8);
      }
    }
    Log("next block: %s", blk + 56);
    block = atoi3(blk + 56);
    freeBlock(blk);
    if (block == 49) break;
  }
  buffered_queue_flush(q);
  free(q);

  Log("read results:");
  block = 100;
  while (block != 0) {
    char *blk = NULL;
    Log("Loading block %d", block);
    blk = readBlock(block);
    for (int i = 0; i < 7; i++) {
      if (*(blk + i * 8) != '\0')
        Log("(%s, %s)", blk + i * 8, blk + i * 8 + 4);
    }
    block = atoi3(blk + 56);
    freeBlock(blk);
  }

  freeBuffer(&buf);

  Log("=========================");
  Log("两阶段多路归并排序算法(TPMMS)");
  Log("利用内存缓冲区将关系 R 和 S 分别排序,并将排序后的结果存放在磁盘上。");
  Log("=========================");

  return 0;
}