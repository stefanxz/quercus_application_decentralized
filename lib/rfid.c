#pragma once

#include "../libc_builtin.h"
#include "../quercus_lib_pico.h"
#include "rfid.h"
#include "essentials.h"

int get_rfid_data(int type) {
	if (!RFID_check_tag()) return -1;
	if (type == PAYLOAD) {
		// TODO: Check whether module is security
	}

	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, type);
	return data[0];
}

int get_entrance_rfid_data(char* rfid_data) {
	if (!RFID_check_tag()) return -1;
	char block_data[BLOCK_SIZE];
	for (int i = 0; i < RFID_LENGTH; ++i) {
		RFID_read_data_block((int)block_data, i);
		rfid_data[i] = block_data[0];
	}
	return 0;
}

int set_tub_id(int tub_id) {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	data[0] = tub_id;
	RFID_write_data_block((int)data, TUB_ID);
	return 0;
}

int set_security_flag(int flag) {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	data[0] = flag;
	RFID_write_data_block((int)data, TUB_ID);
	return 0;
}

int set_security_passed(int flag) {
	if (!RFID_check_tag()) return -1;
	if (!get_rfid_data(NEEDS_SECURITY)) return -2;
	// TODO: Add check whether module is security
	char data[BLOCK_SIZE];
	data[0] = flag;
	RFID_write_data_block((int)data, NEEDS_SECURITY);
	return 0;
}

int set_plane_arrived() {
	if (!RFID_check_tag()) return -1;
	if (!get_rfid_data(PLANE_OR_DROPOFF)) return -2;
	char data[BLOCK_SIZE];
	data[0] = 0x1;
	RFID_write_data_block((int)data, PLANE_ARRIVED);
	return 0;
}

int set_destination(int dest) {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	data[0] = dest;
	RFID_write_data_block((int)data, DESTINATION);
	return 0;
}

//FIX move from here
bool check_plane_arrived(Module module, int tub_plane_id) { return module.plane_to_id[tub_plane_id] != 0; }

//FIX move from here
void determine_destination(Module* module, bool sec_check_passed, bool sec_check_needed, bool plane_dropoff,
	int plane_id, int* destination, int* destination_type) {
	if (sec_check_needed) {
		if (sec_check_passed) {
			*destination_type = QUARANTINE;
			*destination = module->nearest[QUARANTINE];
		} else {
			*destination_type = SECURITY;
			*destination = module->nearest[SECURITY];
		} 
	} else if (plane_dropoff) {
		*destination_type = DROPOFF;
		*destination = module->nearest[DROPOFF];
	} else if (check_plane_arrived(*module, plane_id)) {
		*destination_type = PLANE;
		*destination = module->plane_to_id[plane_id];
	} else {
		*destination_type = STORAGE;
		*destination = module->nearest[STORAGE];
	}
	printf("reading: %d, %d\n", *destination, *destination_type);
}

