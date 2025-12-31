# FreeRTOS Minimal - Clean & Stripped Version

Minimal, readable FreeRTOS for ARM Cortex-M3 with all unused `#ifdef` branches removed.

## Features

✅ **Flat structure** - All files in one directory
✅ **24% code reduction** - Stripped with `unifdef` tool
✅ **Minimal config** - Only essential features enabled
✅ **Simple example** - Task + software timer demo
✅ **Runs in QEMU** - Test without hardware
✅ **Ready to compile** - Complete build system

## Quick Start

```bash
# Build
make

# Run in QEMU
make qemu
```

**Output:**
```
===========================================
Minimal FreeRTOS Example with Timer
Target: ARM Cortex-M3
===========================================

Timer: Created and started (1000 ms period)

Starting FreeRTOS scheduler...

Task: Started - will print every 100 ticks
Task: Running...
Task: Running...
...
Timer: Callback executed (count = 1)
Task: Running...
...
```

Press `Ctrl+A` then `X` to exit QEMU.

## File List

### Source Files (8)
- `startup.c` - Boot code and vector table
- `tasks.c` - Task scheduler (259KB, stripped from 345KB)
- `queue.c` - Queue management (90KB, stripped from 126KB)
- `list.c` - List data structure (9.6KB)
- `timers.c` - Software timers (47KB, stripped from 56KB)
- `port.c` - ARM Cortex-M3 port (38KB)
- `heap_4.c` - Memory management (24KB)
- `main.c` - Example application with task + timer

### Build Files
- `Makefile` - Build system with QEMU support
- `STM32.ld` - Linker script for ARM Cortex-M3
- `FreeRTOSConfig.h` - Minimal configuration

### Header Files (13)
Essential FreeRTOS API headers - all unused headers removed.

## Code Reduction Summary

| File | Before | After | Reduction |
|------|--------|-------|-----------|
| tasks.c | 8,696 lines | 6,576 lines | 24.4% |
| queue.c | 3,364 lines | 2,372 lines | 29.5% |
| timers.c | 1,340 lines | 1,156 lines | 13.7% |
| **Total** | **13,646** | **10,343** | **24.2%** |

## Building

### Requirements
```bash
# Install ARM GCC toolchain
sudo apt-get install gcc-arm-none-eabi

# Install QEMU for testing (optional)
sudo apt-get install qemu-system-arm
```

### Build Commands

```bash
# Build the project
make

# Run in QEMU emulator
make qemu

# Show project configuration
make info

# List all source files
make list

# Clean build artifacts
make clean

# Show help
make help
```

### Build Output
```
   text    data     bss     dec     hex filename
  16632     172   13596   30400    76c0 build/freertos-minimal.elf
```

- **Text:** 16.6 KB (code)
- **Data:** 172 bytes (initialized data)
- **BSS:** 13.3 KB (uninitialized data)
- **Total:** ~30 KB

## Testing in QEMU

### Run the Example
```bash
make qemu
```

Or manually:
```bash
qemu-system-arm \
  -machine mps2-an385 \
  -cpu cortex-m3 \
  -kernel build/freertos-minimal.elf \
  -nographic \
  -semihosting-config enable=on,target=native
```

### What You'll See

1. ✅ **Initialization messages** - FreeRTOS starts up
2. ✅ **Task execution** - Periodic task runs every 100 ticks
3. ✅ **Timer callbacks** - Software timer fires every 1000ms
4. ✅ **Context switching** - Tasks yield and resume correctly
5. ✅ **Printf output** - Via ARM semihosting

### Exit QEMU
- Press `Ctrl+A` then `X`
- Or use `Ctrl+C` in terminal

## Configuration

Configured in `FreeRTOSConfig.h`:

**Enabled:**
- ✅ Preemptive scheduling
- ✅ Tasks, queues, mutexes, semaphores
- ✅ Software timers
- ✅ Task notifications
- ✅ Static + dynamic allocation

**Disabled:**
- ❌ Event groups, stream buffers
- ❌ Co-routines, queue sets
- ❌ Trace facility, statistics
- ❌ Tickless idle, hooks
- ❌ Stack overflow checking

**Hardware:**
- CPU: ARM Cortex-M3
- Clock: 72 MHz
- Tick rate: 1000 Hz (1ms)
- Heap: 10 KB
- Priorities: 5 levels

## Example Application

`main.c` demonstrates:

1. **Task Creation** - Simple periodic task using `vTaskDelay()`
2. **Software Timer** - Auto-reload timer with 1000ms period
3. **Printf Support** - Output via ARM semihosting
4. **Static Allocation** - Idle and timer task memory

