//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "buffered_queue.h"

void q1() {
  Log("=========================");
  Log("Q1: 基于线性搜索的关系选择算法");
  Log("select S.C,S.D from S where S.C = 128");
  Log("=========================");

  buffered_queue *q = buffered_queue_init(1, 100);
  unsigned int block = 17;
  while (block != 0) {
    char *blk = NULL;
    Dbg("loading block %d", block);
    blk = readBlock(block);
    for (int i = 0; i < 7; i++) {
      if (SEQ("128", blk + i * 8)) {
        Log("(%s, %s)", blk + i * 8, blk + i * 8 + 4);
        buffered_queue_push(q, blk + i * 8);
      }
    }
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
    Dbg("loading block %d", block);
    blk = readBlock(block);
    for (int i = 0; i < 7; i++) {
      if (*(blk + i * 8) != '\0')
        Log("(%s, %s)", blk + i * 8, blk + i * 8 + 4);
    }
    block = atoi3(blk + 56);
    freeBlock(blk);
  }
}