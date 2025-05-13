/* 定义内存区域（根据硬件调整地址和大小） */
MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 0x100000  /* 可读写可执行的RAM区域 */
    /*RAM2 (rwx) : ORIGIN = 0x80100000, LENGTH = 0x100000  /* 可读写可执行的RAM区域 */
    /* ROM (rx) : ORIGIN = 0x0, LENGTH = 0x10000 */  /* 如果有只读存储区域 */
}

/* 入口点设置 */
ENTRY(start)

/* 主体段定义 */
SECTIONS
{
    . = ORIGIN(RAM);
    /* 代码段（只读+可执行） */
    .text : 
    {
        *(.text.start*)
        *(.text.*)
    } > RAM  /* RX权限，因RAM的rwx定义 */

    /* 数据段（可读写） */
    .data :
    {
        *(.data)
        *(.data.*)
    } > RAM AT > RAM  /* RW权限，AT确保加载时正确初始化 */

    /* 未初始化数据段（可读写） */
    .bss :
    {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    } > RAM  /* RW权限 */

    /* 任务列表段（关键部分） */
    .task_list :
    {
        __task_list_start = .;  /* 定义任务列表起始地址 */
        *(.task_list)           /* 包含所有任务结构 */
        __task_list_end = .;    /* 义任务列表结束地址 */
    } > RAM

    /* 其他段（如中断向量等） */
    . = ALIGN(4);
    _end = .;  /* 程序结束地址 */
}

/* 符号定义（可选，但建议显式声明） */
PROVIDE(__task_list_start = .task_list_start);
PROVIDE(__task_list_end = .task_list_end);
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
*/
