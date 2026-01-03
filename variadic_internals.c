/*
 * VARIADIC FUNCTIONS: UNDER THE HOOD
 *
 * Deep dive into how variadic functions work at the assembly/ABI level
 * Target: ARM Cortex-M3 (ARMv7-M architecture)
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

/*===========================================================================*/
/* PART 1: ARM Calling Convention (AAPCS)                                   */
/*===========================================================================*/

/*
 * ARM Architecture Procedure Call Standard (AAPCS):
 *
 * REGISTER USAGE:
 * - r0-r3:   First 4 arguments (32-bit each)
 * - r4-r11:  Callee-saved registers
 * - r12 (IP): Intra-procedure call scratch
 * - r13 (SP): Stack pointer
 * - r14 (LR): Link register (return address)
 * - r15 (PC): Program counter
 *
 * ARGUMENT PASSING:
 * 1. First 4 arguments go in r0, r1, r2, r3
 * 2. Additional arguments go on the STACK
 * 3. Stack grows DOWNWARD (high address → low address)
 * 4. Arguments on stack are pushed RIGHT-TO-LEFT
 *
 * EXAMPLE:
 *   printf("Value: %d %d %d %d %d\n", a, b, c, d, e);
 *
 *   r0 = pointer to "Value: %d %d %d %d %d\n"
 *   r1 = a
 *   r2 = b
 *   r3 = c
 *   [SP+0] = d    (on stack)
 *   [SP+4] = e    (on stack)
 */

/*===========================================================================*/
/* PART 2: What is va_list?                                                 */
/*===========================================================================*/

/*
 * On ARM Cortex-M3, va_list is typically defined as:
 *
 *   typedef struct {
 *       void *__ap;   // Pointer to current argument
 *   } va_list[1];
 *
 * Or simply:
 *   typedef char * va_list;
 *
 * It's essentially a POINTER that walks through memory where arguments live.
 */

void demonstrate_va_list_internals(void)
{
    printf("\n=== va_list Size and Alignment ===\n");
    printf("sizeof(va_list) = %zu bytes\n", sizeof(va_list));
    printf("This is just a pointer to walk through argument memory\n\n");
}

/*===========================================================================*/
/* PART 3: How va_start Works                                               */
/*===========================================================================*/

/*
 * va_start(ap, last_param) does:
 *
 * 1. Takes the ADDRESS of the last fixed parameter
 * 2. Advances past it to find where variadic args begin
 * 3. Sets ap to point there
 *
 * On ARM, this is tricky because first 4 args are in REGISTERS!
 *
 * PSEUDOCODE:
 *   #define va_start(ap, last) \
 *       ap = (char *)&last + ((sizeof(last) + 3) & ~3)
 *
 * The ((sizeof(last) + 3) & ~3) rounds up to 4-byte alignment
 *
 * But wait! What about register arguments?
 * - The compiler may create a "register save area" on the stack
 * - In variadic functions, ALL args might be spilled to stack
 */

int show_va_start_behavior(int fixed, ...)
{
    va_list ap;

    printf("\n=== va_start Behavior ===\n");
    printf("Address of 'fixed' parameter: %p\n", (void*)&fixed);

    va_start(ap, fixed);

    /* On ARM, va_list is typically a struct containing a pointer */
    /* Let's see where it points */
    void **ap_ptr = (void **)ap;
    printf("va_list points to: %p\n", *ap_ptr);
    printf("Difference: %td bytes\n", (char*)*ap_ptr - (char*)&fixed);

    /* Extract some values */
    int arg1 = va_arg(ap, int);
    int arg2 = va_arg(ap, int);

    printf("First variadic arg: %d\n", arg1);
    printf("Second variadic arg: %d\n", arg2);

    va_end(ap);

    return arg1 + arg2;
}

/*===========================================================================*/
/* PART 4: How va_arg Works                                                 */
/*===========================================================================*/

/*
 * va_arg(ap, type) does:
 *
 * 1. Read the value at the current pointer location
 * 2. Advance the pointer by sizeof(type) [aligned]
 *
 * PSEUDOCODE:
 *   #define va_arg(ap, type) \
 *       (ap += ((sizeof(type) + 3) & ~3), \
 *        *(type *)(ap - ((sizeof(type) + 3) & ~3)))
 *
 * TYPE PROMOTION RULES:
 * - char/short → int      (at least 32-bit on ARM)
 * - float → double        (variadic functions always use double)
 *
 * This is why:
 *   va_arg(ap, char)   // WRONG! Use int instead
 *   va_arg(ap, int)    // CORRECT
 */

