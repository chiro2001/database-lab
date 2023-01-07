//
// Created by chiro on 23-1-7.
//
#include "main_utils.h"
#include "buffered_queue.h"
#include "questions.h"
#include "iterator.h"

uint range_sorted_deduplicate(uint left, uint right, uint target) {
  buffered_queue *q = buffered_queue_init(1, target, true);
  char *last_insert = malloc(sizeof(char) * 9);
  *last_insert = '\0';
  uint skipped = 0;
  iterate_range(left, right, lambda(bool, (char *s) {
    if (!SEQ_T(last_insert, s)) {
      buffered_queue_push(q, s);
      tuple_copy(last_insert, s);
    } else skipped++;
    return true;
  }));
  buffered_queue_flush(q);
  buffered_queue_free(q);
  free(last_insert);
  return skipped;
}

uint deduplicate_subsets(uint left, uint right, uint target) {
  uint blk_total = right - left;
  uint rounds = blk_total / BLK +
                ((blk_total % BLK) == 0 ? 0 : 1);
  uint skipped = 0;
  for (uint r = 0; r < rounds; r++)
    skipped += range_sorted_deduplicate(
        left + r * BLK,
        left + (r + 1) * BLK,
        target + r * BLK);
  return skipped;
}

uint deduplicate_merge_subsets(uint left, uint right, uint target) {
  Dbg("deduplicate_merge_subsets");
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
  uint skipped = 0;
  while ((reader = TPMM_reader_select(readers)) != NULL) {
    char *s = iterator_now(reader);
    if (!SEQ_T(last_insert, s)) {
      buffered_queue_push(q, s);
      tuple_copy(last_insert, s);
    } else skipped++;
    iterator_next(reader);
  }
  for (int i = 0; i < reader_count; i++)
    iterator_free(readers[i]);
  buffered_queue_flush(q);
  buffered_queue_free(q);
  return skipped;
}

uint sort_deduplicate_two_stage_scanning(uint left, uint right, uint target) {
  uint blk_total = right - left;
  uint temp1 = 400;
  TPMMS_sort_subsets(left, right, temp1);
  // Log("sorted subsets:");
  // iterate_range_show(temp1, temp1 + blk_total);
  uint temp2 = 500;
  uint skipped = deduplicate_subsets(temp1, temp1 + blk_total, temp2);
  // Log("deduplicated subsets:");
  // iterate_range_show(temp2, temp2 + blk_total);
  skipped += deduplicate_merge_subsets(temp2, temp2 + blk_total, target);
  // Log("merged deduplicate:");
  // iterate_range_show(target, -1);
  return skipped;
}

void union_two_stage_scanning(uint first, uint second, uint target) {
  buffered_queue *q = buffered_queue_init(1, target, true);
  iterator *reader_first = iterator_init(first, -1, NULL);
  iterator *reader_second = iterator_init(second, -1, NULL);
  char *last_insert = malloc(sizeof(char) * 9);
  *last_insert = '\0';
  char *a = NULL;
  char *b = NULL;
  Dbg("union_two_stage_scanning started");
  while (true) {
    a = iterator_now(reader_first);
    b = iterator_now(reader_second);
    if (a == NULL || b == NULL) break;
    if (cmp_greater(a, b) ||
        (SEQ3(a, b) && cmp_greater(a + 4, b + 4))) {
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
  Dbg("union_two_stage_scanning finished");
  buffered_queue_flush(q);
  buffered_queue_free(q);
}

void union_SUR(uint s_left, uint s_right, uint r_left, uint r_right, uint target) {
  uint temp1 = 600, temp2 = 700;
  // Log(" == S source == ");
  // iterate_range_show(s_left, s_right);
  // Log(" == R source == ");
  // iterate_range_show(r_left, r_right);
  uint skipped_s = sort_deduplicate_two_stage_scanning(s_left, s_right, temp1);
  // Log(" == S deduplicated == ");
  // iterate_range_show(temp1, temp1 + (s_right - s_left));
  uint skipped_r = sort_deduplicate_two_stage_scanning(r_left, r_right, temp2);
  // Log(" == R deduplicated == ");
  // iterate_range_show(temp2, temp2 + (r_right - r_left));
  uint skipped = skipped_s + skipped_r;

  // union_two_stage_scanning(temp1, temp2, target);
  uint temp3 = 300;
  // sort_deduplicate_two_stage_scanning(temp1, s_right, temp1);
  buffered_queue *q = buffered_queue_init(1, temp3, true);
  iterate_range(temp1, -1, lambda(bool, (char *s) {
    if (*s != '\0') {
      buffered_queue_push(q, s);
    }
    return *s != '\0';
  }));
  iterate_range(temp2, -1, lambda(bool, (char *s) {
      if (*s != '\0') {
      buffered_queue_push(q, s);
  }
      return *s != '\0';
  }));
  buffered_queue_flush(q);
  buffered_queue_free(q);
  uint total = (s_right - s_left) + (r_right - r_left);
  sort_deduplicate_two_stage_scanning(temp3, temp3 + total - (skipped / 7), target);
}

void q5() {
  Log("==================");
  Log("基于排序的两趟扫描算法");
  Log("实现 S U R");
  Log("==================");
  // buffer_init();
  buffer_init_large();

  uint target = 900;
  union_SUR(1, 17, 17, 49, target);

  buffered_queue *q = buffered_queue_init(64, -1, false);
  iterate_range_show(target, -1);
  // iterate_range(target, -1, lambda(bool, (char *s) {
  //   if (*s != '\0') {
  //     // Log("pusing (%s, %s)", s, s + 4);
  //     buffered_queue_push(q, s);
  //   }
  //   return *s != '\0';
  // }));
  // buffered_queue_show(q);
  // buffered_queue_free(q);

  buffer_report();
  buffer_free();
}