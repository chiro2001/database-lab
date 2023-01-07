//
// Created by chiro on 23-1-6.
//

#ifndef LAB5_QUESTIONS_H
#define LAB5_QUESTIONS_H

#include "iterator.h"
#include "buffered_queue.h"

void q1();

void q2();

void q3();

void q4();

void q5();

void TPMMS_sort_subset(uint left, uint right, uint target, bool continuous);

void TPMMS_sort_subsets(uint left, uint right, uint target);

iterator *TPMM_reader_select(iterator *readers[BLK - 1]);

void TPMMS_merge_sort(uint left, uint right, uint target);

void TPMMS(uint left, uint right, uint target);

void create_index_range(uint left, uint right, uint addr);

void indexed_select_linear(uint left, uint right, uint key, buffered_queue *target);

#endif //LAB5_QUESTIONS_H
