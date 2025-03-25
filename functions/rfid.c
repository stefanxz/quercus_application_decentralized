#pragma once
#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"
#include <stdbool.h>
#include "rfid.h"

int get_rfid_data(int type) {
	if (!RFID_check_tag()) return -1;
	if (type == PAYLOAD) {
		//TODO: Check whether module is security
	}

	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, type);
	return data[0];
}

int get_entrance_rfid_data(char* rfid_data) {
	if (!RFID_check_tag()) return -1;
	char block_data[BLOCK_SIZE];
	for (int i = 0; i < END_OF_ENUM; ++i) {
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
	if (!get_rfid_data(SECURITY)) return -2;
	//TODO: Add check whether module is security
	char data[BLOCK_SIZE];
	data[0] = flag;
	RFID_write_data_block((int)data, SECURITY);
	return 0;
}

int set_plane_arrived() {
	if (!RFID_check_tag()) return -1;
	if (!get_rfid_data(PLANE_DROPOFF)) return -2;
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
