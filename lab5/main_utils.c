//
// Created by chiro on 23-1-6.
//

#include "main_utils.h"

char *itoa(uint i) {
  static char itoa_buffer[5];
  sprintf(itoa_buffer, "%d", i);
  return itoa_buffer;
}

uint atoi3(char *s) {
  if (s[0] == '\0') return 0;
  if (s[1] == '\0') return s[0] - '0';
  if (s[2] == '\0') return (s[0] - '0') * 10 + (s[1] - '0');
  return (s[0] - '0') * 100 + (s[1] - '0') * 10 + (s[0] - '0');
}

/**
 * 全局的缓冲区
 */
Buffer buf;

char *readBlock(uint addr) {
  char *blk = NULL;
  Assert(NULL != (blk = (char *) readBlockFromDisk(addr, &buf)), "Reading Block %u Failed", addr);
  return blk;
}

void freeBlock(char *blk) {
  freeBlockInBuffer((unsigned char *) blk, &buf);
}

void data_iterate(uint left, uint right, iter_handler(handler)) {
  uint block = left;
  while (block != 0) {
    char *blk = NULL;
    Dbg("loading block %d", block);
    blk = readBlock(block);
    for (int i = 0; i < 7; i++) {
      handler(blk + i * 8);
    }
    block = atoi3(blk + 56);
    freeBlock(blk);
    if (block == right) break;
  }
}