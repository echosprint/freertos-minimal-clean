# FreeRTOS Minimal - Codebase Learning Guide

A structured guide for efficiently learning this stripped-down FreeRTOS implementation.

## Overview

| Item | Details |
|------|---------|
| FreeRTOS Version | 202406.04 LTS (Kernel V11.1.0) |
| Target | ARM Cortex-M3 (QEMU mps2-an385) |
| Total Code | ~25,700 lines across 8 .c files and 13 .h files |
| Compiled Size | ~30.4 KB |
| Code Reduction | 24.2% stripped via unifdef |

## Quick Start

```bash
make          # Build the project
make qemu     # Run in QEMU emulator (Ctrl-A X to exit)
make clean    # Clean build artifacts
make info     # Show project configuration
```

## Architecture Layers

```
Application Layer     main.c
                        |
Kernel API            task.h / queue.h / timers.h / semphr.h
                        |
Kernel Implementation tasks.c / queue.c / timers.c / list.c / heap_4.c
                        |
Port (HW Abstraction) port.c / portmacro.h / portable.h
                        |
Startup / Platform    startup.c / syscalls.c / STM32.ld
```

## Recommended Reading Order

### Phase 1: Entry Points and Configuration (< 400 lines)

Start here to understand what this project does and how it's configured.

1. **`FreeRTOSConfig.h`** (124 lines)
   - All kernel configuration switches
   - Clock frequency, tick rate, heap size, max priorities
   - Feature enable/disable flags
   - Interrupt priority configuration for ARM Cortex-M3

2. **`main.c`** (172 lines)
   - Application entry point
   - Creates 2 periodic tasks (100 and 150 tick periods)
   - Creates a software timer (1000ms auto-reload)
   - Demonstrates `xTaskCreateStatic()` and `xTimerCreate()`
   - Calls `vTaskStartScheduler()` to hand control to the kernel

3. **`startup.c`** (82 lines)
   - ARM Cortex-M3 boot code
   - Vector table definition (exception handlers)
   - Reset handler: copies .data from FLASH to RAM, zeros .bss, calls `main()`

### Phase 2: Core Data Structure (750 lines)

The linked list is the foundation of the entire scheduler.

4. **`list.h`** (511 lines) + **`list.c`** (239 lines)
   - Doubly-linked list with sorted insertion
   - Used for: ready lists, delayed task lists, event lists, timer lists
   - Key types: `List_t`, `ListItem_t`, `MiniListItem_t`
   - Key functions: `vListInitialise()`, `vListInsert()`, `vListRemove()`
   - Short enough to read line-by-line

### Phase 3: Task Scheduler — The Kernel Core (6,576 lines)

The largest and most important file. Read it in functional blocks:

5. **`tasks.c`** — Read in this order:
   - **TCB definition** (~line 1-150) — Task Control Block structure, the core data type
   - **Module variables** (~line 150-300) — Ready lists, delayed lists, current TCB pointer
   - **`xTaskCreate()` / `xTaskCreateStatic()`** — How tasks are created and added to ready list
   - **`vTaskStartScheduler()`** — Creates idle task + timer task, then calls port layer
   - **`xTaskIncrementTick()`** — Called every 1ms by SysTick; checks delayed tasks, requests context switch
   - **`vTaskDelay()` / `vTaskDelayUntil()`** — How tasks sleep and wake up
   - **`vTaskSwitchContext()`** — Selects the highest-priority ready task
   - **`vTaskPlaceOnEventList()` / `xTaskRemoveFromEventList()`** — How tasks block/unblock on queues
   - **`vTaskSuspend()` / `vTaskResume()`** — Task suspend/resume
   - **`vTaskDelete()`** — Task cleanup
   - **Idle task** — Runs at lowest priority, cleans up deleted tasks

### Phase 4: Hardware Abstraction Layer (1,089 lines)

How FreeRTOS interfaces with ARM Cortex-M3 hardware.

6. **`portmacro.h`** (265 lines)
   - CPU-specific type definitions (`StackType_t`, `BaseType_t`, `TickType_t`)
   - Critical section macros (`portENTER_CRITICAL`, `portEXIT_CRITICAL`)
   - Interrupt enable/disable (`portDISABLE_INTERRUPTS`, `portENABLE_INTERRUPTS`)

