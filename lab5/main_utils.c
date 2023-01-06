//
// Created by chiro on 23-1-6.
//

#include "main_utils.h"

char *itoa(uint i) {
  static char itoa_buffer[5];
  sprintf(itoa_buffer, "%d", (int) i);
  return itoa_buffer;
}

char *itot(uint a, uint b) {
  static char itot_buffer[9];
  Assert(100 <= a && a <= 999, "a out of range");
  Assert(100 <= b && b <= 999, "b out of range");
  sprintf(itot_buffer, "%u %u", a, b);
  itot_buffer[3] = '\0';
  return itot_buffer;
}

uint atoi3(const char *s) {
  int a = s[0] - '0', b = s[1] - '0', c = s[2] - '0';
  // Log("s: %s, v: %d%d%d", s, a, b, c);
  if (s[0] == '\0') return 0;
  if (s[1] == '\0') return a;
  if (s[2] == '\0') return a * 10 + b;
  return a * 100 + b * 10 + c;
}

/**
 * 全局的缓冲区
 */
Buffer g_buf;

char *readBlock(uint addr) {
  char *blk = NULL;
  Assert(NULL != (blk = (char *) readBlockFromDisk(addr, &g_buf)), "Reading Block %u Failed", addr);
  return blk;
}

char *allocBlock() {
  return (char *) getNewBlockInBuffer(&g_buf);
}

void freeBlock(char *blk) {
  freeBlockInBuffer((unsigned char *) blk, &g_buf);
}

void iterate_range(uint left, uint right, iter_handler(handler)) {
  Log("iterate_range(%d, %d)", left, right);
  uint block = left;
  while (block != 0) {
    char *blk = NULL;
    Dbg("loading block %d", block);
    blk = readBlock(block);
    if (handler != NULL)
      for (int i = 0; i < 7; i++)
        handler(blk + i * 8);
    block = atoi3(blk + 56);
    freeBlock(blk);
    if (block == right) break;
  }
}