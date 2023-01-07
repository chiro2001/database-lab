//
// Created by chiro on 23-1-7.
//
#include "main_utils.h"
#include "buffered_queue.h"
#include "questions.h"

void range_sorted_deduplication(uint left, uint right, uint target) {
  buffered_queue *q = buffered_queue_init(1, target, true);
  char *last_insert = malloc(sizeof(char) * 9);
  *last_insert = '\0';
  iterate_range(left, right, lambda(bool, (char *s) {
    if (!SEQ_T(last_insert, s)) {
      buffered_queue_push(q, s);
      tuple_copy(last_insert, s);
    }
    return true;
  }));
  buffered_queue_flush(q);
  buffered_queue_free(q);
  free(last_insert);
}

void deduplication_subsets(uint left, uint right, uint target) {
  uint blk_total = right - left;
  uint rounds = blk_total / BLK +
                ((blk_total % BLK) == 0 ? 0 : 1);
  for (uint r = 0; r < rounds; r++)
    range_sorted_deduplication(
        left + r * BLK,
        left + (r + 1) * BLK,
        target + r * BLK);
}

void deduplication_merge_subsets(uint left, uint right, uint target) {
  Dbg("deduplication_merge_subsets");
  uint blk_total = right - left;
  uint reader_count = blk_total / BLK +
                      ((blk_total % BLK) == 0 ? 0 : 1);
  Assert(reader_count < BLK, "cannot have readers more than %d", BLK - 1);
  iterator *readers[BLK - 1] = {NULL};
  for (int i = 0; i < reader_count; i++)
    readers[i] = iterator_init(left + i * BLK, left + min((i + 1) * BLK, right), NULL);
  buffered_queue *q = buffered_queue_init(1, target, true);
  iterator *reader;
  char *last_insert = malloc(sizeof(char) * 9);
  *last_insert = '\0';
  while ((reader = TPMM_reader_select(readers)) != NULL) {
    char *s = iterator_now(reader);
    if (!SEQ_T(last_insert, s)) {
      buffered_queue_push(q, s);
      tuple_copy(last_insert, s);
    }
    iterator_next(reader);
  }
  for (int i = 0; i < reader_count; i++)
    iterator_free(readers[i]);
  buffered_queue_flush(q);
  buffered_queue_free(q);
}

void deduplication_two_stage_scanning(uint left, uint right, uint target) {
  uint blk_total = right - left;
  uint temp1 = target + 2000;
  TPMMS_sort_subsets(left, right, temp1);
  uint temp2 = target + 3000;
  deduplication_subsets(temp1, temp1 + blk_total, temp2);
  deduplication_merge_subsets(temp2, temp2 + blk_total, target);
}

void q5() {
  Log("==================");
  Log("基于排序的两趟扫描算法");
  Log("实现 S U R");
  Log("==================");
  buffer_init();
  TPMMS(1, 17, 301);
  TPMMS(17, 49, 317);

  // union_two_stage_scanning()

  buffer_report();
  buffer_free();
}