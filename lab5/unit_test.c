//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "buffered_queue.h"
#include "cache.h"

#define DATA_SZ_MAX 512

typedef struct {
  uint a, b;
} tuple;

typedef struct {
  tuple *r, *s;
} data_mem;

tuple *load_range(uint left, uint right) {
  tuple *list = malloc(sizeof(tuple) * DATA_SZ_MAX);
  memset(list, 0, sizeof(tuple) * DATA_SZ_MAX);
  tuple *p = list;
  iterate_range(left, right, lambda(bool, (char *s) {
    if (*s) {
      p->a = atoi3(s);
      p->b = atoi3(s + 4);
      p++;
    }
    return true;
  }));
  return list;
}

void tuple_list_append(tuple *dest, tuple *src) {
  tuple *p = src;
  while (p->a > 0) p++;
  memcpy(dest, src, p - src);
}

data_mem *load_all() {
  tuple r_range = {1, 17};
  tuple s_range = {17, 48};
  data_mem *d = malloc(sizeof(data_mem));
  d->r = load_range(r_range.a, r_range.b);
  d->s = load_range(r_range.a, r_range.b);
  return d;
}

int main() {
  srand(time(NULL));
  buffer_init();
  Log("TEST: CMP");
  Assert(CMP("123", "456"), "CMP Failed!");
  Assert(CMP("123", "124"), "CMP Failed!");
  Assert(CMP("100", "101"), "CMP Failed!");
  Assert(!CMP("100", "100"), "CMP Failed!");
  Assert(CMP("333", "334"), "CMP Failed!");
  Log("TEST: Buffer size");
  Log("buffer free size: %zu", g_buf.numFreeBlk);
  Log("TEST: buffered queue sort");
  int queue_test_blk = 1;
  buffered_queue *q = buffered_queue_init(queue_test_blk, -1, false);
  for (int i = 0; i < queue_test_blk * 7; i++) {
    buffered_queue_push(q, itot(100 + rand() % 100, 200 + rand() % 200));
  }
  buffered_queue_sort(q, 0);
  buffered_queue_iterate(q, lambda(bool, (char *c) {
      Log("(%s, %s)", c, c + 4); return true;
  }));
  buffered_queue_free(q);
  Log("TEST: atoi3");
  Assert(atoi3("302") == 302, "atoi3 error, return value: %d", atoi3("302"));
  Assert(atoi3("303") == 303, "atoi3 error, return value: %d", atoi3("303"));
  Assert(atoi3("0") == 0, "atoi3 err");
  Assert(atoi3("40") == 40, "atoi3 err");

  buffer_free();
  buffer_init();
  Log("TEST: buffered cache");
  cache *p = cache_init(2);
  for (int addr = 1; addr < 49; addr++) {
    Assert(cache_read(p, addr) != NULL, "cache read err, addr: %d", addr);
  }
  for (int i = 0; i < 500; i++) {
    Assert(cache_read(p, (rand() % 48) + 1) != NULL, "cache read err, addr: %d", i);
  }
  cache_free(p);
  buffer_free();

  buffer_init();
  Log("TEST: sort merge join");
  int r[] = {20, 30, 30, 30, 31, 33, 35, 0};
  int s[] = {21, 22, 30, 30, 32, 43, 45, 0};
  int res[20] = {0};
  int *p_w = res;
  int *p_r = r;
  int *p_s = s;
  while (*p_r && *p_s) {
    bool overflow = false;
    while (*p_s > *p_r) {
      p_r++;
      if (!*p_r) {
        overflow = true;
        break;
      }
    }
    if (overflow) break;
    if (*p_s == *p_r) {
      int *r_clone = p_r;
      while (*p_s == *r_clone) {
        Log("insert: %d, %d", *p_s, *r_clone);
        *(p_w++) = *p_s;
        *(p_w++) = *r_clone;
        r_clone++;
      }
    }
    p_s++;
  }
  for (int *pp = res; *pp; pp += 2) {
    printf("(%d, %d) ", *pp, *(pp + 1));
  }

  Log("TEST: load data");
  data_mem *data = load_all();
  for (tuple *i = data->r; i->a; i++) {
    Log("(%d, %d)", i->a, i->b);
  }
  Log("TEST: Q1/3 select S.C, S.D from S where S.C = 128");
  for (tuple *i = data->s; i->a; i++) {
    if (i->a == 128)
      Log("(%d, %d)", i->a, i->b);
  }
  buffer_free();
  return 0;
}