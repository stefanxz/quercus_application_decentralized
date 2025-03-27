#include "main.c"
#include "test_constants.h"

export int main(void) {
	reset_module();
    init(&T2);
    subscribe_to_event(EVENT_MESSAGE_RECEIVED);

    while(1){
        loop();
        sleep(100);
    }
}
