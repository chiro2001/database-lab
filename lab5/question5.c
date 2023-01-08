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

uint union_stage(iterator* reader_first, iterator* reader_second, buffered_queue *target) {
  char last_insert[9] = "";
  char *a = NULL;
  char *b = NULL;
  uint skipped = 0;
  while (true) {
    a = iterator_now(reader_first);
    b = iterator_now(reader_second);
    if (a == NULL || b == NULL) break;
    if (cmp_greater(a, b) ||
        (SEQ3(a, b) && cmp_greater(a + 4, b + 4))) {
      // a > b
      // 与上次插入不同，可以插入 b
      if (!SEQ_T(b, last_insert)) {
        buffered_queue_push(target, b);
        tuple_copy(last_insert, b);
      } else skipped++;
      iterator_next(reader_second);
    } else {
      // a <= b
      // 与上次插入不同，可以插入 a
      if (!SEQ_T(a, last_insert)) {
        buffered_queue_push(target, a);
        tuple_copy(last_insert, a);
      } else skipped++;
      iterator_next(reader_first);
    }
  }
  // 插入剩下的 b
  while (a == NULL && b != NULL) {
    if (!SEQ_T(b, last_insert)) {
      buffered_queue_push(target, b);
      tuple_copy(last_insert, b);
    } else skipped++;
    iterator_next(reader_second);
    b = iterator_now(reader_second);
  }
  // 插入剩下的 a
  while (a != NULL && b == NULL) {
    if (!SEQ_T(a, last_insert)) {
      buffered_queue_push(target, a);
      tuple_copy(last_insert, a);
    } else skipped++;
    iterator_next(reader_first);
    a = iterator_now(reader_first);
  }
  return skipped;
}

uint intersect_stage(iterator* reader_first, iterator* reader_second, buffered_queue *target) {
  char last_top[9] = "";
  char *a = NULL;
  char *b = NULL;
  uint skipped = 0;
  while (true) {
    a = iterator_now(reader_first);
    b = iterator_now(reader_second);
    if (a == NULL || b == NULL) break;
    if (cmp_greater(a, b) ||
        (SEQ3(a, b) && cmp_greater(a + 4, b + 4))) {
      // a > b, b 与上次的最小值相同则插入 b
      if (SEQ_T(b, last_top)) {
        buffered_queue_push(target, b);
      } else skipped++;
      tuple_copy(last_top, b);
      iterator_next(reader_second);
    } else {
      // a <= b, a 与上次最小的值相同则插入 a
      if (SEQ_T(a, last_top)) {
        buffered_queue_push(target, a);
      } else skipped++;
      tuple_copy(last_top, a);
      iterator_next(reader_first);
    }
  }
  // 去重插入剩下的 b
  while (a == NULL && b != NULL) {
    iterator_next(reader_second);
    if (SEQ_T(b, last_top)) {
      buffered_queue_push(target, b);
      tuple_copy(last_top, b);
    } else skipped++;
    b = iterator_now(reader_second);
  }
  // 去重插入剩下的 a
  while (a != NULL && b == NULL) {
    iterator_next(reader_first);
    if (SEQ_T(a, last_top)) {
      buffered_queue_push(target, a);
      tuple_copy(last_top, a);
    } else skipped++;
    a = iterator_now(reader_first);
  }
  return skipped;
}

uint difference_set_stage(iterator* reader_first, iterator* reader_second, buffered_queue *target) {
  char *a = NULL;
  char *b = NULL;
  uint skipped = 0;
  while (true) {
    a = iterator_now(reader_first);
    b = iterator_now(reader_second);
    if (a == NULL || b == NULL) break;
    if (cmp_greater(a, b) ||
        (SEQ3(a, b) && cmp_greater(a + 4, b + 4))) {
      // a > b
      skipped++;
      // a != b, do not insert b
      iterator_next(reader_second);
    } else {
      // a <= b
      if (!SEQ_T(a, b)) {
        // a != b, insert a
        buffered_queue_push(target, a);
      } else {
        // a == b, do not insert a or b
        skipped += 2;
        iterator_next(reader_second);
      }
      iterator_next(reader_first);
    }
  }
  while (a == NULL && b != NULL) {
    skipped++;
    iterator_next(reader_second);
    b = iterator_now(reader_second);
  }
  while (a != NULL && b == NULL) {
    buffered_queue_push(target, a);
    iterator_next(reader_first);
    a = iterator_now(reader_first);
  }
  return skipped;
}

uint two_stage_scanning(uint s_left, uint s_right, uint r_left, uint r_right, uint target,
                        uint (*fn)(iterator*, iterator*, buffered_queue*)) {
  uint temp1 = 600, temp2 = 700;
  // 对两个数据源排序并去重
  uint skipped_s = sort_deduplicate_two_stage_scanning(s_left, s_right, temp1);
  uint skipped_r = sort_deduplicate_two_stage_scanning(r_left, r_right, temp2);
  buffered_queue *q = buffered_queue_init(1, target, true);
  uint first_right = temp1 + (s_right - s_left) - skipped_s / 7;
  uint second_right = temp2 + (r_right - r_left) - skipped_r / 7;
  iterator *reader_first = iterator_init(temp1, first_right, NULL);
  iterator *reader_second = iterator_init(temp2, second_right, NULL);
  // 调用函数钩子
  uint skipped_stage2 = fn(reader_first, reader_second, q);
  buffered_queue_flush(q);
  buffered_queue_free(q);
  return skipped_s + skipped_r + skipped_stage2;
}

void q5() {
  Log("=====================");
  Log("Q5: 基于排序的两趟扫描算法");
  Log("=====================");

  buffer_init();
  uint target = 300;
  Log("计算 S union R");
  uint skipped = two_stage_scanning(1, 17, 17, 49, target, union_stage);
  buffer_report_msg("计算 S union R");
  uint right = target + 48 - skipped / 7;
  Log("计算结果储存于 [%d, %d]", target, right - 1);
  buffer_free();

  buffer_init_large();
  buffered_queue *q = buffered_queue_init(256, -1, false);
  buffered_queue_load_from(q, target, right);
  Log("S union R 结果元组数量为 %d", buffered_queue_count(q));
  buffered_queue_free(q);
  iterate_range_show_some(target, right);
  buffer_free();

  buffer_init();
  target = 350;
  Log("计算 S intersects R");
  skipped = two_stage_scanning(1, 17, 17, 49, target, intersect_stage);
  buffer_report_msg("计算 S intersects R");
  right = target + 48 - skipped / 7;
  Log("计算结果储存于 [%d, %d]", target, right - 1);
  buffer_free();

  buffer_init_large();
  q = buffered_queue_init(256, -1, false);
  buffered_queue_load_from(q, target, right);
  Log("S intersects R 结果元组数量为 %d", buffered_queue_count(q));
  buffered_queue_free(q);
  iterate_range_show_some(target, right);
  buffer_free();

  buffer_init();
  target = 360;
  Log("计算 S - R");
  skipped = two_stage_scanning(17, 49, 1, 17, target, difference_set_stage);
  buffer_report_msg("计算 S - R");
  right = target + 48 - skipped / 7;
  Log("计算结果储存于 [%d, %d]", target, right - 1);
  buffer_free();

  buffer_init_large();
  q = buffered_queue_init(256, -1, false);
  buffered_queue_load_from(q, target, right);
  Log("S - R 结果元组数量为 %d", buffered_queue_count(q));
  buffered_queue_free(q);
  iterate_range_show_some(target, right);
  buffer_free();
}