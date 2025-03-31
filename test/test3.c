#include "../pico_routing.c"
#include "test_constants.h"
#include "../lib/writer.c"

export int main(void) {
    reset_module();
    write();
}