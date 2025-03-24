// #include "../quercus_lib_pico.h"
// #include "../libc_builtin.h"

// #include "../elementary_functions/movement_functions.c"
// #include "../elementary_functions/rfid_functions.c"
// #include "../algo_functions.c"

// #include <stdbool.h>


// int main(void) {
// 	Module module;
// 	Module* module_ptr = &module;
// 	enum EventType e;
// 	char * msg;
// 	subscribe_to_event(EVENT_MESSAGE_RECEIVED | EVENT_RFID_DETECT | EVENT_LASER_LEFT_DETECT | EVENT_LASER_RIGHT_DETECT);
// 	while(1) {
// 		e = next_event();
// 		if (e == EVENT_RFID_DETECT) {
// 			(module_ptr);
// 		} else if (e == EVENT_MESSAGE_RECEIVED) {
// 			next_message_address(&msg);
// 			int type = get_message_type(&msg);
// 			if (type == REQUEST_MOVEMENT) {
// 				//Send accept message
// 				//Save request data to tub

// 				if(module.tub.destination == module.id){
// 					//Move to RFID
// 					//if (RFID detected) {
// 					change_tub_status(module);
// 					//}
// 				} else {
// 					//move(module);
// 				}
// 			} else if (type == PLANE_STATUS){
// 				//Read plane status
// 				//plane_to_id[plane_id] = plane_module_id;
// 			} else if (type == TUB_STATUS) {
// 				// Don't care
// 			} else if (type == PATHS_CONFIG) {
// 				// Save lookup table
// 			} else if (type == LOGGING) {
// 				printf("MESSAGE TYPE WAS WRONG, I DO NOT DO LOGGING\n");
// 			}
// 		}
// 		sleep(10);
// 	}
// 	return 0;
// }