7. **`port.c`** (824 lines)
   - `pxPortInitialiseStack()` — Sets up initial stack frame for a new task
   - `xPortStartScheduler()` — Configures SysTick, sets interrupt priorities, starts first task
   - `vPortEnterCritical()` / `vPortExitCritical()` — Nested critical sections
   - `xPortSysTickHandler()` — SysTick ISR, calls `xTaskIncrementTick()`
   - `PendSV_Handler` (inline assembly) — The actual context switch: saves/restores registers

8. **`STM32.ld`** — Linker script
   - FLASH: 256 KB at 0x00000000 (code + constants)
   - RAM: 64 KB at 0x20000000 (data + stack + heap)
   - Section layout: .isr_vector → .text → .data → .bss → .heap

### Phase 5: Communication and Synchronization (3,528 lines)

9. **`queue.c`** (2,372 lines)
   - Implements queues, binary semaphores, counting semaphores, and mutexes — all in one file
   - `xQueueCreate()` / `xQueueGenericReset()` — Queue creation and initialization
   - `xQueueSend()` / `xQueueReceive()` — Blocking send/receive with timeout
   - `xQueueSendFromISR()` / `xQueueReceiveFromISR()` — ISR-safe variants (never block)
   - Priority inheritance for mutexes
   - Queue registry for debugging

10. **`timers.c`** (1,156 lines)
    - Software timer implementation using a daemon task + command queue
    - `xTimerCreate()` — Creates timer control block
    - Timer daemon task — Waits on command queue, processes start/stop/reset/delete
    - Timer expiry processing — Executes callbacks, reloads auto-reload timers
    - Two timer lists (current + overflow) for tick wraparound handling

### Phase 6: Memory Management (623 lines)

11. **`heap_4.c`** (623 lines)
    - Block-based allocator with free block coalescing
    - `pvPortMalloc()` — First-fit allocation from a static byte array
    - `vPortFree()` — Returns block to free list, merges adjacent free blocks
    - Free list sorted by address for efficient coalescing
    - Tracks total free bytes and minimum-ever free bytes

## Boot Sequence

```
Power On / Reset
  |
  v
Reset_Handler (startup.c)
  |-- Copy .data section: FLASH -> RAM
  |-- Zero .bss section in RAM
  |-- Call main()
  |
  v
main() (main.c)
  |-- initialise_monitor_handles()    // Enable semihosting for printf
  |-- xTaskCreateStatic(task1, ...)   // Create Task 1
  |-- xTaskCreateStatic(task2, ...)   // Create Task 2
  |-- xTimerCreate(...)               // Create software timer
  |-- xTimerStart(...)                // Start the timer
  |-- vTaskStartScheduler()           // Start FreeRTOS
  |
  v
vTaskStartScheduler() (tasks.c)
  |-- Create idle task (priority 0)
  |-- Create timer daemon task
  |-- xPortStartScheduler()
  |
  v
xPortStartScheduler() (port.c)
  |-- Set PendSV and SysTick to lowest priority
  |-- Configure SysTick for 1ms interrupts
  |-- Load first task's stack pointer
  |-- Branch to first task
  |
  v
Scheduler Running (continuous)
  |-- SysTick fires every 1ms
  |     |-- xTaskIncrementTick(): update tick, check delayed tasks
  |     |-- Trigger PendSV if context switch needed
  |
  |-- PendSV_Handler: save/restore task context (ARM registers)
  |
  |-- Tasks execute based on priority
        |-- Task1: prints every 100 ticks, then vTaskDelay(100)
        |-- Task2: prints every 150 ticks, then vTaskDelay(150)
        |-- Timer daemon: executes timer callback every 1000ms
        |-- Idle task: runs when nothing else is ready
```

## Key Data Structures

### Task Control Block (TCB) — defined in tasks.c

```
TCB_t
  |-- pxTopOfStack        // Current stack pointer
  |-- xStateListItem      // Links task into ready/delayed/suspended lists
  |-- xEventListItem      // Links task into event wait lists
  |-- uxPriority          // Task priority (0 = lowest)
  |-- pxStack             // Start of allocated stack memory
  |-- pcTaskName          // Human-readable task name
  |-- uxBasePriority      // Original priority (before inheritance)
  |-- ulNotifiedValue     // Task notification value
```

