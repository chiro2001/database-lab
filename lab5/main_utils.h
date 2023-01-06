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

#define lambda( return_type, function_body) \
  ({return_type lfunc function_body lfunc;})


typedef uint uint;

char *itoa(uint i);

uint atoi3(char *s);

char *readBlock(uint addr);

void freeBlock(char *blk);

void data_iterate(uint left, uint right, iter_handler(handler));

extern Buffer buf;

#endif //LAB5_MAIN_UTILS_H
