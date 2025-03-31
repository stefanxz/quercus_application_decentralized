#include "../pico_routing.c"
#include "test_constants.h"

export int main(void) {
    reset_module();
    init(&T4);

    while(1){
        loop();
        sleep(100);
    }
}