### Queue / Semaphore / Mutex — defined in queue.c

```
Queue_t
  |-- pcHead / pcWriteTo   // Circular buffer pointers
  |-- xTasksWaitingToSend  // List of tasks blocked on send
  |-- xTasksWaitingToReceive // List of tasks blocked on receive
  |-- uxMessagesWaiting    // Current item count
  |-- uxLength             // Max items
  |-- uxItemSize           // Size per item (0 for semaphores)
  |-- ucQueueType          // Queue, mutex, semaphore, etc.
```

### Timer — defined in timers.c

```
Timer_t
  |-- pcTimerName          // Timer name
  |-- xTimerListItem       // Links into active timer list
  |-- xTimerPeriodInTicks  // Period in ticks
  |-- pvTimerID            // User-defined ID
  |-- pxCallbackFunction   // Callback function pointer
  |-- uxAutoReload         // One-shot or auto-reload
```

## Key Concepts to Understand

### 1. Context Switching
- ARM Cortex-M3 uses PendSV exception for deferred context switching
- SysTick ISR sets PendSV pending flag; actual switch happens in PendSV handler
- Context = R4-R11 (manually saved) + R0-R3, R12, LR, PC, xPSR (auto-saved by hardware)

### 2. Critical Sections
- `taskENTER_CRITICAL()` disables interrupts below `configMAX_SYSCALL_INTERRUPT_PRIORITY`
- Supports nesting via `uxCriticalNesting` counter
- Higher-priority interrupts (above threshold) still fire — for hard real-time needs

### 3. Priority Inheritance
- When a low-priority task holds a mutex needed by a high-priority task
- Low-priority task temporarily inherits the high priority
- Prevents unbounded priority inversion

### 4. Tick Overflow Handling
- 32-bit tick counter overflows every ~49.7 days at 1kHz
- Two delayed task lists: current and overflow
- Lists swap when tick counter wraps around

### 5. ISR-Safe API
- Functions ending in `FromISR` never block
- Use `portYIELD_FROM_ISR()` to request context switch from ISR
- Must not call blocking API from interrupt context

## Hands-On Exercises

### Exercise 1: Add a Third Task
Modify `main.c` to add a third task with a different priority and delay period. Observe how priority affects scheduling order.

### Exercise 2: Inter-Task Communication with Queues
Create a queue in `main.c`. Have Task1 send incrementing integers to the queue, and Task2 receive and print them.

### Exercise 3: Binary Semaphore Synchronization
Create a binary semaphore. Have Task1 give it periodically, and Task2 block waiting to take it. Observe the synchronization behavior.

### Exercise 4: Trace the Context Switch
Add `printf` calls inside `xTaskIncrementTick()` and `vTaskSwitchContext()` in `tasks.c` to trace exactly when and why context switches happen.

### Exercise 5: Explore Priority Inversion
Create 3 tasks (low, medium, high priority) and a mutex. Have the low-priority task hold the mutex while the high-priority task waits for it. Observe priority inheritance in action.

## File Quick Reference

| File | Lines | Purpose |
|------|-------|---------|
| `FreeRTOSConfig.h` | 124 | Kernel configuration |
| `main.c` | 172 | Example application |
| `startup.c` | 82 | Boot code and vector table |
| `list.h` + `list.c` | 750 | Core linked list |
| `tasks.c` | 6,576 | Task scheduler (kernel core) |
| `port.c` | 824 | ARM Cortex-M3 port |
| `portmacro.h` | 265 | Port-specific macros |
| `queue.c` | 2,372 | Queues, semaphores, mutexes |
| `timers.c` | 1,156 | Software timers |
| `heap_4.c` | 623 | Memory allocator |
| `syscalls.c` | 93 | Semihosting I/O |
| `STM32.ld` | — | Linker script (memory layout) |

## Further Resources

- [FreeRTOS Official Documentation](https://www.freertos.org/Documentation/RTOS_book.html)
- [ARM Cortex-M3 Technical Reference Manual](https://developer.arm.com/documentation/ddi0337/latest/)
- [Mastering the FreeRTOS Real Time Kernel (free book)](https://www.freertos.org/Documentation/161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide.pdf)
