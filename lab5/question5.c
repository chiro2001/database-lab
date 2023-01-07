//
// Created by chiro on 23-1-7.
//
#include "main_utils.h"
#include "buffered_queue.h"
#include "questions.h"
#include "iterator.h"

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

void sort_deduplication_two_stage_scanning(uint left, uint right, uint target) {
  uint blk_total = right - left;
  uint temp1 = 400;
  TPMMS_sort_subsets(left, right, temp1);
  Log("sorted subsets:");
  iterate_range_show(temp1, temp1 + blk_total);
  uint temp2 = 500;
  deduplication_subsets(temp1, temp1 + blk_total, temp2);
  Log("deduplication subsets:");
  iterate_range_show(temp2, temp2 + blk_total);
  deduplication_merge_subsets(temp2, temp2 + blk_total, target);
  Log("merged deduplication:");
  iterate_range_show(target, -1);
}

void union_two_stage_scanning(uint first, uint second, uint target) {
  buffered_queue *q = buffered_queue_init(1, target, true);
  iterator *reader_first = iterator_init(first, -1, NULL);
  iterator *reader_second = iterator_init(first, -1, NULL);
  char *last_insert = malloc(sizeof(char) * 9);
  *last_insert = '\0';
  char *a = NULL;
  char *b = NULL;
  while (true) {
    a = iterator_now(reader_first);
    b = iterator_now(reader_second);
    if (a == NULL || b == NULL) break;
    if (cmp_greater(a, b)) {
      if (!SEQ_T(b, last_insert)) {
        buffered_queue_push(q, b);
        tuple_copy(last_insert, b);
      }
      iterator_next(reader_second);
    } else {
      if (!SEQ_T(a, last_insert)) {
        buffered_queue_push(q, a);
        tuple_copy(last_insert, a);
      }
      iterator_next(reader_first);
    }
  }
  while (a == NULL && b != NULL) {
    buffered_queue_push(q, b);
    b = iterator_next(reader_second);
  }
  while (a != NULL && b == NULL) {
    buffered_queue_push(q, a);
    a = iterator_next(reader_first);
  }
  buffered_queue_flush(q);
}

void q5() {
  Log("==================");
  Log("基于排序的两趟扫描算法");
  Log("实现 S U R");
  Log("==================");
  buffer_init();
  sort_deduplication_two_stage_scanning(1, 17, 500);
  sort_deduplication_two_stage_scanning(17, 49, 600);
  union_two_stage_scanning(500, 600, 700);

  iterate_range_show(700, -1);

  buffer_report();
  buffer_free();
}