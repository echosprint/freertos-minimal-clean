/*
 * Demonstration of Variadic Functions in C
 *
 * This file shows how printf() and similar functions can accept
 * a variable number of arguments using stdarg.h
 */

#include <stdio.h>
#include <stdarg.h>

/*-----------------------------------------------------------*/
/* Example 1: Simple variadic function that sums integers    */
/*-----------------------------------------------------------*/

/**
 * Sum a variable number of integers
 * @param count Number of integers to sum
 * @param ... Variable number of integer arguments
 * @return Sum of all integers
 */
int sum_integers(int count, ...)
{
    int sum = 0;

    /* Step 1: Declare a va_list variable to hold the argument list */
    va_list args;

    /* Step 2: Initialize the va_list with va_start()
     *         - First param: the va_list variable
     *         - Second param: the LAST FIXED parameter (before ...)
     */
    va_start(args, count);

    /* Step 3: Access each argument using va_arg()
     *         - First param: the va_list variable
     *         - Second param: the TYPE of the next argument
     */
    for (int i = 0; i < count; i++)
    {
        sum += va_arg(args, int);
    }

    /* Step 4: Clean up with va_end() */
    va_end(args);

    return sum;
}

/*-----------------------------------------------------------*/
/* Example 2: Simplified printf-like function                */
/*-----------------------------------------------------------*/

/**
 * A simplified printf-like function
 * Only handles %d (int) and %s (string) format specifiers
 */
void my_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    /* Scan through the format string */
    for (const char *p = format; *p != '\0'; p++)
    {
        if (*p == '%')
        {
            p++; /* Move to the character after % */

            switch (*p)
            {
                case 'd': /* Integer */
                {
                    int value = va_arg(args, int);
                    printf("%d", value);
                    break;
                }

                case 's': /* String */
                {
                    const char *str = va_arg(args, const char*);
                    printf("%s", str);
                    break;
                }

                case '%': /* Literal % */
                {
                    putchar('%');
                    break;
                }

                default:
                    putchar('%');
                    putchar(*p);
                    break;
            }
        }
        else
        {
            putchar(*p);
        }
    }

    va_end(args);
}

/*-----------------------------------------------------------*/
/* Example 3: Variable argument logging function             */
/*-----------------------------------------------------------*/

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} LogLevel;

/**
 * Logging function with variable arguments
 */
void log_message(LogLevel level, const char *format, ...)
{
    /* Print the log level prefix */
    switch (level)
    {
        case LOG_INFO:    printf("[INFO] ");    break;
        case LOG_WARNING: printf("[WARNING] "); break;
        case LOG_ERROR:   printf("[ERROR] ");   break;
    }

    /* Handle the variable arguments */
    va_list args;
    va_start(args, format);

    /* Use vprintf which accepts a va_list instead of ... */
    vprintf(format, args);

    va_end(args);

    printf("\n");
}

/*-----------------------------------------------------------*/
/* Demo main function                                         */
/*-----------------------------------------------------------*/

int demo_variadic_functions(void)
{
    printf("===========================================\n");
    printf("Variadic Functions Demonstration\n");
    printf("===========================================\n\n");

    /* Example 1: Sum integers */
    printf("Example 1: Sum of integers\n");
    printf("sum_integers(3, 10, 20, 30) = %d\n", sum_integers(3, 10, 20, 30));
    printf("sum_integers(5, 1, 2, 3, 4, 5) = %d\n\n", sum_integers(5, 1, 2, 3, 4, 5));

    /* Example 2: Custom printf */
    printf("Example 2: Custom printf-like function\n");
    my_printf("Hello %s, you have %d messages\n\n", "World", 42);

    /* Example 3: Logging with variable arguments */
    printf("Example 3: Logging function\n");
    log_message(LOG_INFO, "System initialized");
    log_message(LOG_WARNING, "Temperature is %d degrees", 85);
    log_message(LOG_ERROR, "Failed to connect to %s:%d", "192.168.1.1", 8080);

    printf("\n");

    /* Show how printf accepts different numbers of arguments */
    printf("Standard printf examples:\n");
    printf("No arguments (just format string)\n");
    printf("One argument: %d\n", 100);
    printf("Two arguments: %s = %d\n", "value", 200);
    printf("Three arguments: %s %d %s\n", "Task", 1, "completed");

    return 0;
}

/*-----------------------------------------------------------*/
/* Technical explanation in comments                         */
/*-----------------------------------------------------------*/

/*
 * HOW IT WORKS INTERNALLY:
 *
 * 1. CALLING CONVENTION:
 *    - Arguments are pushed onto the stack (or passed in registers)
 *    - The function knows where to start looking based on the last fixed parameter
 *
 * 2. va_list:
 *    - A pointer that walks through the stack/memory where arguments are stored
 *
 * 3. va_start(ap, last_fixed):
 *    - Initializes va_list to point just after the last fixed parameter
 *
 * 4. va_arg(ap, type):
 *    - Retrieves the next argument and advances the pointer
 *    - YOU must specify the correct type!
 *    - Type promotion applies: char/short become int, float becomes double
 *
 * 5. va_end(ap):
 *    - Cleanup (required for portability, may be a no-op on some systems)
 *
 * IMPORTANT LIMITATIONS:
 *
 * - The function must have a way to know:
 *   a) How many arguments were passed (like our count parameter), OR
 *   b) What types the arguments are (like printf's format string), OR
 *   c) A sentinel value to mark the end (e.g., NULL-terminated list)
 *
 * - Type safety is NOT enforced! If you use va_arg with the wrong type,
 *   you get undefined behavior
 *
 * - At least ONE fixed parameter is required before the ...
 *
 * PRINTF SPECIFICALLY:
 *
 * - printf() parses the format string to determine:
 *   1. How many arguments to expect
 *   2. What type each argument should be (%d = int, %s = char*, %f = double, etc.)
 *
 * - The format string acts as the "contract" between caller and callee
 *
 * - That's why printf("value = %d\n", x) works but printf("value = %d\n") crashes!
 *   The format string says "expect an int" but none was provided
 */
