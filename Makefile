# Makefile for building WebAssembly and AOT binaries

# Tools
CC       ?= clang
WAMRC    ?= wamrc

# Compilation flags for building WebAssembly
# -Wall: Enable all warnings
# -Wextra: Enable extra warnings
# -Wno-incompatible-library-redeclaration: Suppress warnings about redefined functions
# since we are not using the real stdlib anyway
CFLAGS   = -Wall -Wextra -Wno-incompatible-library-redeclaration
WASM_OPTS = -target wasm32 -nostdlib \
		   -Wl,--no-entry \
		   -Wl,--allow-undefined \
		   -Wl,--export-dynamic \
		   -Os

WASM_PI_OPTS = -Wl,-z,stack-size=128 \
			   -Wl,--max-memory=65536 \

WASM_PICO_OPTS = -Wl,-z,stack-size=128 \
				 -Wl,--max-memory=65536 \


# Options for wamrc to compile wasm to AOT
WAMRC_OPTS = --xip

WAMRC_PICO_OPTS = --target=thumbv6m --target-abi=eabi --cpu=cortex-m0 \
				  --enable-builtin-intrinsics=all

WAMRC_PI_OPTS = --target=armv8a --target-abi=eabi --cpu=cortex-a53 \
				--enable-builtin-intrinsics=all

# Files
SRC_PI      = pi/main.c
WASM_PI     = $(SRC_PI).wasm
AOT_PI      = $(SRC_PI).aot

SRC_PICO_IN      = pico_in/main.c
WASM_PICO_IN     = $(SRC_PICO_IN).wasm
AOT_PICO_IN      = $(SRC_PICO_IN).aot

SRC_PICO_OUT      = pico_out/main.c
WASM_PICO_OUT     = $(SRC_PICO_OUT).wasm
AOT_PICO_OUT      = $(SRC_PICO_OUT).aot

SRC_PICO_ROUTING      = pico_routing/main.c
WASM_PICO_ROUTING     = $(SRC_PICO_ROUTING).wasm
AOT_PICO_ROUTING      = $(SRC_PICO_ROUTING).aot

SRC_PICO_SECURITY      = pico_security/main.c
WASM_PICO_SECURITY     = $(SRC_PICO_SECURITY).wasm
AOT_PICO_SECURITY      = $(SRC_PICO_SECURITY).aot


# Default target builds all AOT binaries
all: $(AOT_PI) $(AOT_PICO_IN) $(AOT_PICO_OUT) $(AOT_PICO_ROUTING) $(AOT_PICO_SECURITY)

# Build the WebAssembly binary from C source
$(WASM_PI): $(SRC_PI)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PI_OPTS) $(SRC_PI) -o $(WASM_PI)

$(WASM_PICO_IN): $(SRC_PICO_IN)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $(SRC_PICO_IN) -o $(WASM_PICO_IN)

$(WASM_PICO_OUT): $(SRC_PICO_OUT)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $(SRC_PICO_OUT) -o $(WASM_PICO_OUT)

$(WASM_PICO_ROUTING): $(SRC_PICO_ROUTING)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $(SRC_PICO_ROUTING) -o $(WASM_PICO_ROUTING)

$(WASM_PICO_SECURITY): $(SRC_PICO_SECURITY)
	$(CC) $(CFLAGS) $(WASM_OPTS) $(WASM_PICO_OPTS) $(SRC_PICO_SECURITY) -o $(WASM_PICO_SECURITY)


# Build the AOT binary from the WebAssembly binary
$(AOT_PI): $(WASM_PI)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PI_OPTS) -o $(AOT_PI) $(WASM_PI) > /dev/null

$(AOT_PICO_IN): $(WASM_PICO_IN)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $(AOT_PICO_IN) $(WASM_PICO_IN) > /dev/null

$(AOT_PICO_OUT): $(WASM_PICO_OUT)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $(AOT_PICO_OUT) $(WASM_PICO_OUT) > /dev/null

$(AOT_PICO_ROUTING): $(WASM_PICO_ROUTING)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $(AOT_PICO_ROUTING) $(WASM_PICO_ROUTING) > /dev/null

$(AOT_PICO_SECURITY): $(WASM_PICO_SECURITY)
	$(WAMRC) $(WAMRC_OPTS) $(WAMRC_PICO_OPTS) -o $(AOT_PICO_SECURITY) $(WASM_PICO_SECURITY) > /dev/null


# Clean up build artifacts
clean:
	rm -f $(WASM_PI) $(AOT_PI) \
		  $(WASM_PICO_IN) $(AOT_PICO_IN) \
		  $(WASM_PICO_OUT) $(AOT_PICO_OUT) \
		  $(WASM_PICO_ROUTING) $(AOT_PICO_ROUTING) \
		  $(WASM_PICO_SECURITY) $(AOT_PICO_SECURITY)

.PHONY: all clean
