/*
 * Simple variadic function to compile to assembly
 * Compile with: arm-none-eabi-gcc -S -O1 -mcpu=cortex-m3 show_assembly.c
 */

#include <stdarg.h>

/* Simple sum function - we'll examine its assembly */
int sum(int count, ...)
{
    int total = 0;
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++)
    {
        total += va_arg(args, int);
    }

    va_end(args);
    return total;
}

/* Non-variadic version for comparison */
int sum_fixed(int a, int b, int c)
{
    return a + b + c;
}
