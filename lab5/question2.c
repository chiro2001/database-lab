//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "questions.h"
#include "buffered_queue.h"
#include "iterator.h"

void TPMMS_sort_subset(uint left, uint right, uint target, bool continuous) {
  buffered_queue *q = buffered_queue_init(BLK, target, false);
  // load one subset
  for (uint addr = left; addr < right; addr++)
    buffered_queue_push_blk(q, addr, continuous);
  // sort in this subset
  buffered_queue_sort(q, 0);
  q->flushable = true;
  buffered_queue_flush(q);
  buffered_queue_free(q);
}

void TPMMS_sort_subsets(uint left, uint right, uint target) {
  Dbg("TPMMS_sort_subsets");
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

iterator *TPMM_reader_select(iterator *readers[BLK - 1]) {
  iterator *r = NULL;
  int s = 0;
  char smallest[9] = "";
  for (int i = 0; i < BLK - 1; i++) {
    if (readers[i] == NULL) continue;
    char *now = iterator_now(readers[i]);
    if (now == NULL) continue;
    if (smallest[0] == '\0') {
      tuple_copy(smallest, now);
      r = readers[i];
      s = i;
    } else {
      if (cmp_greater(smallest, now)) {
        tuple_copy(smallest, now);
        r = readers[i];
        s = i;
      }
    }
  }
  return r;
}

void TPMMS_merge_sort(uint left, uint right, uint target) {
  Dbg("TPMMS_merge_sort");
  uint blk_total = right - left;
  uint reader_count = blk_total / BLK +
                      ((blk_total % BLK) == 0 ? 0 : 1);
  Assert(reader_count < BLK, "merge sort cannot have readers more than %d", BLK - 1);
  iterator *readers[BLK - 1] = {NULL};
  for (int i = 0; i < reader_count; i++)
    readers[i] = iterator_init(left + i * BLK, left + min((i + 1) * BLK, right), NULL);
  buffered_queue *q = buffered_queue_init(1, target, true);
  iterator *reader;
  while ((reader = TPMM_reader_select(readers)) != NULL)
    buffered_queue_push(q, iterator_next(reader));
  for (int i = 0; i < reader_count; i++)
    iterator_free(readers[i]);
  buffered_queue_flush(q);
  buffered_queue_free(q);
}

void TPMMS(uint left, uint right, uint target) {
  uint temp = target + 100;
  TPMMS_sort_subsets(left, right, temp);
  TPMMS_merge_sort(temp, temp + (right - left), target);
}

void q2() {
  Log("============================");
  Log("Q2: 两阶段多路归并排序算法(TPMMS)");
  Log("利用内存缓冲区将关系 R 和 S 分别排序,并将排序后的结果存放在磁盘上。");
  Log("============================");
  buffer_init();
  TPMMS(1, 17, 301);
  TPMMS(17, 49, 317);
  buffer_report();
  buffer_free();
}