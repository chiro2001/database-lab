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

#define SWAP_TUPLE(a, b) do { tuple t = (a); (a) = (b); (b) = t; } while (0)

typedef struct {
  tuple *r, *s;
} data_mem;

const uint tuple_sz_max = sizeof(tuple) * DATA_SZ_MAX;

tuple *load_range(uint left, uint right) {
  tuple *list = malloc(tuple_sz_max);
  memset(list, 0, tuple_sz_max);
  tuple *p = list;
  iterate_range(left, right, lambda(bool, (char *s) {
      if (*s) {
      // Log("source (%s, %s)", s, s + 4);
      p->a = atoi3(s);
      p->b = atoi3(s + 4);
      p++;
  }
      return true;
  }));
  return list;
}

data_mem *data_clone(data_mem *src) {
  data_mem *d = malloc(sizeof(data_mem));
  d->r = malloc(tuple_sz_max);
  d->s = malloc(tuple_sz_max);
  memcpy(d->r, src->r, tuple_sz_max);
  memcpy(d->s, src->s, tuple_sz_max);
  return d;
}

void tuple_sort(tuple *l, uint sz, uint order_by) {
  for (uint i = 0; i < sz - 1; i++) {
    for (uint j = 0; j < sz - 1 - i; j++) {
      tuple *n = l + j;
      tuple *m = l + j + 1;
      if (n->a == 0 || m->a == 0) continue;
      if (order_by == 0) { if (n->a > m->a) SWAP_TUPLE(*n, *m); }
      else { if (n->b > m->b) SWAP_TUPLE(*n, *m); }
    }
  }
}

void tuple_sort_all(tuple *l, uint sz) {
  for (uint i = 0; i < sz - 1; i++) {
    for (uint j = 0; j < sz - 1 - i; j++) {
      tuple *n = l + j;
      tuple *m = l + j + 1;
      if (n->a == 0 || m->a == 0) continue;
      if (n->a > m->a) SWAP_TUPLE(*n, *m);
      else if (n->a == m->a && n->b > m->b) SWAP_TUPLE(*n, *m);
    }
  }
}

void data_sort(data_mem *d, uint order_by) {
  tuple_sort(d->r, tuple_sz_max, order_by);
  tuple_sort(d->s, tuple_sz_max, order_by);
}

void tuple_list_append(tuple *dest, tuple *src) {
  tuple *p = src;
  while (p->a > 0) p++;
  memcpy(dest, src, p - src);
}

data_mem *load_all() {
  tuple r_range = {1, 17};
  tuple s_range = {17, 49};
  data_mem *d = malloc(sizeof(data_mem));
  d->r = load_range(r_range.a, r_range.b);
  d->s = load_range(s_range.a, s_range.b);
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
  puts("");

  Log("TEST: load data");
  data_mem *data = load_all();
  // for (tuple *i = data->r; i->a; i++) {
  //   Log("(%d, %d)", i->a, i->b);
  // }
  Log("TEST: Q1/3 select S.C, S.D from S where S.C = 128");
  // for (tuple *i = data->s; i->a; i++) {
  //   if (i->a == 128)
  //     Log("(%d, %d)", i->a, i->b);
  // }
  // data_mem *data2 = data_clone(data);
  // data_sort(data2, 0);
  q = buffered_queue_init(4, -1, false);
  for (tuple *i = data->s; i->a; i++) {
    if (i->a == 128) {
      char *it = itot(i->a, i->b);
      char *ss = malloc(sizeof(char) * 9);
      tuple_copy(ss, it);
      // Log("... (%s, %s)", ss, ss + 4);
      buffered_queue_push(q, ss);
    }
  }
  buffered_queue_sort(q, 1);
  buffered_queue_iterate(q, lambda(bool, (char *s) {
      Log("# (%s, %s)", s, s + 4);
      return true;
  }));
  buffered_queue_free(q);

  buffer_free();

  buffer_init_large();
  Log("TEST: Q5 === S U R");
  tuple *buf = malloc(tuple_sz_max * 2);
  memset(buf, 0, tuple_sz_max * 2);
  uint len_s = 0;
  uint len_r = 0;
  Log(" === S source === ");
  uint cnt = 0;
  for (tuple *i = data->s; i->a; i++) {
    printf("(%d, %d) ", i->a, i->b);
    if ((++cnt) == 7) {
      puts("");
      cnt = 0;
    }
    len_s++;
  }
  if (cnt != 0) puts("");
  cnt = 0;
  Log(" === R source === ");
  for (tuple *i = data->r; i->a; i++) {
    printf("(%d, %d) ", i->a, i->b);
    if ((++cnt) == 7) {
      puts("");
      cnt = 0;
    }
    len_r++;
  }
  if (cnt != 0) puts("");
  memcpy(buf, data->s, sizeof(tuple) * len_s);
  memcpy(buf + len_s, data->r, sizeof(tuple) * len_r);
  uint len = len_s + len_r;
  tuple_sort_all(buf, len);
  q = buffered_queue_init(128, -1, false);
  tuple last_insert = {0, 0};
  for (tuple *i = buf; i->a; i++) {
    if (i->a) {
      // Log("(%d, %d)", i->a, i->b);
      if (!(last_insert.a == i->a && last_insert.b == i->b)) {
        buffered_queue_push(q, itot(i->a, i->b));
        last_insert.a = i->a;
        last_insert.b = i->b;
      }
    }
  }
  buffered_queue_show(q);
  buffered_queue_free(q);
  buffer_free();

  buffer_init();
  Log("TEST: join connect");
  Log("select S.C, S.D, R.A, R.B from S inner join R on S.C = R.A");
  data_mem *data2 = data_clone(data);
  tuple_sort_all(data2->s, DATA_SZ_MAX);
  tuple_sort_all(data2->r, DATA_SZ_MAX);
  tuple result[DATA_SZ_MAX * 3];
  // q = buffered_queue_init(256, -1, false);
  uint join_count = 0;
  for (int i = 0; i < DATA_SZ_MAX; i++) {
    for (int j = 0; j < DATA_SZ_MAX; j++) {
      if (!data2->s[i].a || !data2->r[j].a) continue;
      uint c = data2->s[i].a;
      uint d = data2->s[i].b;
      uint a = data2->r[j].a;
      uint b = data2->r[j].b;
      if (c == a) {
        // Log("push (%d, %d) (%d, %d) i=%d, j=%d", c, d, a, b, i, j);
        // buffered_queue_push(q, itot(c, d));
        // buffered_queue_push(q, itot(a, b));
        join_count++;
      }
    }
  }
  // buffered_queue_show(q);
  // buffered_queue_free(q);
  Log("join count = %d", join_count);
  buffer_free();
  return 0;
}