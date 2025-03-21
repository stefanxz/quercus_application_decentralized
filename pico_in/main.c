#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../elementary_functions/movement_functions.c"
#include "../elementary_functions/rfid_functions.c"
#include "../algo_functions.c"

#include <stdbool.h>

#define MAX_NUMBER_OF_TUBS 2
#define MAX_NUMBER_OF_PLANES 100
#define MAX_NUMBER_OF_MODULES 8


int in(EventType e, Module* module_ptr) {
	char* msg;
	while(true) {
		e = next_event();
		if (e == EVENT_RFID_DETECT /*detect rfid somehow*/) {
			//if(rfid is a leaving plane) -> return 0;
			save_RFID_data(module_ptr);
		} else if (e == EVENT_MESSAGE_RECEIVED) {
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
			} else if (type == LOGGING) {
				printf("MESSAGE TYPE WAS WRONG, I DO NOT DO LOGGING\n");
			}
		}
		sleep(10);
	}
	
}
