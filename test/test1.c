#include "../pico_plane.c"
#include "test_constants.h"

export int main(void) {
	reset_module();
    init(&T1);
    subscribe_to_event(EVENT_MESSAGE_RECEIVED);
    while(1){
        in(&this);
        led_set_color(0xffffff);
        loop();
        sleep(100);
    }
}