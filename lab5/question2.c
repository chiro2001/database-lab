//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "questions.h"
#include "buffered_queue.h"
#include "iterator.h"

void TPMMS_sort_subset(uint left, uint right, uint target, bool continous) {
  buffered_queue *q = buffered_queue_init(BLK, target, false);
  // load one subset
  for (uint addr = left; addr < right; addr++)
    buffered_queue_push_blk(q, addr, continous);
  // sort in this subset
  buffered_queue_sort(q, 0);
  q->flushable = true;
  buffered_queue_flush(q);
  buffered_queue_free(q);
}

void TPMMS_sort_subsets(uint left, uint right, uint target) {
  uint blk_total = right - left;
  uint rounds = blk_total / BLK +
                ((blk_total % BLK) == 0 ? 0 : 1);
  for (uint r = 0; r < rounds; r++)
    TPMMS_sort_subset(
        left + r * BLK,
        left + (r + 1) * BLK,
        target + r * BLK,
        r != rounds - 1);
}

void TPMMS_merge_sort(uint left, uint right, uint target) {
  uint blk_total = right - left;
  uint reader_count = blk_total / BLK +
                      ((blk_total % BLK) == 0 ? 0 : 1);
  Assert(reader_count <= BLK, "merge sort cannot have readers more than %d", BLK);
  iterator *readers[BLK] = {NULL};
  for (int i = 0; i < reader_count; i++)
    readers[i] = iterator_init(i * BLK, min((i + 1) * BLK, right));

}

void TPMMS(uint left, uint right, uint target) {
  uint temp = target + 100;
  TPMMS_sort_subsets(left, right, temp);
}

void q2() {
  Log("============================");
  Log("Q2: 两阶段多路归并排序算法(TPMMS)");
  Log("利用内存缓冲区将关系 R 和 S 分别排序,并将排序后的结果存放在磁盘上。");
  Log("============================");
  TPMMS(1, 17, 301);
  // TPMMS(17, 49, 317);
}