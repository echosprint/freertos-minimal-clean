/*
 * Minimal startup code for ARM Cortex-M3 on QEMU
 */

#include <stdint.h>

/* External symbols from linker script */
extern uint32_t _estack;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _sidata;

/* Main function */
extern int main(void);

/* Cortex-M3 core exception handlers */
void Reset_Handler(void);
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

/* Default handler for interrupts */
void Default_Handler(void)
{
    while(1);
}

/* Vector table */
__attribute__((section(".isr_vector")))
uint32_t vector_table[] = {
    (uint32_t)&_estack,           /* Initial Stack Pointer */
    (uint32_t)Reset_Handler,      /* Reset Handler */
    (uint32_t)NMI_Handler,        /* NMI Handler */
    (uint32_t)HardFault_Handler,  /* Hard Fault Handler */
    (uint32_t)MemManage_Handler,  /* MPU Fault Handler */
    (uint32_t)BusFault_Handler,   /* Bus Fault Handler */
    (uint32_t)UsageFault_Handler, /* Usage Fault Handler */
    0,                            /* Reserved */
    0,                            /* Reserved */
    0,                            /* Reserved */
    0,                            /* Reserved */
    (uint32_t)SVC_Handler,        /* SVCall Handler */
    (uint32_t)DebugMon_Handler,   /* Debug Monitor Handler */
    0,                            /* Reserved */
    (uint32_t)PendSV_Handler,     /* PendSV Handler */
    (uint32_t)SysTick_Handler,    /* SysTick Handler */
};

/* Reset handler - entry point */
void Reset_Handler(void)
{
    uint32_t *src, *dest;

    /* Copy data section from flash to RAM */
    src = &_sidata;
    dest = &_sdata;
    while (dest < &_edata)
    {
        *dest++ = *src++;
    }

    /* Zero fill bss section */
    dest = &_sbss;
    while (dest < &_ebss)
    {
        *dest++ = 0;
    }

    /* Call main */
    main();

    /* Infinite loop if main returns */
    while(1);
}
