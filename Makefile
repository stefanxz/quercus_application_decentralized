# Makefile for building WebAssembly and AOT binaries
# Tools
CC       = clang
WAMRC    ?= wamrc

# Build directory
BUILD_DIR = build

# Compilation flags for building WebAssembly
# -Wall: Enable all warnings
# -Wextra: Enable extra warnings
# -Wno-incompatible-library-redeclaration: Suppress warnings about redefined functions
# since we are not using the real stdlib anyway
CFLAGS   = -Wall -Wextra -Wno-incompatible-library-redeclaration
WASM_OPTS = -target wasm32 -nostdlib \
-Wl,--export=__heap_base,--export=__data_end,--export=__global_base \
   -Wl,--no-entry \
   -Wl,--allow-undefined \
   -Wl,--export-dynamic \
   -Os

WASM_PI_OPTS = -Wl,-z,stack-size=809600 \
   -Wl,--max-memory=65536000

WASM_PICO_OPTS = -Wl,-z,stack-size=8096 \
-Wl,--max-memory=65536

# Options for wamrc to compile wasm to AOT
WAMRC_OPTS = --xip
WAMRC_PICO_OPTS = --target=thumbv6m --target-abi=eabi --cpu=cortex-m0 \
  --enable-builtin-intrinsics=all
WAMRC_PI_OPTS = --target=armv4 --target-abi=eabi --cpu=cortex-a53 \
--enable-builtin-intrinsics=all

# Files - Source files remain in source/, output files go to BUILD_DIR
SRC_PI      = source/pi.c
WASM_PI     = $(BUILD_DIR)/$(notdir $(SRC_PI)).wasm
AOT_PI      = $(BUILD_DIR)/$(notdir $(SRC_PI)).aot

SRC_PICO_IN      = source/pico_check-in.c
WASM_PICO_IN     = $(BUILD_DIR)/$(notdir $(SRC_PICO_IN)).wasm
AOT_PICO_IN      = $(BUILD_DIR)/$(notdir $(SRC_PICO_IN)).aot

SRC_PICO_ROUTING      = source/pico_default.c
WASM_PICO_ROUTING     = $(BUILD_DIR)/$(notdir $(SRC_PICO_ROUTING)).wasm
AOT_PICO_ROUTING      = $(BUILD_DIR)/$(notdir $(SRC_PICO_ROUTING)).aot

SRC_PICO_SECURITY      = source/pico_security.c
WASM_PICO_SECURITY     = $(BUILD_DIR)/$(notdir $(SRC_PICO_SECURITY)).wasm
AOT_PICO_SECURITY      = $(BUILD_DIR)/$(notdir $(SRC_PICO_SECURITY)).aot

SRC_PICO_PLANE      = source/pico_gate.c
WASM_PICO_PLANE     = $(BUILD_DIR)/$(notdir $(SRC_PICO_PLANE)).wasm
AOT_PICO_PLANE      = $(BUILD_DIR)/$(notdir $(SRC_PICO_PLANE)).aot

# Uncomment if you have this source file
SRC_PICO_WRITER      = source/tub_writer.c
WASM_PICO_WRITER     = $(BUILD_DIR)/$(notdir $(SRC_PICO_WRITER)).wasm
AOT_PICO_WRITER      = $(BUILD_DIR)/$(notdir $(SRC_PICO_WRITER)).aot

# List of all AOT targets to build
AOT_TARGETS = $(AOT_PI) $(AOT_PICO_IN) $(AOT_PICO_ROUTING) $(AOT_PICO_SECURITY) $(AOT_PICO_PLANE) $(AOT_PICO_WRITER)

# Default target builds all AOT binaries
.PHONY: all
all: $(BUILD_DIR) $(AOT_TARGETS)

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Build the WebAssembly binary from C source
$(WASM_PI): $(SRC_PI) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PI_OPTS) $< -o $@

$(WASM_PICO_IN): $(SRC_PICO_IN) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $< -o $@

$(WASM_PICO_OUT): $(SRC_PICO_OUT) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $< -o $@

$(WASM_PICO_ROUTING): $(SRC_PICO_ROUTING) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $< -o $@

$(WASM_PICO_SECURITY): $(SRC_PICO_SECURITY) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $< -o $@

$(WASM_PICO_PLANE): $(SRC_PICO_PLANE) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $< -o $@

$(WASM_PICO_WRITER): $(SRC_PICO_WRITER) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $< -o $@

# Build the AOT binary from the WebAssembly binary
$(AOT_PI): $(WASM_PI) | $(BUILD_DIR)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PI_OPTS) -o $@ $<

$(AOT_PICO_IN): $(WASM_PICO_IN) | $(BUILD_DIR)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $@ $<

$(AOT_PICO_OUT): $(WASM_PICO_OUT) | $(BUILD_DIR)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $@ $<

$(AOT_PICO_ROUTING): $(WASM_PICO_ROUTING) | $(BUILD_DIR)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $@ $<

$(AOT_PICO_SECURITY): $(WASM_PICO_SECURITY) | $(BUILD_DIR)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $@ $<

$(AOT_PICO_PLANE): $(WASM_PICO_PLANE) | $(BUILD_DIR)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $@ $<

$(AOT_PICO_WRITER): $(WASM_PICO_WRITER) | $(BUILD_DIR)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $@ $<

# Clean up build artifacts
.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
