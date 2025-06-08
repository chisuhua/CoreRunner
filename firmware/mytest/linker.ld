MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 0x400000 /* 可读写可执行的RAM区域 */
}

ENTRY(_start)

SECTIONS
{
    . = ORIGIN(RAM);
    .text : 
    {
        /**(.text._start*)*/
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.srodata)
        *(.srodata.*)
        _etext = .;
    } > RAM   /* RX权限，因RAM的rwx定义 */

    .data :
    {
        *(.data)
        *(.data.*)
        *(.sdata)
        *(.sdata.*)
        _edata = .;
    } > RAM AT > RAM  /* RW权限，AT确保加载时正确初始化 */

     .bss :
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(.sbss)
        *(.sbss.*)
        *(COMMON)
        __bss_end = .;
    } > RAM   /* RW权限 */       

    __global_pointer$ = .;

    .heap : {
        . = ALIGN(4096);
        . += 0x2000;
    } > RAM

    .stack :
    {
        . = ALIGN(4096);
        _stack_start = .;
        . += 0x2000; /* 32K stack size */
        _stack_end = .;
    } > RAM
    .end = .;
}

