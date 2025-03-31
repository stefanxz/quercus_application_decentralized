#include "../pico_routing.c"
#include "test_constants.h"
#include "../examples.c"

export int main(void) {
    reset_module();
    init(&T5);

    while(1){
        loop();
        sleep(100);
    }
    // demo();
}