void demonstrate_va_arg_mechanics(int count, ...)
{
    printf("\n=== va_arg Mechanics ===\n");

    va_list ap;
    va_start(ap, count);

    /* Get the internal pointer */
    char **ap_internal = (char **)ap;
    char *current_ptr = *ap_internal;

    printf("Initial va_list pointer: %p\n", (void*)current_ptr);

    for (int i = 0; i < count; i++)
    {
        printf("\nArgument %d:\n", i + 1);
        printf("  Reading from: %p\n", (void*)current_ptr);

        int value = va_arg(ap, int);

        /* Update our tracking pointer */
        char **ap_new = (char **)ap;
        current_ptr = *ap_new;

        printf("  Value: %d (0x%08X)\n", value, value);
        printf("  Next pointer: %p\n", (void*)current_ptr);
        printf("  Advanced by: %td bytes\n",
               current_ptr - (*(char **)ap_internal - sizeof(int)));
    }

    va_end(ap);
}

/*===========================================================================*/
/* PART 5: Memory Layout Visualization                                      */
/*===========================================================================*/

void visualize_stack_layout(const char *fmt, ...)
{
    printf("\n=== Stack Layout Visualization ===\n");

    /* Get addresses of our local variables */
    printf("Stack layout (approximate):\n\n");
    printf("  [Higher addresses]\n");
    printf("       |\n");

    /* The format string pointer */
    printf("  fmt parameter:     %p  (in r0 or stack)\n", (void*)&fmt);

    va_list ap;
    va_start(ap, fmt);

    /* Show where variadic args start */
    void **ap_ptr = (void **)ap;
    printf("  va_list points to: %p  <-- Variadic args start here\n", *ap_ptr);

    /* Extract and show some arguments */
    int arg1 = va_arg(ap, int);
    int arg2 = va_arg(ap, int);
    int arg3 = va_arg(ap, int);

    printf("       |\n");
    printf("  Arg 1: %d\n", arg1);
    printf("  Arg 2: %d\n", arg2);
    printf("  Arg 3: %d\n", arg3);
    printf("       |\n");
    printf("  [Lower addresses]\n");

    va_end(ap);
}

/*===========================================================================*/
/* PART 6: Assembly-Level View                                              */
/*===========================================================================*/

/*
 * Let's create a simple function and see what assembly it generates
 */

/* Prevent inlining so we can see the actual call */
__attribute__((noinline))
int simple_variadic_sum(int count, ...)
{
    int sum = 0;
    va_list ap;
    va_start(ap, count);

    for (int i = 0; i < count; i++)
    {
        sum += va_arg(ap, int);
    }

    va_end(ap);
    return sum;
}

/*
 * EXPECTED ASSEMBLY (ARM Cortex-M3, simplified):
 *
 * simple_variadic_sum:
 *     push    {r4, lr}         ; Save registers
 *     sub     sp, sp, #16      ; Allocate stack space
 *
 *     ; Store r1-r3 to stack (register save area for variadic args)
 *     str     r1, [sp, #4]     ; Save r1 (potential 1st variadic arg)
 *     str     r2, [sp, #8]     ; Save r2 (potential 2nd variadic arg)
 *     str     r3, [sp, #12]    ; Save r3 (potential 3rd variadic arg)
 *
 *     ; va_start: Point to first variadic arg
 *     add     r3, sp, #4       ; r3 = address of first variadic arg
 *     str     r3, [sp]         ; Store in va_list
 *
 *     ; Loop through arguments
 *     mov     r4, #0           ; sum = 0
 *     mov     r2, #0           ; i = 0
 * .loop:
 *     cmp     r2, r0           ; i < count?
 *     bge     .done
 *
 *     ; va_arg: Load next argument
 *     ldr     r3, [sp]         ; Load va_list pointer
 *     ldr     r1, [r3]         ; Load argument value
 *     add     r3, r3, #4       ; Advance pointer by 4 bytes
 *     str     r3, [sp]         ; Store updated pointer
 *
 *     add     r4, r4, r1       ; sum += arg
 *     add     r2, r2, #1       ; i++
 *     b       .loop
 * .done:
 *     mov     r0, r4           ; Return sum
 *     add     sp, sp, #16      ; Restore stack
 *     pop     {r4, pc}         ; Return
 *
 * KEY INSIGHTS:
 * 1. r1-r3 are SPILLED to stack even though they're in registers
 * 2. va_list is just a pointer to this stack area
 * 3. va_arg is just: load from pointer, advance pointer
 * 4. No runtime type checking - compiler trusts you!
 */

/*===========================================================================*/
/* PART 7: Why Type Safety Doesn't Exist                                    */
/*===========================================================================*/

