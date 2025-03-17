#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"
#include <stdbool.h>
#include "rfid_functions.h"

int get_security_flag() {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, 0);
	return data[0];
}

int get_plane_dropoff_flag() {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, 1);
	return data[0];
}

int get_plane_id() {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, 2);
	return data[0];
}

int get_payload() {
	if (!RFID_check_tag()) return -1;
	//TODO: Add check whether module is security
	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, 3);
	return data[0];
}

int has_security_been_passed() {
	if (!RFID_check_tag()) return -1;
	if (!get_security_flag()) return -2;
	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, 6);
	return data[0];
}

int has_plane_arrived() {
	if (!RFID_check_tag()) return -1;
	if (!get_plane_dropoff_flag()) return -2;
	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, 7);
	return data[0];
}

int get_destination() {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, 8);
	return data[0];
}

int set_security_passed() {
	if (!RFID_check_tag()) return -1;
	if (!get_security_flag()) return -2;
	//TODO: Add check whether module is security
	char data[BLOCK_SIZE];
	data[0] = 0x1;
	RFID_write_data_block((int)data, 6);
	return 0;
}

int set_plane_arrived() {
	if (!RFID_check_tag()) return -1;
	if (!get_plane_dropoff_flag()) return -2;
	char data[BLOCK_SIZE];
	data[0] = 0x1;
	RFID_write_data_block((int)data, 7);
	return 0;
}

int set_destination(int dest) {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	data[0] = dest;
	RFID_write_data_block((int)data, 8);
	return 0;
}