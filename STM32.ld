/*
 * Linker script for ARM Cortex-M3 (QEMU MPS2-AN385 or Stellaris)
 */

/* Entry point */
ENTRY(Reset_Handler)

/* Memory layout for MPS2-AN385 / Stellaris */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Stack size */
_stack_size = 0x400;

/* Top of stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
    /* Vector table goes first in flash */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* Code and constants */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH

    /* ARM exception unwinding */
    .ARM.extab :
    {
        *(.ARM.extab*)
    } > FLASH

    .ARM.exidx :
    {
        *(.ARM.exidx*)
    } > FLASH

    /* Data section (initialized variables) */
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT> FLASH

    /* BSS section (uninitialized variables) */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    /* End of BSS - needed for sbrk */
    . = ALIGN(4);
    end = .;
    _end = .;

    /* Heap (optional) */
    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;
        . = . + 0x200;
        _heap_end = .;
    } > RAM
}
