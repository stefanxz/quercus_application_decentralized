#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../functions/movement.c"
#include "../functions/rfid.c"
#include "../functions/algorithm.c"

#include "../functions/network.h"

#include <stdbool.h>

int in(EventType e, Module* module_ptr) {
	char* msg;
	while(true) {
		e = next_event();
		if (RFID_check_tag()) {
			//if(rfid is a leaving plane) -> return 0;
			save_RFID_data(module_ptr);
		} 
		if (e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(&msg);
			int type = get_message_type(&msg);
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
