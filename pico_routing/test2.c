#include "main.c"
#include "test_constants.h"

export int main(void) {
	reset_module();
    init(&T2);
    subscribe_to_event(EVENT_MESSAGE_RECEIVED);
    // EventType e = next_event();
    // while(1){
    //     if(e == EVENT_MESSAGE_RECEIVED) {
    //         printf("Message received.\n");
    //     }
    //     e = next_event();
    //     sleep(500);
    // }
    while(1){
        loop();
        sleep(1000);
    }
}
