#include "main.c"
#include "test_constants.h"
#include "../pico_in/main.c"

export int main(void) {
	reset_module();
    init(&T1);
    subscribe_to_event(EVENT_MESSAGE_RECEIVED);
    while(1){
        in(&this);
        loop();
        sleep(100);
    }
}