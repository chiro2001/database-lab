//
// Created by chiro on 23-1-6.
//

#include "main_utils.h"
#include "cache.h"

/**
 * 数字转字符串。⚠️不能连续使用其指针返回值。
 * @param i
 * @return
 */
char *itoa_(uint i) {
  static char itoa_buffer[5];
  sprintf(itoa_buffer, "%d", (int) i);
  return itoa_buffer;
}

/**
 * 数字转 tuple。⚠️不能连续使用其指针返回值。
 * @param a
 * @param b
 * @return
 */
char *itot(uint a, uint b) {
  static char itot_buffer[9];
  Assert(100 <= a && a <= 999, "a out of range");
  Assert(100 <= b && b <= 999, "b out of range");
  sprintf(itot_buffer, "%u %u", a, b);
  itot_buffer[3] = '\0';
  return itot_buffer;
}

/**
 * 数字转 tuple，没有值域限制。⚠️不能连续使用其指针返回值。
 * @param a
 * @param b
 * @return
 */
char *itot3(uint a, uint b) {
  static char itot_buffer[9];
  sprintf(itot_buffer, "%3u %3u", a, b);
  itot_buffer[3] = '\0';
  return itot_buffer;
}

/**
 * 至多三位数字符串转数字
 * @param s
 * @return
 */
uint atoi3(const char *s) {
  int a = s[0] - '0', b = s[1] - '0', c = s[2] - '0';
  // Log("s: %s, v: %d%d%d", s, a, b, c);
  if (s[0] == '\0') return 0;
  if (s[1] == '\0') return a;
  if (s[2] == '\0') return a * 10 + b;
  return a * 100 + b * 10 + c;
}

/**
 * 判断 tuple a > tuple b
 * @param a
 * @param b
 * @return a > b
 */
bool cmp_greater(char *a, char *b) {
  uint i = atoi3(a);
  uint j = atoi3(b);
  return i > j;
}

/**
 * 复制一个 tuple
 * @param dest
 * @param src
 */
void tuple_copy(char *dest, char *src) {
  memcpy(dest, src, 8);
}

/**
 * 全局的缓冲区
 */
Buffer g_buf;

/**
 * 初始化全局缓冲区
 */
void buffer_init() {
  Assert(initBuffer((BLK * (BLK_SZ + 1)), BLK_SZ, &g_buf),
         "Buffer Initialization Failed!\n");
}

/**
 * 初始化一个超大的全局缓冲区，在题目外对空间无要求的时候使用
 */
void buffer_init_large() {
  uint large_block = BLK * 128;
  Assert(initBuffer((large_block * (BLK_SZ + 1)), BLK_SZ, &g_buf),
         "Buffer Initialization Failed!\n");
}

/**
 * 释放全局缓冲区
 */
void buffer_free() {
  freeBuffer(&g_buf);
}

/**
 * 报告缓冲区使用情况
 */
void buffer_report() {
  Log("Buffer: IO 读写一共 %lu 次", g_buf.numIO);
}

/**
 * 报告缓冲区在 [[msg]] 期间使用情况
 * @param msg
 */
void buffer_report_msg(const char *msg) {
  Log("%s: IO 读写一共 %lu 次", msg, g_buf.numIO);
}

/**
 * 读一个块的数据到一个新建的全局缓冲区块
 * @param addr
 * @return 缓冲区地址
 */
char *read_block(uint addr) {
  char *blk = NULL;
  Assert(NULL != (blk = (char *) readBlockFromDisk(addr, &g_buf)),
         "Reading Block %u Failed, free blocks in buf: %zu", addr, g_buf.numFreeBlk);
  return blk;
}

/**
 * 尝试性地读一个块数据到全局缓冲区块，读取失败则返回 NULL
 * @param addr
 * @return
 */
char *read_block_try(uint addr) {
  return (char *) readBlockFromDisk(addr, &g_buf);
}

/**
 * 申请全局缓冲区块
 * @return
 */
char *alloc_block() {
  char *b = (char *) getNewBlockInBuffer(&g_buf);
  Assert(b, "cannot allocate new block!");
  return b;
}

/**
 * 释放一个全局缓冲区块
 * @param blk
 */
void free_block(char *blk) {
  freeBlockInBuffer((unsigned char *) blk, &g_buf);
}

/**
 * 在磁盘 [[left]] - [[right]] 块中对每一个数据项目调用 handler
 * @param left 左闭区间参数
 * @param right 右开区间参数
 * @param handler 参数为数据项目，返回是否继续迭代
 */
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

/**
 * 格式化输出磁盘块 [left], right)
 * @param left
 * @param right
 */
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
    fflush(stdout);
    return true;
  }));
  if (cnt != 0) puts("");
}

/**
 * 格式化输出 [left, right)，如果项目多于一定数量则省略中间部分数据的输出
 * @param left
 * @param right
 */
void iterate_range_show_some(uint left, uint right) {
  if (right == -1 || (right != -1 && right - left <= 4)) {
    iterate_range_show(left, right);
    return;
  }
  printf("======== data [%d, %d):\n", left, right);
  uint cnt = 0;
  uint addr = left;
  bool points = false;
  iterate_range(left, right, lambda(bool, (char *s) {
    if (*s != '\0') {
      if (addr - left < 2 || right - addr <= 2) {
        if (cnt == 0) {
          printf("[%3d] ", addr);
        }
        printf("(%s, %s) ", s, s + 4);
      } else {
        if (!points) {
          points = true;
          printf("     ... ...\n");
        }
      }
      if ((cnt++) == 6) {
        if (addr - left < 2 || right - addr <= 2)
          puts("");
        cnt = 0;
        addr++;
      }
    }
    fflush(stdout);
    return true;
  }));
  if (cnt != 0) puts("");
}
