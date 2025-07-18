#include "common.h"


/// @brief checks the payload of a tub, determines its destination and resets the should_check value.
/// @return the destination type of the tub after the check.
uint8_t security_check(Module* module) {
	printf("I am checking the tub, %d.\n", get_rfid_data(TUB_ID));

	// Consume the should_check token
	module->should_check = 0;

	// Set that the tub has passed security
	module->tub[RFID].passed_security = 1;
	// Get the plane id of the tub
	int plane_id = get_rfid_data(PLANE_ID);

	// If the payload is "safe", so not 0
	if (get_rfid_data(PAYLOAD)) {
		// If the tub is going to drop-off, set the destination to the nearest drop-off
		if (get_rfid_data(PLANE_OR_DROPOFF)) {
			module->tub[RFID].destination_id = module->nearest[DROPOFF];
			module->tub[RFID].destination_type = DROPOFF;
			printf("I am rerouting to Drop-Off.\n");
			return DROPOFF;
		}

		// If the plane of the tub has arrived, set the destination to the plane
		if (module->plane_to_id[plane_id] > 0) {
			// plane arrived
			module->tub[RFID].destination_id = module->plane_to_id[plane_id];
			module->tub[RFID].destination_type = PLANE;
			module->tub[RFID].plane_id = plane_id;
			printf("I am rerouting to plane %d on module %d.\n", module->tub[RFID].destination_id,
				   module->tub[RFID].destination_type);
			return PLANE;
		}

		// If the plane of the tub has not arrived, set the destination to the nearest storage
		module->tub[RFID].destination_id = module->nearest[STORAGE];
		module->tub[RFID].destination_type = STORAGE;
		printf("I am rerouting to Storage. \n");
		return STORAGE;
	}
	// If the payload is "unsafe", so 0, rerout the tub to quarantine
	module->tub[RFID].destination_id = module->nearest[QUARANTINE];
	module->tub[RFID].destination_type = QUARANTINE;
	printf("I am rerouting to Quarantine.\n");
	return QUARANTINE;
}

/// @brief Reroutes the tub from the security module to its destination after passing the security check.
void reroute_from_security(Module* module) {
	// Create a request to send to the next module
	uint8_t request[REQ_LENGTH];
	request[MSG_SENDER] = module->id;
	request[MSG_TYPE] = REQUEST_MOVEMENT;
	request[REQ_DEST_ID] = module->tub[RFID].destination_id;
	request[REQ_DEST_TYPE] = module->tub[RFID].destination_type;
	request[REQ_TUB_ID] = module->tub[RFID].id;
	request[REQ_PLANE_ID] = module->tub[RFID].plane_id;

	// Determine the direction within the module, leading to the destination
	Direction to = module->dir_lookup[module->tub[RFID].destination_id];

	// Reroute the tub to the direction towards the destination
	printf("I am rerouting to dir: %d with final destination %d and destination type %d \n", to, request[REQ_DEST_ID],
		   request[REQ_DEST_TYPE]);
	add_task(module, RFID, to, request);
	add_task(module, to, OUT, request);
}

export int main(void) {
	// Initialize the module
	Module module = module_init();
	// Initialize the should_check variable to 0
	module.should_check = 0;

	// Loop infinitely, checking if a tub should go to a security check and routing it accordingly
	while (1) {
		// If a security check should be done to the tub
		if (module.should_check) {
			printf("I get to security\n");
			// If there is a tub on the RFID reader
			if (RFID_check_tag()) {
				// Do a security check on the tub and reroute it to its destination
				security_check(&module);
				reroute_from_security(&module);
			}
		}
		// Deal with general routing of tubs
		loop(&module);

		// Sleep for a bit to add reliability
		sleep(100);
	}
}
