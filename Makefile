# Minimal FreeRTOS Makefile for ARM Cortex-M3
# Flat directory structure - all files in root

# Target processor
ARCH = cortex-m3

# Toolchain
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Project name
PROJECT = freertos-minimal

# Build directory
BUILD_DIR = build

# Source files (all in root directory)
SRC = \
	startup.c \
	tasks.c \
	queue.c \
	list.c \
	timers.c \
	port.c \
	heap_4.c \
	main.c

# Object files
OBJS = $(SRC:%.c=$(BUILD_DIR)/%.o)

# Include paths (current directory only)
INCLUDES = -I.

# Compiler flags
CFLAGS = -mcpu=$(ARCH) \
	-mthumb \
	-Wall \
	-Wextra \
	-O2 \
	-g \
	-ffunction-sections \
	-fdata-sections \
	$(INCLUDES)

# Linker flags
LDFLAGS = -mcpu=$(ARCH) \
	-mthumb \
	-Wl,--gc-sections \
	-T STM32.ld \
	-specs=nano.specs \
	-specs=rdimon.specs \
	-lrdimon

# Default target
all: $(BUILD_DIR)/$(PROJECT).elf size

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compile C files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link
$(BUILD_DIR)/$(PROJECT).elf: $(OBJS)
	@echo "LD $@"
	@$(CC) $(LDFLAGS) $(OBJS) -o $@

# Generate binary
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@

# Generate hex
$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O ihex $< $@

# Show size
size: $(BUILD_DIR)/$(PROJECT).elf
	@echo ""
	@$(SIZE) $<
	@echo ""

# Binary and hex targets
bin: $(BUILD_DIR)/$(PROJECT).bin
hex: $(BUILD_DIR)/$(PROJECT).hex

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Show configuration
info:
	@echo "Project: $(PROJECT)"
	@echo "Architecture: $(ARCH)"
	@echo "Source files:"
	@echo "$(SRC)" | tr ' ' '\n' | sed 's/^/  /'
	@echo ""
	@echo "Build directory: $(BUILD_DIR)"
	@echo ""

# List files
list:
	@echo "FreeRTOS source files in current directory:"
	@ls -lh *.c *.h | awk '{print $$9, $$5}'

# Run in QEMU
qemu: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Running in QEMU (Ctrl+A X to exit)..."
	@qemu-system-arm -machine mps2-an385 -cpu cortex-m3 \
		-kernel $(BUILD_DIR)/$(PROJECT).elf \
		-nographic \
		-semihosting-config enable=on,target=native \
		-serial mon:stdio

# Help
help:
	@echo "Minimal FreeRTOS Makefile - Flat Structure"
	@echo ""
	@echo "Targets:"
	@echo "  all     - Build the project (default)"
	@echo "  qemu    - Run in QEMU emulator"
	@echo "  bin     - Generate binary file"
	@echo "  hex     - Generate hex file"
	@echo "  clean   - Remove build artifacts"
	@echo "  info    - Show project configuration"
	@echo "  list    - List all source files"
	@echo "  help    - Show this help message"
	@echo ""
	@echo "Requirements:"
	@echo "  - arm-none-eabi-gcc toolchain"
	@echo "  - qemu-system-arm for emulation"
	@echo ""
	@echo "All source and header files are in the root directory"
	@echo ""

.PHONY: all clean size bin hex info list help qemu
