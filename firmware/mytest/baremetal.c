#include "QuarkTS.h"
#include "unity.h"
#include <stdio.h>

#define UART_BASE 0x10000000

// 定义寄存器偏移量
#define UART_RBR (UART_BASE + 0x00) // 接收缓冲寄存器
#define UART_THR (UART_BASE + 0x00) // 发送保持寄存器
#define UART_IER (UART_BASE + 0x01) // 中断使能寄存器
#define UART_FCR (UART_BASE + 0x02) // FIFO 控制寄存器
#define UART_LCR (UART_BASE + 0x03) // 线路控制寄存器
#define UART_MCR (UART_BASE + 0x04) // MODEM 控制寄存器
#define UART_LSR (UART_BASE + 0x05) // 线路状态寄存器

// UART 寄存器偏移
#define UART_LSR_THRE (1 << 5)        // Transmitter Holding Register Empty
//
// 内联汇编函数用于读写内存映射 I/O
static inline uint8_t mmio_read(uintptr_t addr) {
    uint8_t value;
    __asm__ volatile ("lw %0, (%1)" : "=r"(value) : "r"(addr));
    return value;
}

static inline void mmio_write(uintptr_t addr, uint8_t value) {
    __asm__ volatile ("sw %0, (%1)" :: "r"(value), "r"(addr));
}

// 读写 8-bit 寄存器
static inline uint8_t mmio_read8(uintptr_t addr) {
    return *(volatile uint8_t*)addr;
}

static inline void mmio_write8(uintptr_t addr, uint8_t val) {
    *(volatile uint8_t*)addr = val;
}

// 读写 32-bit 寄存器
static inline uint32_t mmio_read32(uintptr_t addr) {
    return *(volatile uint32_t*)addr;
}

static inline void mmio_write32(uintptr_t addr, uint32_t val) {
    *(volatile uint32_t*)addr = val;
}
void uart_init() {
    // 禁用中断
    mmio_write(UART_IER, 0x00);
    
    // 设置波特率除数寄存器（DLAB=1）
    mmio_write(UART_LCR, 0x80); // DLAB bit = 1
    
    // 设置波特率，假设时钟频率为 18.432 MHz
    mmio_write(UART_RBR, 12); // Low byte of divisor (for 9600 baud)
    mmio_write(UART_IER, 0);  // High byte of divisor
    
    // 设置数据格式：8N1
    mmio_write(UART_LCR, 0x03); // Clear DLAB, set word length to 8 bits
}

void uart_putc(char c) {
    // 等待直到 THR 可以接收新的字符
    while ((mmio_read8(UART_LSR) & UART_LSR_THRE) == 0); // 检查 THRE (Transmitter Holding Register Empty) flag
    // 发送字符
    mmio_write8(UART_THR, c);
}
void uart_puts(const char* str) {
    while (*str) {
        uart_putc(*str++);
    }
}

#ifdef BAREMETAL
void putUART1(void *sp, const char c) {
    uart_putc(c);
}
#else
void putUART1(void *sp, const char c) {
    putchar(c);
}
#endif

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
  qTrace_Message("=========================");
  count_t start_instret;
  count_t end_instret;
  if (largebase == 100) {
    start_instret = read_instret_start();
    //start_instret = read_cycle_start();
    loop100inst(loop_count);
    end_instret = read_instret_end();
    //end_instret = read_cycle_end();
    qTrace_UnsignedDecimal(loop_count);
    qTrace_UnsignedDecimal(loop_count*100);
  } else {
    start_instret = read_instret_start();
    //start_instret = read_cycle_start();
    loop10inst(loop_count);
    end_instret = read_instret_end();
    //end_instret = read_cycle_end();
    qTrace_UnsignedDecimal(loop_count);
    qTrace_UnsignedDecimal(loop_count*10);
  }

  qTrace_UnsignedHexadecimal(start_instret);
  qTrace_UnsignedHexadecimal(end_instret);
  qTrace_UnsignedHexadecimal(end_instret - start_instret);
  qTrace_UnsignedDecimal(end_instret - start_instret);
}

void main() {
  uart_init();
  uart_puts("Hello Uart\n");

  qTrace_Set_OutputFcn(putUART1);
  verify_loop_inst(1, 10);
  verify_loop_inst(10, 10);
  verify_loop_inst(1000, 10);

  static qUINT32_t Counter = 0;
  while(1) {
    Counter++;
    qTrace_Message("hello world\n");
    qTrace_Variable(Counter, UnsignedDecimal);
    if (Counter > 2) exit(0);
  }
}