void demonstrate_type_mismatch_danger(void)
{
    printf("\n=== Type Safety Problems ===\n");

    /* This is DANGEROUS but compiles fine */
    int count = 2;
    va_list ap;

    /* Simulate passing different types */
    printf("What happens with type mismatches:\n\n");

    /* Example: passing pointer as int */
    char *str = "Hello";
    int num = 42;

    /*
     * If we call: some_function(str, num)
     * But read as: va_arg(ap, int), va_arg(ap, char*)
     *
     * The BYTES will be interpreted differently!
     * - On 32-bit ARM, both are 4 bytes
     * - But the VALUES are completely different
     * - Pointer value 0x0000ABCD read as int = 43981
     * - Integer 42 read as pointer = crash!
     */

    printf("Pointer '%s' at address: %p\n", str, (void*)str);
    printf("If misread as int: 0x%08X (%d)\n",
           (unsigned int)(uintptr_t)str, (int)(uintptr_t)str);

    printf("\nInteger %d (0x%08X)\n", num, num);
    printf("If misread as pointer: %p (likely crashes!)\n", (void*)(uintptr_t)num);

    printf("\nThis is why printf format strings MUST match arguments!\n");
}

/*===========================================================================*/
/* PART 8: The Complete Picture                                             */
/*===========================================================================*/

void complete_example_with_tracking(void)
{
    printf("\n=== Complete Call Walkthrough ===\n");

    /* We'll call with 5 arguments to see register + stack */
    printf("\nCalling: simple_variadic_sum(5, 10, 20, 30, 40, 50)\n");
    printf("\nWhat happens:\n");
    printf("1. Arguments 1-4 try to go in r0-r3\n");
    printf("2. But it's variadic, so compiler spills r1-r3 to stack\n");
    printf("3. Arguments 5+ go directly on stack\n");
    printf("4. va_start points to the stack save area\n");
    printf("5. va_arg walks through memory, 4 bytes at a time\n");
    printf("6. No type checking - just byte copying!\n\n");

    int result = simple_variadic_sum(5, 10, 20, 30, 40, 50);
    printf("Result: %d (expected 150)\n", result);
}

/*===========================================================================*/
/* DEMO MAIN                                                                 */
/*===========================================================================*/

int demo_internals(void)
{
    printf("===========================================\n");
    printf("VARIADIC FUNCTIONS: UNDER THE HOOD\n");
    printf("Architecture: ARM Cortex-M3 (ARMv7-M)\n");
    printf("===========================================\n");

    demonstrate_va_list_internals();
    show_va_start_behavior(999, 111, 222);
    demonstrate_va_arg_mechanics(3, 100, 200, 300);
    visualize_stack_layout("test", 10, 20, 30);
    demonstrate_type_mismatch_danger();
    complete_example_with_tracking();

    printf("\n=== KEY TAKEAWAYS ===\n");
    printf("1. va_list is just a POINTER to memory\n");
    printf("2. Arguments live on the STACK (or register save area)\n");
    printf("3. va_arg = 'read 4 bytes, move pointer forward'\n");
    printf("4. NO type checking - you must get types right!\n");
    printf("5. Format strings (printf) provide the 'contract'\n");
    printf("6. Compiler generates code to spill registers to stack\n");
    printf("7. All based on ARM calling convention (AAPCS)\n\n");

    return 0;
}

/*===========================================================================*/
/* ADDITIONAL NOTES                                                          */
/*===========================================================================*/

/*
 * PLATFORM DIFFERENCES:
 *
 * ARM (32-bit):
 * - First 4 args in r0-r3, rest on stack
 * - va_list is pointer to stack
 * - 4-byte alignment
 *
 * x86-64 (64-bit):
 * - First 6 INTEGER args in rdi, rsi, rdx, rcx, r8, r9
 * - First 8 FLOAT args in xmm0-xmm7
 * - va_list is complex struct tracking register save areas
 * - More complicated due to register classes
 *
 * ARM64 (AArch64):
 * - First 8 args in x0-x7 (or d0-d7 for floats)
 * - va_list points to register save area on stack
 * - 8-byte alignment
 *
 * WHY IT WORKS:
 * - Calling conventions are STANDARDIZED (ABI)
 * - Caller and callee agree on memory layout
 * - va_list exploits this known layout
 * - It's just careful pointer arithmetic!
 *
 * WHY IT'S UNSAFE:
 * - No runtime type information
 * - Compiler can't verify format vs args
 * - Undefined behavior if types mismatch
 * - Buffer overruns possible
 *
 * MODERN ALTERNATIVES:
 * - C11: _Generic for type-generic programming
 * - C++: Templates, function overloading
 * - Rust: Macros with type checking
 * - But variadic functions still used for C compatibility
 */
