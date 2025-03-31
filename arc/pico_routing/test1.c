#include "main.c"
#include "test_constants.h"
#include "../pico_in/main.c"

export int main(void) {
	reset_module();
    init();
    subscribe_to_event(EVENT_MESSAGE_RECEIVED);
    while(1){
        in(&this);
        led_set_color(0xffffff);
        loop();
        sleep(100);
    }
}