### Code Flow
```
main()
  ├─> Create task (static allocation)
  ├─> Create software timer
  ├─> Start timer
  └─> Start FreeRTOS scheduler
       ├─> Task runs periodically
       └─> Timer callback fires every 1s
```

## How It Was Created

### 1. Extracted Minimal Files
Copied from FreeRTOS-Kernel v11.1.0:
- Core: `tasks.c`, `queue.c`, `list.c`, `timers.c`
- Port: ARM Cortex-M3 (`port.c`, `portmacro.h`)
- Memory: `heap_4.c`
- Headers: 13 essential headers only

### 2. Created Minimal Configuration
`FreeRTOSConfig.h` with only essential features enabled.

### 3. Stripped with unifdef
```bash
unifdef -DconfigUSE_PREEMPTION=1 \
        -UconfigUSE_EVENT_GROUPS \
        -UconfigUSE_TRACE_FACILITY \
        -UconfigUSE_STREAM_BUFFERS \
        ... (many more flags)
        tasks.c > tasks_clean.c
```

### 4. Flattened Directory Structure
All files in root - no subdirectories.

### 5. Added Build System
- `Makefile` with QEMU support
- `STM32.ld` linker script
- `startup.c` for boot code
- Semihosting for printf

### 6. Tested in QEMU
Verified running on ARM Cortex-M3 emulation.

## Porting to Hardware

To port to real ARM Cortex-M hardware:

### 1. Update Clock Configuration
In `FreeRTOSConfig.h`:
```c
#define configCPU_CLOCK_HZ  ( YourClockSpeed )
```

### 2. Add Hardware-Specific Code
- UART initialization for printf
- Clock/PLL setup
- Peripheral initialization

### 3. Update Linker Script
Adjust memory regions in `STM32.ld`:
```ld
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 256K
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 64K
}
```

### 4. Replace Semihosting
Implement `_write()` for your UART instead of semihosting.

### 5. Port to Other Cortex-M
Replace `port.c` and `portmacro.h`:
- Cortex-M0: `portable/GCC/ARM_CM0/`
- Cortex-M4F: `portable/GCC/ARM_CM4F/`
- Cortex-M7: `portable/GCC/ARM_CM7/`

## File Structure

```
FreeRTOS-Clean/
├── Source Files (8 .c files)
│   ├── startup.c          - Boot and vector table
│   ├── tasks.c            - Scheduler (stripped)
│   ├── queue.c            - Queues (stripped)
│   ├── list.c             - Lists (stripped)
│   ├── timers.c           - Software timers (stripped)
│   ├── port.c             - ARM Cortex-M3 port
│   ├── heap_4.c           - Memory manager
│   └── main.c             - Example application
├── Header Files (13 .h files)
│   ├── FreeRTOS.h         - Main API
│   ├── FreeRTOSConfig.h   - Configuration
│   ├── task.h, queue.h... - API headers
│   └── port headers...    - Port-specific
├── Build System
│   ├── Makefile           - Build + QEMU
│   ├── STM32.ld           - Linker script
│   └── build/             - Output directory
└── README.md              - This file
```

## License

Based on FreeRTOS Kernel V11.1.0

- **License:** MIT
- **Copyright:** (C) 2021 Amazon.com, Inc. or its affiliates

See original FreeRTOS repository:
- https://www.freertos.org
- https://github.com/FreeRTOS

## Resources

- [FreeRTOS Documentation](https://www.freertos.org/Documentation)
- [ARM Cortex-M3 Port](https://freertos.org/Documentation/02-Kernel/03-Supported-devices/04-Demos/ARM-Cortex/RTOS-Cortex-M3-M4)
- [unifdef Tool](https://dotat.at/prog/unifdef/)
- [QEMU ARM Emulation](https://www.qemu.org/docs/master/system/arm/mps2.html)

## Summary

This minimal FreeRTOS build provides:

- ✨ **Clean, readable code** - 24% less code to read
- 🎯 **Focused functionality** - Only what you need
- 🧪 **Tested in QEMU** - Runs without hardware
- 📚 **Educational value** - Easier to learn and understand
- 🚀 **Quick start** - Simple example to get started
- 🔧 **Easy to customize** - Well-organized flat structure

**Perfect for:**
- Learning FreeRTOS internals
- Embedded systems education
- Quick prototyping
- Understanding RTOS concepts
- Starting point for custom projects

**Verified working in QEMU with ARM Cortex-M3 emulation!**
