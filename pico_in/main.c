#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../functions/rfid.c"
#include "../functions/algorithm.c"

#include "../functions/network.h"

#include <stdbool.h>

#define COLOR_CYAN 0x00FFFF
int in(EventType e, Module* module) {
	char* msg;
	led_set_color(0xffffff);
	sleep(5000);
	led_set_color(COLOR_CYAN);
	while(true) {
		e = next_event();
		if (RFID_check_tag()) {
			if(get_rfid_data(TUB_PLANE) == 1){
				// int plane_id = get_rfid_data(PLANE_ID);
				// bool plane_has_arrived = module -> plane_to_id[plane_id] == module -> id;
				if (1 /* plane_has_arrived */) return 0;
			} else {
				save_RFID_data(module);
				printf("Tub finally has: id:%d, passed_sec:%d, plane_dropoff:%d, destination: %d, plane_id:%d\n", module -> tub.id, module -> tub.passed_security, module->tub.plane_dropoff, module->tub.destination,  module-> tub.plane_id);
				sleep(5000);
			}
		} 
		if (e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(&msg);
			int type = (int)&msg[MESSAGE_TYPE];
			if (type == REQUEST_MOVEMENT) {
				//Send accept message
				//Move the actuators
			} else if (type == PLANE_STATUS){
				//Read plane status
				//Save to module
			} else if (type == TUB_STATUS) {
				printf("MESSAGE TYPE WAS WRONG, I DO NOT DO TUB STATUS\n");
			} else if (type == PATHS_CONFIG) {
				// Save lookup table
			} else if (type == TUB_LOGGING) {
				printf("MESSAGE TYPE WAS WRONG, I DO NOT DO LOGGING\n");
			}
		}
		sleep(10);
	}
	
}
