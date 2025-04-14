#include "libc_builtin.h"
#include "quercus_lib_pico.h"

#include "movement.h"
#include "rfid.c"
#include "pico_routing.c"

#define COLOR_CYAN 0x00FFFF

/// @brief Send a packet to the Pi to indicate the status of a plane.
/// @param plane_id ID of the plane to be updated.
/// @param module_id ID of the module sending the update.
/// @return 0 on success, < 0 on failure.
int broadcast_plane_status(int plane_id, int dep_time, int module_id) {
	char data[ARR_LENGTH];
	data[MSG_SENDER] = module_id;
	data[MSG_TYPE] = PLANE_STATUS;
	data[ARR_PLANE_ID] = plane_id;
	data[ARR_MODULE_ID] = module_id;
	data[ARR_DEP_TIME] = dep_time;
	return send_packet(0, data, sizeof(data));
}

/// @brief Create a Tub object with the given parameters.
/// @param id ID of the tub.
/// @param passed_security Indicates if the tub has passed through the security module.
/// @param plane_dropoff Indicates if the tub is going to a plane or drop-off.
/// @param plane_arrived Indicates if the plane has arrived at the module.
/// @param destination ID of the destination module.
/// @param destination_type Type of the destination (e.g., PLANE, STORAGE, SECURITY).
/// @param plane_id ID of the plane associated with the tub.
/// @return A Tub object initialized with the given parameters.
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


/// @brief Check if a plane has arrived at the module.
/// @param tub_plane_id ID of the plane to check.
bool check_plane_arrived(int tub_plane_id) { return this.plane_to_id[tub_plane_id] > 0; }

/// @brief Determine the destination of a tub based on its security check status and other parameters.
/// @param sec_check_passed Indicates if the tub has passed through the security module.
/// @param sec_check_needed Indicates if a security check is needed for the tub.
/// @param plane_dropoff Indicates if the tub is going to a plane or drop-off.
/// @param plane_id ID of the plane associated with the tub.
/// @param destination Pointer to store the determined destination ID.
/// @param destination_type Pointer to store the determined destination type.
void determine_destination(bool sec_check_passed, bool sec_check_needed, bool plane_dropoff,
	int plane_id, int* destination, int* destination_type) {
	// If the tub needs a security check
	if (sec_check_needed) {
		// If the security check has passed, send it to quarantine; otherwise, send it to security
		if (sec_check_passed) {
			*destination_type = QUARANTINE;
			*destination = this.nearest[QUARANTINE];
		} else {
			*destination_type = SECURITY;
			*destination = this.nearest[SECURITY];
		} 
	// If the tub is going to a drop-off, send it to the nearest drop-off
	} else if (plane_dropoff) {
		*destination_type = DROPOFF;
		*destination = this.nearest[DROPOFF];
	// If the tub is going to a plane and its plane has arrived, send it to it
	} else if (check_plane_arrived(plane_id)) {
		*destination_type = PLANE;
		*destination = this.plane_to_id[plane_id];
	// If the tub is going to storage, send it to the nearest storage module
	} else {
		*destination_type = STORAGE;
		*destination = this.nearest[STORAGE];
	}
	printf("reading: %d, %d\n", *destination, *destination_type);
}

/// @brief Determine the priority of a tub based on its destination type.
/// @param tub Pointer to the tub whose priority is to be determined.
void determine_priority(Tub* tub){
	if(tub->destination_type == PLANE) {
		tub->priority = PRIO_HI;
	}
	else if(tub -> destination_type == STORAGE){
		tub->priority = PRIO_LO;
	} else {
		tub->priority = PRIO_ME;
	}
	printf("Tub %d is assigned priorit: %d\n", tub->id, tub->priority);
}

/// @brief Saves the data of a tub to this module.
void save_RFID_data() {
	// tub detected
	char data[RFID_LENGTH];
	get_entrance_rfid_data(data);
	int tub_id = (int)data[TUB_ID];
	bool has_passed_security = (bool)data[PASSED_SECURITY];
	bool security_bit = (bool)data[NEEDS_SECURITY];
	bool plane_dropoff = (bool)data[PLANE_OR_DROPOFF];
	

	int plane_id = (int)data[PLANE_ID];
	bool plane_arrived = this.plane_to_id[plane_id];
	int tub_destination_id;
	int tub_destination_type;
	determine_destination(has_passed_security, security_bit, plane_dropoff, plane_id, &tub_destination_id, &tub_destination_type);
	printf("I am going to destination: %d with type: %d\n", tub_destination_id, tub_destination_type);
	this.tub[RFID] = create_tub(tub_id, has_passed_security, plane_dropoff, plane_arrived, tub_destination_id, tub_destination_type, plane_id);
	determine_priority(&this.tub[RFID]);
}

/// @brief Detects and handles the entry of a tub into the module via RFID reader
int in() {
	char* msg;
	// Set the LED color to cyan to indicate waiting for a tag
	led_set_color(COLOR_CYAN);

	// If the RFID detects a tag
	if (RFID_check_tag()) {
		sleep(PAUSE);
		// If the detected tag is a plane, exit out of the function as it might mean the current plane is leaving
		if (get_rfid_data(TUB_OR_PLANE) == 1) {
			return 0;
		// If the detected tag is a tub
		} else {
			// If the module is in storage mode, handle the storage, if there is any 
			if(is_storing() > NON) {
				printf("I am storing at %d\n", is_storing());
				handle_storage();
			}

			// Save the RFID data to the module
			save_RFID_data();

			// Create a request message for the movement of the tub
			char request[REQ_LENGTH];
			request[MSG_SENDER] = this.id;
			request[MSG_TYPE] = REQUEST_MOVEMENT;

			request[REQ_TUB_ID] = this.tub[RFID].id;

			request[REQ_PLANE_ID] = this.tub[RFID].plane_id;
			request[REQ_PLANE_ARRIVED] = this.tub[RFID].plane_arrived;
			request[REQ_DEST_ID] = this.tub[RFID].destination_id;
			request[REQ_DEST_TYPE] = this.tub[RFID].destination_type;

			// Reroute the tub from the RFID towards its destination
			add_task(RFID, this.dir_lookup[this.tub[RFID].destination_id], request);
			add_task(this.dir_lookup[this.tub[RFID].destination_id], OUT, request);

			// Update the state of the module to indicate that the tub is at the RFID position
			state.at[RFID] = this.tub[RFID].id;
		}
	}
}
