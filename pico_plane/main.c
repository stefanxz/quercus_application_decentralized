#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../functions/movement.c"
#include "../functions/rfid.h"
#include "../functions/network.c"
#include "../functions/graph.c"
// #include "../functions/algorithm.c"
#include "../pico_in/main.c"

#include <stdbool.h>

#define MAX_NUMBER_OF_TUBS 2
#define MAX_NUMBER_OF_PLANES 100
#define MAX_NUMBER_OF_MODULES 8

int get_message_type(char* msg){
	return msg[1];
}

export int main(void) {
	Module module;
	Module* module_ptr = &module;
	enum EventType e;
	char * msg;
	subscribe_to_event(EVENT_MESSAGE_RECEIVED | EVENT_LASER_LEFT_DETECT | EVENT_LASER_RIGHT_DETECT);
	while(true) {
		led_set_color(LED_RED);
		if(RFID_check_tag()){
			int counter = 0;
			int direction = get_rfid_data(PLANE_DIRECTION);
			while(RFID_check_tag) {
				int new_direction = get_rfid_data(PLANE_DIRECTION);
				if(direction == new_direction) counter++;
				else counter = 0;
				if(counter > 5) break;
				direction = new_direction;
				sleep(10);
			}
			//Tag is plane
			if(get_rfid_data(TUB_PLANE) == 1){
				int plane_id = get_rfid_data(PLANE_ID);
				int direction = get_rfid_data(PLANE_DIRECTION);
				if(direction == 1){
					// send_plane_status(plane_id, plane_arrival);
					int result = in(e, module_ptr);
					printf("Result is: %d\n", result);
					//Plane left properly
					if(result == 0) {
						led_set_color(0xffa500);
						sleep(5000);
						continue;
					}
				} else if(direction == 0) {
					//implement the out logic
					printf("I am living eternally, SEP is going to be the biggest success of our life\n");
				}
			}
		}
		sleep(20);
	}
	return 0;
}
