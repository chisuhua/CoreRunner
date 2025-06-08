#include "QuarkTS.h"
#include "unity.h"
#include <stdio.h>


static uint32_t instreth;
static uint32_t cycleh;

//typedef uint64_t count_t;
typedef uint32_t count_t;

static inline count_t read_cycle_start(void) {
    uint32_t lo, hi;
    asm volatile ("csrr %0, cycle" : "=r"(lo));  // 读取低32位
    asm volatile ("csrr %0, cycleh" : "=r"(hi));  // 读取高32位
    cycleh = hi;
    return lo;
}

static inline count_t read_cycle_end(void) {
    uint32_t lo, hi;
    asm volatile ("csrr %0, cycle" : "=r"(lo));  // 读取低32位
    asm volatile ("csrr %0, cycleh" : "=r"(hi));  // 读取高32位
    TEST_ASSERT_TRUE(cycleh == hi);
    return lo;
}

static inline count_t read_instret_start(void) {
    uint32_t lo, hi;
    asm volatile ("csrr %0, instret" : "=r"(lo));  // 读取低32位
    asm volatile ("csrr %0, instreth" : "=r"(hi));  // 读取高32位
    instreth = hi;
    return lo;
}

static inline count_t read_instret_end(void) {
    uint32_t lo, hi;
    asm volatile ("csrr %0, instret" : "=r"(lo));  // 读取低32位
    asm volatile ("csrr %0, instreth" : "=r"(hi));  // 读取高32位
    TEST_ASSERT_TRUE(instreth == hi);
    return lo;
}

extern void loop10inst(int loop_count);
extern void loop100inst(int loop_count);

void verify_loop_inst(int loop_count, int largebase) {
  printf("=========================");
  count_t start_instret;
  count_t end_instret;
  if (largebase == 100) {
    start_instret = read_instret_start();
    //start_instret = read_cycle_start();
    loop100inst(loop_count);
    end_instret = read_instret_end();
    //end_instret = read_cycle_end();
    printf("loop count %d, instcount %d\n",loop_count, loop_count*100);
  } else {
    start_instret = read_instret_start();
    //start_instret = read_cycle_start();
    loop10inst(loop_count);
    end_instret = read_instret_end();
    //end_instret = read_cycle_end();
    printf("loop count %d, instcount %d\n",loop_count, loop_count*10);
  }

  printf("start_instret %d, end_instret %d, delta instret %d\n", start_instret, end_instret, end_instret - start_instret);
}

void main() {
  verify_loop_inst(1, 10);
  verify_loop_inst(2, 10);
  verify_loop_inst(3, 10);
  verify_loop_inst(4, 10);
  verify_loop_inst(5, 10);
  verify_loop_inst(10, 10);
  verify_loop_inst(100, 10);
  verify_loop_inst(1000, 10);

  verify_loop_inst(1, 100);
  verify_loop_inst(10, 100);
  verify_loop_inst(100, 100);
  verify_loop_inst(1000, 100);
}
