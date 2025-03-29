#include "../pico_routing.c"
#include "test_constants.h"

export int main(void) {
    reset_module();
    init(&T4);
    subscribe_to_event(EVENT_MESSAGE_RECEIVED);

    while(1){
        loop();
        sleep(100);
    }
}