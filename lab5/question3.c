//
// Created by chiro on 23-1-6.
//
#include "main_utils.h"
#include "questions.h"
#include "buffered_queue.h"
#include "iterator.h"

void index_binary_search(uint left, uint right, uint key, uint target) {

}

void q3() {
  Log("============================");
  Log("Q3: 基于索引的关系选择算法");
  Log("排序结果为关系R或S分别建立索引文件，");
  Log("模拟实现 select S.C, S.D from S where S.C = 128");
  Log("============================");
  buffer_init();
  Log("正在建立索引……");
  TPMMS(1, 17, 301, true);
  TPMMS(17, 49, 317, true);
  buffer_free();

  buffer_init();
  Log("索引文件位于 [301, 316], [317, 349]");
  index_binary_search(317, 349, 128, 500);
  buffer_report();
  buffer_free();
}