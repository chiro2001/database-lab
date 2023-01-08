//
// Created by chiro on 23-1-6.
//

#ifndef LAB5_MAIN_UTILS_H
#define LAB5_MAIN_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <debug_macros.h>
#include "extmem.h"

#define SEQ2(a, b) (((a)[0]==(b)[0])&&((a)[1]==(b)[1]))
#define SEQ3(a, b) (SEQ2(a,b)&&((a)[2]==(b)[2]))
#define SEQ(a, b) SEQ3(a, b)
#define SEQ_T(a, b) (SEQ3(a, b) && SEQ3((a) + 4, (b) + 4))

#define iter_handler(x) bool (*(x))(char*)
#define handle_iter(x) (bool (*)(char*))(x)

#define lambda(return_type, function_body) \
  ({return_type lfunc function_body lfunc;})


typedef uint uint;

char *itoa(uint i);

char *itot(uint a, uint b);

uint atoi3(const char *s);

void buffer_init();

void buffer_init_large();

void buffer_free();

void buffer_report();

void buffer_report_msg(const char *msg);

char *read_block(uint addr);

char *read_block_try(uint addr);

char *allocBlock();

void free_block(char *blk);

void iterate_range(uint left, uint right, iter_handler(handler));

void iterate_range_show(uint left, uint right);

void iterate_range_show_some(uint left, uint right);

bool cmp_greater(char *a, char *b);

void tuple_copy(char *dest, char *src);

char *block_tuple_tail(char *blk);

extern Buffer g_buf;

/// Some arguments
#define BLK_SZ 64
#define BLK 8

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define CMP(a, b) ((a)[0] < (b)[0] ? true : \
                   (a)[1] < (b)[1] ? true : \
                   (a)[2] < (b)[2])
#define SWAP_(a, b) do { char t = (a); (a) = (b); (b) = t; } while (0)
#define SWAP(a, b) do { \
    SWAP_((a)[0], (b)[0]); SWAP_((a)[1], (b)[1]); SWAP_((a)[2], (b)[2]); \
    SWAP_((a)[4], (b)[4]); SWAP_((a)[5], (b)[5]); SWAP_((a)[6], (b)[6]); \
  } while (0)
#define SWAP_U(a, b) do { uint t = (a); (a) = (b); (b) = t; } while (0)

#endif //LAB5_MAIN_UTILS_H
