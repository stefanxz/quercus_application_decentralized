#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../functions/rfid.c"
#include "../functions/algorithm.c"

#include "../functions/algorithm.h"
#include "../functions/network.h"

#define COLOR_CYAN 0x00FFFF

State state;

int in(Module* module) {
	char* msg;
	led_set_color(COLOR_CYAN);
	
	if (RFID_check_tag()) {
		if(get_rfid_data(TUB_OR_PLANE) == 1){
			int plane_id = get_rfid_data(PLANE_ID);
			bool plane_has_arrived = module -> plane_to_id[plane_id] == module -> id;
			if (plane_has_arrived) return 0;
		} else {
			save_RFID_data(module);
			char request[REQ_LENGTH];
			request[MSG_SENDER] = module -> id;
			request[MSG_TYPE] = REQUEST_MOVEMENT;

			// FIX
			request[REQ_TUB_ID] = module -> tub.id;

			request[REQ_PLANE_ID] = module -> tub.plane_id;
			request[REQ_PLANE_ARRIVED] = module -> tub.plane_arrived;
			request[REQ_DEST_ID] = 6; // hardcoded
			request[REQ_DEST_TYPE] = STORAGE;

			// FIX
			request[REQ_SECURITY] = module -> tub.passed_security;
			request[REQ_PAYLOAD] = -17; //FAKE
			
			add_task(module, RFID, module->direction_lookup[module->tub.destination], request);
			add_task(module, module->direction_lookup[module->tub.destination], OUT, request);

			state.at[RFID] = module -> tub.id;
		}
	}
	
}
