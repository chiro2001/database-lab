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

#define iter_handler(x) void (*(x))(char*)

#define lambda(return_type, function_body) \
  ({return_type lfunc function_body lfunc;})


typedef uint uint;

char *itoa(uint i);

char *itot(uint a, uint b);

uint atoi3(char *s);

char *readBlock(uint addr);

void freeBlock(char *blk);

void iterate_range(uint left, uint right, iter_handler(handler));

extern Buffer g_buf;

/// Some arguments
#define BLK_SZ 64
#define BLK 8

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define CMP(a, b) (a)[0] < (b)[0] ? true : \
                  (a)[1] < (b)[1] ? true : \
                  (a)[2] < (b)[2]
#define SWAP_(a, b) do { char t = (a); (a) = (b); (b) = t; } while (0)
#define SWAP(a, b) do { SWAP_((a)[0], (b)[0]); SWAP_((a)[1], (b)[1]); SWAP_((a)[2], (b)[2]); } while (0)

#endif //LAB5_MAIN_UTILS_H
