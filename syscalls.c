/*
 * Minimal syscalls for semihosting support
 */

#include <sys/stat.h>
#include <errno.h>

/* Semihosting SWI for ARM */
static inline int __attribute__((always_inline))
call_host(int reason, void *arg)
{
    int value;
    __asm__ volatile (
        "mov r0, %1\n"
        "mov r1, %2\n"
        "bkpt 0xAB\n"
        "mov %0, r0"
        : "=r" (value)
        : "r" (reason), "r" (arg)
        : "r0", "r1", "r2", "r3", "ip", "lr", "memory", "cc"
    );
    return value;
}

/* Semihosting operations */
#define SYS_WRITE   0x05
#define SYS_READ    0x06

int _write(int file, char *ptr, int len)
{
    (void)file;

    /* Semihosting write */
    unsigned int args[3];
    args[0] = 1; /* stdout */
    args[1] = (unsigned int)ptr;
    args[2] = (unsigned int)len;

    return len - call_host(SYS_WRITE, args);
}

int _read(int file, char *ptr, int len)
{
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

int _close(int file)
{
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file)
{
    (void)file;
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

extern char end;

void *_sbrk(int incr)
{
    static char *heap_end = 0;
    char *prev_heap_end;

    if (heap_end == 0)
    {
        heap_end = &end;
    }

    prev_heap_end = heap_end;
    heap_end += incr;

    return (void *)prev_heap_end;
}
