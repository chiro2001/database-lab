//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "questions.h"
#include "buffered_queue.h"

void TPMMS_sort_subsets(uint left, uint right) {
  uint blk_total = right - left;
  uint rounds = blk_total / BLK +
                ((blk_total % BLK) == 0 ? 0 : 1);
  for (uint r = 0; r < rounds; r++) {
    buffered_queue *q = buffered_queue_init(BLK, -1);
    // load one subset
    for (uint p = 0; p < min(BLK, blk_total - r * BLK); p++) {
      iterate_range(r * BLK, (r + 1) * BLK, lambda(void, (char *c){
          buffered_queue_push(q, c);
      }));
    }

    free(q);
  }
}

void TPMMS(uint left, uint right) {
  TPMMS_sort_subsets(left, right);
}

void q2() {
  Log("============================");
  Log("Q2: 两阶段多路归并排序算法(TPMMS)");
  Log("利用内存缓冲区将关系 R 和 S 分别排序,并将排序后的结果存放在磁盘上。");
  Log("============================");
  TPMMS(1, 17);
  // TPMMS(17, 49);
}