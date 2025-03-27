#include "main.c"
#include "test_constants.h"
#include "../examples.c"

export int main(void) {
    reset_module();
    init(&T5);
    subscribe_to_event(EVENT_MESSAGE_RECEIVED);

    while(1){
        loop();
        sleep(100);
    }
    // demo();
}
