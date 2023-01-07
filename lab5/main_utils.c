//
// Created by chiro on 23-1-6.
//

#include "main_utils.h"
#include "cache.h"

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

bool cmp_greater(char *a, char *b) {
  uint i = atoi3(a);
  uint j = atoi3(b);
  return i > j;
}

void tuple_copy(char *dest, char *src) {
  memcpy(dest, src, 8);
}

/**
 * 全局的缓冲区
 */
Buffer g_buf;

void buffer_init() {
  Assert(initBuffer((BLK * (BLK_SZ + 1)), BLK_SZ, &g_buf),
         "Buffer Initialization Failed!\n");
}

void buffer_init_large() {
  uint large_block = BLK * 128;
  Assert(initBuffer((large_block * (BLK_SZ + 1)), large_block, &g_buf),
         "Buffer Initialization Failed!\n");
}

void buffer_free() {
  freeBuffer(&g_buf);
}

void buffer_report() {
  Log("Buffer: IO 读写一共 %lu 次", g_buf.numIO);
}

char *read_block(uint addr) {
  char *blk = NULL;
  Assert(NULL != (blk = (char *) readBlockFromDisk(addr, &g_buf)),
         "Reading Block %u Failed, free blocks in buf: %zu", addr, g_buf.numFreeBlk);
  return blk;
}

char *read_block_try(uint addr) {
  return (char *) readBlockFromDisk(addr, &g_buf);
}

char *allocBlock() {
  return (char *) getNewBlockInBuffer(&g_buf);
}

void free_block(char *blk) {
  freeBlockInBuffer((unsigned char *) blk, &g_buf);
}

void iterate_range(uint left, uint right, iter_handler(handler)) {
  Dbg("iterate_range(%d, %d)", left, right);
  uint block = left;
  while (block != 0) {
    char *blk = NULL;
    Dbg("loading block %d", block);
    blk = read_block(block);
    if (handler != NULL)
      for (int i = 0; i < 7; i++)
        if (!handler(blk + i * 8)) return;
    block = atoi3(blk + 56);
    free_block(blk);
    if (block == right) break;
  }
}

char *block_tuple_tail(char *blk) {
  for (int i = 0; i < 7; i++) {
    if (*(blk + 8) == '\0') return blk;
    blk += 8;
  }
  return NULL;
}

void iterate_range_show(uint left, uint right) {
  printf("======== data [%d, %d):\n", left, right);
  uint cnt = 0;
  uint addr = left;
  iterate_range(left, right, lambda(bool, (char *s) {
      if (*s != '\0') {
      if (cnt == 0) {
      printf("[%3d] ", addr);
  }
      printf("(%s, %s) ", s, s + 4);
      if ((cnt++) == 6) {
      puts("");
      cnt = 0;
      addr++;
  }
  }
      return true;
  }));
  if (cnt != 0) puts("");
}
