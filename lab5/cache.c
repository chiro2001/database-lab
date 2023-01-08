//
// Created by chiro on 23-1-7.
//

#include "cache.h"

/**
 * 初始化只读缓存
 * @param capacity
 * @return
 */
cache *cache_init(uint capacity) {
  cache *p = malloc(sizeof(cache));
  memset(p, 0, sizeof(cache));
  p->capacity = capacity;
  p->data = malloc(sizeof(cache_pair) * capacity);
  memset(p->data, 0, sizeof(cache_pair) * capacity);
  return p;
}

/**
 * 缓存读计数器
 */
uint cache_read_cnt = 0;

/**
 * 在缓存中根据地址找到一块缓冲区
 * @param self
 * @param addr
 * @return 找不到则返回 NULL
 */
cache_pair *cache_find(cache *self, uint addr) {
  for (uint i = 0; i < self->size; i++) {
    if (self->data[i].addr == addr) {
      return &self->data[i];
    }
  }
  return NULL;
}

/**
 * 向缓存中插入对应地址的缓冲区块数据
 * @param self
 * @param addr
 */
void cache_insert(cache *self, uint addr) {
  Dbg("cache_insert(%d)", addr);
  Assert(self->size < self->capacity, "insert failed, full");
  // self->cache = realloc(self->cache, ++self->size);
  cache_pair *cache = &self->data[self->size];
  cache->addr = addr;
  cache->blk = read_block(addr);
  cache->visit = cache_read_cnt;
  self->size++;
}

/**
 * 按照 [[index]] 删除缓存内的数据
 * @param self
 * @param index
 */
void cache_remove_index(cache *self, int index) {
  if (index < 0 || index >= self->size) return;
  cache_pair *target = &self->data[index];
  if (target == NULL) return;
  free_block(target->blk);
  // move last item to target place
  Dbg("cache_remove_index(%d), size=%d, move %d to %d in cache",
      index, self->size, self->data[self->size - 1].addr, target->addr);
  memcpy(self->data + self->size - 1, target, sizeof(cache_pair));
  self->size--;
}

/**
 * 按照地址删除缓存内的数据
 * @param self
 * @param addr
 */
void cache_remove(cache *self, uint addr) {
  cache_pair *target = cache_find(self, addr);
  if (target == NULL) return;
  cache_remove_index(self, (int) (target - self->data));
}

void cache_kick(cache *self) {
  if (self->size == 0) return;
  uint size_original = self->size;
  uint oldest = -1;
  int index = -1;
  for (uint i = 0; i < self->size; i++) {
    if (self->data[i].visit < oldest) {
      index = (int) i;
      oldest = self->data[i].visit;
    }
  }
  cache_remove_index(self, index);
  Assert(self->size + 1 == size_original, "cache kick failed!");
}

/**
 * 过缓存地读一个地址。<br/>
 * 由于使用了全局缓冲区所以不能和其他不过缓冲区的 extmem api 一起使用。
 * @param self
 * @param addr
 * @return
 */
char *cache_read(cache *self, uint addr) {
  cache_read_cnt++;
  cache_pair *target = cache_find(self, addr);
  // hit
  if (target != NULL) {
    target->visit = cache_read_cnt;
    return target->blk;
  }
  // miss, and full
  if (self->size == self->capacity) {
    cache_kick(self);
  }
  cache_insert(self, addr);
  target = cache_find(self, addr);
  Assert(target != NULL, "cache insert failed!");
  return target->blk;
}

/**
 * 释放缓存空间
 * @param self
 */
void cache_free(cache *self) {
  if (!self) return;
  for (uint i = 0; i < self->size; i++) {
    free_block(self->data[i].blk);
  }
  free(self->data);
  free(self);
}