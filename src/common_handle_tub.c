#include "defs.h"
#include "common_hardware.h"
#include "common_loop.h"

#include "libc_builtin.h"
// bool check_plane_arrived(int tub_plane_id) { return this.plane_to_id[tub_plane_id] > 0; }

/// @brief Determine the destination of a tub based on its security check status and other parameters.
/// @param sec_check_passed Indicates if the tub has passed through the security module.
/// @param sec_check_needed Indicates if a security check is needed for the tub.
/// @param plane_dropoff Indicates if the tub is going to a plane or drop-off.
/// @param plane_id ID of the plane associated with the tub.
/// @param destination Pointer to store the determined destination ID.
/// @param destination_type Pointer to store the determined destination type.
void determine_destination(Module* module, bool sec_check_passed, bool sec_check_needed, bool plane_dropoff, int plane_id,
						   int* destination, int* destination_type) {
	// If the tub needs a security check
	if (sec_check_needed) {
		// If the security check has passed, send it to quarantine; otherwise, send it to security
		if (sec_check_passed) {
			*destination_type = DEST_QUARANTINE;
			*destination = module->nearest[DEST_QUARANTINE];
		} else {
			*destination_type = DEST_SECURITY;
			*destination = module->nearest[DEST_SECURITY];
		}
		// If the tub is going to a drop-off, send it to the nearest drop-off
	} else if (plane_dropoff) {
		*destination_type = DEST_DROPOFF;
		*destination = module->nearest[DEST_DROPOFF];
		// If the tub is going to a plane and its plane has arrived, send it to it
	} else if (module->plane_to_module_id[plane_id] > 0) {
	    // plane arrived
		*destination_type = DEST_PLANE;
		*destination = module->plane_to_module_id[plane_id];
		// If the tub is going to storage, send it to the nearest storage module
	} else {
		*destination_type = DEST_STORAGE;
		*destination = module->nearest[DEST_STORAGE];
	}
	printf("reading: %d, %d\n", *destination, *destination_type);
}

/// @brief Determine the priority of a tub based on its destination type.
/// @param tub Pointer to the tub whose priority is to be determined.
void determine_priority(Tub* tub) {
	if (tub->destination_type == DEST_PLANE) {
		tub->priority = PRIO_HI;
	} else if (tub->destination_type == DEST_STORAGE) {
		tub->priority = PRIO_LO;
	} else {
		tub->priority = PRIO_ME;
	}
	printf("Tub %d is assigned priorit: %d\n", tub->id, tub->priority);
}

/// @brief Saves the data of a tub to this module.
void save_RFID_data(Module* module) {
	// tub detected
	char data[DATA_RFID_LENGTH];
	get_entrance_rfid_data(data);
	int tub_id = (int)data[DATA_TUB_ID];
	bool has_passed_security = (bool)data[DATA_PASSED_SECURITY];
	bool security_bit = (bool)data[DATA_NEEDS_SECURITY];
	bool plane_dropoff = (bool)data[DATA_PLANE_OR_DROPOFF];

	int plane_id = (int)data[DATA_PLANE_ID];
	bool plane_arrived = module->plane_to_module_id[plane_id];
	int tub_destination_id;
	int tub_destination_type;
	determine_destination(module, has_passed_security, security_bit, plane_dropoff, plane_id, &tub_destination_id,
						  &tub_destination_type);
	printf("I am going to destination: %d with type: %d\n", tub_destination_id, tub_destination_type);
	// this.tub[DIR_RFID] = create_tub(tub_id, has_passed_security, plane_dropoff, plane_arrived, tub_destination_id,
	// tub_destination_type, plane_id);
	module->tub[DIR_RFID] = (Tub){
		.id = tub_id,
		.passed_security = has_passed_security,
		.plane_dropoff = plane_dropoff,
		.plane_arrived = plane_arrived,
		.destination_id = tub_destination_id,
		.destination_type = tub_destination_type,
		.plane_id = plane_id,
	};
	determine_priority(&module->tub[DIR_RFID]);
}

/// @brief Detects and handles the entry of a tub into the module via DIR_RFID reader
void handle_tub_rfid_entry(Module* module) {
	// Set the LED color to cyan to indicate waiting for a tag
	led_set_color(COLOR_CYAN);

	// If the DIR_RFID detects a tag
	if (RFID_check_tag()) {
		sleep(TIME_PAUSE);
		// If the detected tag is a plane, exit out of the function as it might mean the current plane is leaving
		if (get_rfid_data(DATA_TUB_OR_PLANE) == 1) {
			return;
			// If the detected tag is a tub
		} else {
			// If the module is in storage mode, handle the storage, if there is any
			if (is_storing(*module) != Q_NULL) {
				printf("I am storing at %d\n", is_storing(*module));
				handle_storage(module);
			}

			// Save the DIR_RFID data to the module
			save_RFID_data(module);

			// Create a request message for the movement of the tub
			uint8_t request[REQ_LENGTH];
			request[MSG_SENDER] = module->id;
			request[MSG_TYPE] = MSG_REQUEST_MOVEMENT;

			request[REQ_TUB_ID] = module->tub[DIR_RFID].id;

			request[REQ_PLANE_ID] = module->tub[DIR_RFID].plane_id;
			request[REQ_PLANE_ARRIVED] = module->tub[DIR_RFID].plane_arrived;
			request[REQ_DEST_ID] = module->tub[DIR_RFID].destination_id;
			request[REQ_DEST_TYPE] = module->tub[DIR_RFID].destination_type;

			// Reroute the tub from the DIR_RFID towards its destination
			add_task(module, DIR_RFID, module->dir_lookup[module->tub[DIR_RFID].destination_id], request);
			add_task(module, module->dir_lookup[module->tub[DIR_RFID].destination_id], DIR_OUT, request);

			// Update the state of the module to indicate that the tub is at the DIR_RFID position
			module->state.at[DIR_RFID] = module->tub[DIR_RFID].id;
		}
	}
}
