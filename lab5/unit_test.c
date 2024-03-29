//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "buffered_queue.h"
#include "cache.h"
#include "iterator.h"

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

uint8_t map[1000][1000];

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
  cache *ca = cache_init(2);
  for (int addr = 1; addr < 49; addr++) {
    Assert(cache_read(ca, addr) != NULL, "cache read err, addr: %d", addr);
  }
  for (int i = 0; i < 500; i++) {
    Assert(cache_read(ca, (rand() % 48) + 1) != NULL, "cache read err, addr: %d", i);
  }
  cache_free(ca);
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
        Log("push (%d, %d) (%d, %d) i=%d, j=%d", c, d, a, b, i, j);
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

  buffer_init_large();
  Log("TEST: iterator clone");
  // ca = cache_init(4);
  ca = NULL;
  iterator *it = iterator_init(1, 17, ca);
  iterator *it2 = iterator_init(17, 47, ca);
  iterator *it3 = iterator_init(17, 47, ca);
  iterator *it4 = iterator_init(1, 17, ca);
  q = buffered_queue_init(1, 401, true);
  for (int i = 0; i < 7 * 32; i++)
    buffered_queue_push(q, itot(i + 100, i + 100));
  buffered_queue_flush(q);
  buffered_queue_free(q);
  iterator *it5 = iterator_init(401, 417, ca);
  for (int i = 0; i < 54; i++) iterator_next(it);
  for (int i = 0; i < 32; i++) iterator_next(it2);
  for (int i = 0; i < 32; i++) iterator_next(it3);
  iterator *it_clone = iterator_clone(it);
  iterator *it2_clone = iterator_clone(it2);
  iterator *it4_clone = iterator_clone(it4);
  iterator *it5_clone = iterator_clone(it5);
  for (int i = 0; i < 8; i++) {
    char *c = iterator_now(it_clone); iterator_next(it_clone);
    Log(" clone: (%s, %s)", c, c + 4);
  }
  for (int i = 0; i < 8; i++) {
    char *c = iterator_now(it); iterator_next(it);
    Log("source: (%s, %s)", c, c + 4);
  }
  for (int i = 0; i < 8; i++) {
    char *c = iterator_now(it2_clone); iterator_next(it2_clone);
    char *c2 = iterator_now(it3); iterator_next(it3);
    // Log(" clone: (%s, %s)", c, c + 4);
    Log(" clone: (%s, %s) - (%s, %s)", c, c + 4, c2, c2 + 4);
  }
  for (int i = 0; i < 8; i++) {
    char *c = iterator_now(it2); iterator_next(it2);
    // char *c2 = iterator_now(it3); iterator_next(it3);
    // Log("source: (%s, %s) - (%s, %s)", c, c + 4, c2, c2 + 4);
    Log("source: (%s, %s)", c, c + 4);
  }

  for (int i = 0; i < 5; i++) {
    char *c = iterator_now(it4); iterator_next(it4);
    char *c2 = iterator_now(it4_clone); iterator_next(it4_clone);
    Log("s - c: (%s, %s) - (%s, %s)", c, c + 4, c2, c2 + 4);
  }
  for (int i = 0; i < 5; i++) {
    char *c = iterator_now(it4); iterator_next(it4);
    Log("  s  : (%s, %s)", c, c + 4);
  }
  for (int i = 0; i < 5; i++) {
    char *c = iterator_now(it4_clone); iterator_next(it4_clone);
    Log("  c  : (%s, %s)", c, c + 4);
  }

  for (int i = 0; i < 5; i++) {
    char *c = iterator_now(it5); iterator_next(it5);
    char *c2 = iterator_now(it5_clone); iterator_next(it5_clone);
    Log("s - c: (%s, %s) - (%s, %s)", c, c + 4, c2, c2 + 4);
    // Log("s - c: (%s, %s)", c, c + 4);
  }
  // for (int i = 0; i < 6; i++) {
  //   char *c = iterator_now(it5_clone); iterator_next(it5_clone);
  //   Log("  c  : (%s, %s)", c, c + 4);
  // }
  for (int i = 0; i < 5; i++) {
    char *c = iterator_now(it5); iterator_next(it5);
    Log("  s  : (%s, %s)", c, c + 4);
  }
  for (int i = 0; i < 5; i++) {
    char *c = iterator_now(it5_clone); iterator_next(it5_clone);
    Log("  c  : (%s, %s)", c, c + 4);
  }
  iterator_free_clone(it_clone);
  iterator_free(it);
  iterator_free_clone(it2_clone);
  iterator_free(it2);
  cache_free(ca);
  buffer_free();

  buffer_init_large();
  Log("TEST: join connect - 2");
  Log("select S.C, S.D, R.A, R.B from S inner join R on S.C = R.A");
  data_mem *data3 = data_clone(data);
  tuple_sort_all(data3->s, DATA_SZ_MAX);
  tuple_sort_all(data3->r, DATA_SZ_MAX);
  // q = buffered_queue_init(256, -1, false);
  q = buffered_queue_init(1, 700, true);
  join_count = 0;
  tuple *it_r = data3->r;
  tuple *it_s = data3->s;
  while (it_r->a && it_s->a) {
    while (it_s->a > it_r->a) {
      it_r++;
    }
    if (it_s->a == it_r->a) {
      tuple *it_r_clone = it_r;
      while (it_s->a == it_r_clone->a) {
        // Log("push (%d, %d) (%d, %d)",
        //     it_s->a, it_s->b,
        //     it_r_clone->a, it_r_clone->b);
        buffered_queue_push(q, itot(it_s->a, it_s->b));
        buffered_queue_push(q, itot(it_r_clone->a, it_r_clone->b));
        join_count++;
        it_r_clone++;
      }
    }
    it_s++;
  }
  buffered_queue_flush(q);
  buffered_queue_free(q);
  iterate_range_show(700, -1);
  Log("join count = %d", join_count);
  buffer_free();

  buffer_init_large();
  Log("TEST: cache and iterator");
  ca = cache_init(4);
  it = iterator_init(1, 17, ca);
  it2 = iterator_init(1, 17, NULL);
  iterator_next_n(it, 23);
  iterator_next_n(it2, 23);
  it3 = iterator_clone(it);
  it4 = iterator_clone(it2);
  while (!iterator_is_end(it3)) {
    char *a = iterator_now(it), *b = iterator_now(it2);
    char *c = iterator_now(it3), *d = iterator_now(it4);
    Log("cached: (%s, %s), uncached: (%s, %s)",
        a, a + 4, b, b + 4);
    Log("clone1: (%s, %s),   clone2: (%s, %s)",
        c, c + 4, d, d + 4);
    iterator_next(it);
    iterator_next(it2);
    iterator_next(it3);
    iterator_next(it4);
  }
  buffer_free();

  buffer_init_large();
  Log("TEST: Q5 [ S union R ]");
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
  fflush(stdout);
  memcpy(buf, data->s, sizeof(tuple) * len_s);
  memcpy(buf + len_s, data->r, sizeof(tuple) * len_r);
  uint len = len_s + len_r;
  tuple_sort_all(buf, len);
  q = buffered_queue_init(128, -1, false);
  tuple last_insert = {0, 0};
  uint count = 0;
  for (tuple *i = buf; i->a; i++) {
    if (i->a) {
      // Log("(%d, %d)", i->a, i->b);
      if (!(last_insert.a == i->a && last_insert.b == i->b)) {
        buffered_queue_push(q, itot(i->a, i->b));
        count++;
        last_insert.a = i->a;
        last_insert.b = i->b;
      }
    }
  }
  Log("union count: %d", count);
  buffered_queue_show(q);
  buffered_queue_free(q);
  buffer_free();

  buffer_init_large();
  Log("TEST: Q5 [ S intersects R ]");
  len_s = 0;
  len_r = 0;
  Log(" === S source === ");
  cnt = 0;
  memset(map, 0, sizeof(map));
  for (tuple *i = data->s; i->a; i++) {
    printf("(%d, %d) ", i->a, i->b);
    map[i->a][i->b] = 1;
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
    if (map[i->a][i->b] == 1) map[i->a][i->b] = 2;
    if ((++cnt) == 7) {
      puts("");
      cnt = 0;
    }
    len_r++;
  }
  if (cnt != 0) puts("");
  fflush(stdout);
  q = buffered_queue_init(128, -1, false);
  count = 0;
  for (uint i = 100; i < 1000; i++)
    for (uint j = 100; j < 1000; j++)
      if (map[i][j] == 2) {
        buffered_queue_push(q, itot(i, j));
        count++;
      }
  buffered_queue_show(q);
  Log("intersection count: %d", count);
  buffered_queue_free(q);
  buffer_free();

  buffer_init_large();
  Log("TEST: Q5 [ S - R ]");
  len_s = 0;
  len_r = 0;
  Log(" === S source === ");
  cnt = 0;
  memset(map, 0, sizeof(map));
  for (tuple *i = data->s; i->a; i++) {
    printf("(%d, %d) ", i->a, i->b);
    map[i->a][i->b] = 1;
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
    if (map[i->a][i->b] == 1) map[i->a][i->b] = 0;
    if ((++cnt) == 7) {
      puts("");
      cnt = 0;
    }
    len_r++;
  }
  if (cnt != 0) puts("");
  fflush(stdout);
  q = buffered_queue_init(128, -1, false);
  count = 0;
  for (uint i = 100; i < 1000; i++)
    for (uint j = 100; j < 1000; j++)
      if (map[i][j] == 1) {
        buffered_queue_push(q, itot(i, j));
        count++;
      }
  buffered_queue_show(q);
  Log("difference set count: %d", count);
  buffered_queue_free(q);
  buffer_free();

  return 0;
}