#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../functions/rfid.c"
#include "../functions/algorithm.c"

#include "../functions/network.h"

#include <stdbool.h>

#define COLOR_CYAN 0x00FFFF

State state;

int in(Module* module) {
	char* msg;
	led_set_color(0xff0000);
	// sleep(5000);
	led_set_color(COLOR_CYAN);
	// while(true) {
		if (RFID_check_tag()) {
			if(get_rfid_data(TUB_PLANE) == 1){
				int plane_id = get_rfid_data(PLANE_ID);
				bool plane_has_arrived = module -> plane_to_id[plane_id] == module -> id;
				if (plane_has_arrived) return 0;
			} else {
				save_RFID_data(module);
				Request request;
				request.tub_id = module -> id;
				request.sender_id = module -> id;
				request.plane_id = module -> tub.plane_id;
				request.plane_arrived = module -> tub.plane_arrived;
				request.destination = module -> tub.destination;
				request.plane_or_drop_off = module -> tub.plane_dropoff;
				request.security = module -> tub.passed_security;
				request.security_status = module -> tub.passed_security;
				
				add_task(module, RFID, module->direction_lookup[module->tub.destination], request);
				add_task(module, module->direction_lookup[module->tub.destination], OUT, request);
				state.at[RFID] = module -> tub.id;
				// sleep(5000);
			}
		} 
		// sleep(10);
	// }
	
}
