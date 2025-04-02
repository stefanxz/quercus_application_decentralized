#include "libc_builtin.h"
#include "quercus_lib_pico.h"

#include "lib/movement.h"
#include "lib/rfid.c"
#include "pico_routing.c"

#define COLOR_CYAN 0x00FFFF

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


//FIX move from here
bool check_plane_arrived(int tub_plane_id) { return this.plane_to_id[tub_plane_id] != 0; }

//FIX move from here
void determine_destination(bool sec_check_passed, bool sec_check_needed, bool plane_dropoff,
	int plane_id, int* destination, int* destination_type) {
	if (sec_check_needed) {
		if (sec_check_passed) {
			*destination_type = QUARANTINE;
			*destination = this.nearest[QUARANTINE];
		} else {
			*destination_type = SECURITY;
			*destination = this.nearest[SECURITY];
		} 
	} else if (plane_dropoff) {
		*destination_type = DROPOFF;
		*destination = this.nearest[DROPOFF];
	} else if (check_plane_arrived(plane_id)) {
		*destination_type = PLANE;
		*destination = this.plane_to_id[plane_id];
	} else {
		*destination_type = STORAGE;
		*destination = this.nearest[STORAGE];
	}
	printf("reading: %d, %d\n", *destination, *destination_type);
}

//Might want to make this a bit more complex.
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
	
	this.tub[RFID] = create_tub(tub_id, has_passed_security, plane_dropoff, plane_arrived, tub_destination_id, tub_destination_type, plane_id);
	determine_priority(&this.tub[RFID]);
}

int in() {
	char* msg;
	led_set_color(COLOR_CYAN);
	if (RFID_check_tag()) {
		sleep(100);
		if (get_rfid_data(TUB_OR_PLANE) == 1) {
			return 0;
		} else {
			if(is_storing() > NON) {
				printf("SOMETHING WENT WRONG!\n");
				handle_storage(RFID);
			}
			save_RFID_data();
			char request[REQ_LENGTH];
			request[MSG_SENDER] = this.id;
			request[MSG_TYPE] = REQUEST_MOVEMENT;

			request[REQ_TUB_ID] = this.tub[RFID].id;

			request[REQ_PLANE_ID] = this.tub[RFID].plane_id;
			request[REQ_PLANE_ARRIVED] = this.tub[RFID].plane_arrived;
			request[REQ_DEST_ID] = this.tub[RFID].destination_id;
			//FIX
			request[REQ_DEST_TYPE] = this.tub[RFID].destination_type;

			// FIX
			// request[REQ_SECURITY] = this.tub[RFID].passed_security;
			add_task(RFID, this.dir_lookup[this.tub[RFID].destination_id], request);
			add_task(this.dir_lookup[this.tub[RFID].destination_id], OUT, request);

			state.at[RFID] = this.tub[RFID].id;
		}
	}
}
