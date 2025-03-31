#include "libc_builtin.h"
#include "quercus_lib_pico.h"

#include "lib/movement.h"
#include "lib/rfid.c"
#include "pico_routing.c"

#define COLOR_CYAN 0x00FFFF

// FAKE HAS TO BE IMPLEMENTED
int broadcast_plane_status(int plane_id, int module_id) {
	char data[ARR_LENGTH];
	data[MSG_SENDER] = module_id;
	data[MSG_TYPE] = PLANE_STATUS;
	data[ARR_PLANE_ID] = plane_id;
	data[ARR_MODULE_ID] = module_id;
	return send_packet(0, data, sizeof(data));
}

Tub create_tub(int id, bool passed_security, bool plane_dropoff, bool plane_arrived, int destination, int destination_type, int plane_id) {
	Tub tub;
	tub.id = id;
	tub.passed_security = passed_security;
	tub.plane_dropoff = plane_dropoff;
	tub.destination_id = destination;
	tub.destination_type = destination_type;
	tub.plane_arrived = plane_arrived;
	tub.plane_id = plane_id;
	return tub;
}


/// @brief Saves the data of a tub to the given module.
/// @param module pointer to the Module that receives the rfid readings.
void save_RFID_data(Module* module) {
	// tub detected
	char data[RFID_LENGTH];
	get_entrance_rfid_data(data);
	int tub_id = (int)data[TUB_ID];
	bool has_passed_security = (bool)data[PASSED_SECURITY];
	bool security_bit = (bool)data[NEEDS_SECURITY];
	bool plane_dropoff = (bool)data[PLANE_OR_DROPOFF];

	int plane_id = (int)data[PLANE_ID];
	bool plane_arrived = module->plane_to_id[plane_id];
	int tub_destination_id;
	int tub_destination_type;
	determine_destination(module, has_passed_security, security_bit, plane_dropoff, plane_id, &tub_destination_id, &tub_destination_type);
	// write tub destination to module
	// int tub_priority = -1;
	// printf("Tub gets: id: %d, passed_sec:%d, sec_bit:%d, plane_dropoff:%d, plane_id:%d\n", tub_id,
	// has_passed_security, security_bit, plane_dropoff, plane_id);
	module->tub = create_tub(tub_id, has_passed_security, plane_dropoff, plane_arrived, tub_destination_id, tub_destination_type, plane_id);
	// send tub status message -> entry
}

void change_tub_status(/* Module module */) {
	// set_security_passed(1);
	// set_needs_security(1 /* get_payload() */);
	// set_destination(1 /* determine_destination(module, get_payload(), 1, module.tub.plane_dropoff,
	// module.tub.plane_id)*/);
}

int in(Module* module) {
	char* msg;
	led_set_color(COLOR_CYAN);
	if (RFID_check_tag()) {
		sleep(100);
		if (get_rfid_data(TUB_OR_PLANE) == 1) {
			printf("dogpoop\n");
			return 0;
		} else {
			save_RFID_data(module);
			char request[REQ_LENGTH];
			request[MSG_SENDER] = module->id;
			request[MSG_TYPE] = REQUEST_MOVEMENT;

			// FIX
			request[REQ_TUB_ID] = module->tub.id;

			request[REQ_PLANE_ID] = module->tub.plane_id;
			request[REQ_PLANE_ARRIVED] = module->tub.plane_arrived;
			request[REQ_DEST_ID] = module->tub.destination_id;
			//FIX
			request[REQ_DEST_TYPE] = module->tub.destination_type;

			// FIX
			request[REQ_SECURITY] = module->tub.passed_security;
			request[REQ_PAYLOAD] = -17; // FAKE
			add_task(RFID, module->dir_lookup[module->tub.destination_id], request);
			add_task(module->dir_lookup[module->tub.destination_id], OUT, request);

			state.at[RFID] = module->tub.id;
		}
	}
}
