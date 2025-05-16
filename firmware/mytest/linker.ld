MEMORY
{
    RAM (rwx) : ORIGIN = 0x00002000, LENGTH = 0x40000 /* 可读写可执行的RAM区域 */
    /*RAM2 (rwx) : ORIGIN = 0x80100000, LENGTH = 0x100000  /* 可读写可执行的RAM区域 */
}

ENTRY(_start)

SECTIONS
{
    . = ORIGIN(RAM);
    .text : 
    {
        /**(.text._start*)*/
        *(.text)
        *(.rodata)
        _etext = .;
    } > RAM  /* RX权限，因RAM的rwx定义 */

    .data :
    {
        *(.data)
        _edata = .;
    } > RAM AT > RAM  /* RW权限，AT确保加载时正确初始化 */

    .bss :
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        _end = .;
    } > RAM  /* RW权限 */

    __global_pointer$ = .;

    .stack :
    {
        _stack_start = .;
        . += 0x1000; /* 4K stack size */
        _stack_end = .;
    } > RAM
    .end = .;

    /* 其他段（如中断向量等） */
}

/* 符号定义（可选，但建议显式声明） */
/*
SECTIONS
{
    . = 0x10000000;
    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    . = ALIGN(4);
    .bss : { *(.bss) }
    .task_list :
    {
        __task_list_start = .;   // 定义起始地址 
        *(.task_list)            // 包含所有任务列表段 
        __task_list_end = .;     // 定义结束地址
    } > RAM
    // .task_list : { *(.task_list) }  任务列表段 
    .task_stack : { *(.task_stack) } // 堆栈段
}
PROVIDE(__task_list_start = .task_list_start);
PROVIDE(__task_list_end = .task_list_end);
*/
