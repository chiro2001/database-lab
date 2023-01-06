//
// Created by chiro on 23-1-6.
//

#include "main_utils.h"

char *itoa(unsigned int i) {
  static char itoa_buffer[5];
  sprintf(itoa_buffer, "%d", i);
  return itoa_buffer;
}

unsigned int atoi3(char *s) {
  if (s[0] == '\0') return 0;
  if (s[1] == '\0') return s[0] - '0';
  if (s[2] == '\0') return (s[0] - '0') * 10 + (s[1] - '0');
  return (s[0] - '0') * 100 + (s[1] - '0') * 10 + (s[0] - '0');
}

/**
 * 全局的缓冲区
 */
Buffer buf;

char *readBlock(unsigned int addr) {
  char *blk = NULL;
  Assert(NULL != (blk = (char *) readBlockFromDisk(addr, &buf)), "Reading Block %u Failed", addr);
  return blk;
}

void freeBlock(char *blk) {
  freeBlockInBuffer((unsigned char *) blk, &buf);
}

