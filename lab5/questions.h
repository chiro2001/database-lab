//
// Created by chiro on 23-1-6.
//

#ifndef LAB5_QUESTIONS_H
#define LAB5_QUESTIONS_H

#include "iterator.h"

void q1();
void q2();
void q3();
void q4();
void q5();

void TPMMS_sort_subset(uint left, uint right, uint target, bool continuous, bool create_index);
void TPMMS_sort_subsets(uint left, uint right, uint target, bool create_index);
iterator *TPMM_reader_select(iterator *readers[BLK - 1]);
void TPMMS_merge_sort(uint left, uint right, uint target);
void TPMMS(uint left, uint right, uint target, bool create_index);

#endif //LAB5_QUESTIONS_H
