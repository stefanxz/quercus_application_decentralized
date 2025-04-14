/// @file rfid.C
/// @brief Provides functions for reading and writing RFID data blocks, including
///        tub ID, security flags, and other RFID-related operations.

#pragma once

#include "libc_builtin.h"
#include "quercus_lib_pico.h"
#include "rfid.h"

#include "essentials.h"

/// @brief Reads a single byte of RFID data from the specified data block type.
/// @param type The data block index/type to read from.
/// @return The first byte of the specified RFID data block, or -1 if no tag is detected.
int get_rfid_data(int type) {
	if (!RFID_check_tag()) return -1;

	char data[BLOCK_SIZE];
	RFID_read_data_block((int)data, type);
	return data[0];
}

/// @brief Reads multiple RFID data blocks (up to RFID_LENGTH) and stores the first byte of each into rfid_data.
/// @param rfid_data A pointer to a buffer that will store the read bytes.
/// @return 0 on success, or -1 if no tag is detected.
int get_entrance_rfid_data(char* rfid_data) {
	if (!RFID_check_tag()) return -1;
	char block_data[BLOCK_SIZE];
	for (int i = 0; i < RFID_LENGTH; ++i) {
		RFID_read_data_block((int)block_data, i);
		rfid_data[i] = block_data[0];
	}
	return 0;
}

/// @brief Writes a tub ID to the RFID tag.
/// @param tub_id The tub ID to be written.
/// @return 0 on success, or -1 if no tag is detected.
int set_tub_id(int tub_id) {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	data[0] = tub_id;
	RFID_write_data_block((int)data, TUB_ID);
	return 0;
}

/// @brief Writes a security flag to the RFID tag.
/// @param flag The security flag to be written.
/// @return 0 on success, or -1 if no tag is detected.
int set_security_flag(int flag) {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	data[0] = flag;
	RFID_write_data_block((int)data, TUB_ID);
	return 0;
}

/// @brief Sets the 'security passed' flag if the RFID tag indicates security is needed.
/// @param flag The value to set for the security-passed indicator.
/// @return 0 on success, -1 if no tag is detected, or -2 if the 'NEEDS_SECURITY' block indicates no security requirement.
int set_security_passed(int flag) {
	if (!RFID_check_tag()) return -1;
	if (!get_rfid_data(NEEDS_SECURITY)) return -2;
	// TODO: Add check whether module is security
	char data[BLOCK_SIZE];
	data[0] = flag;
	RFID_write_data_block((int)data, NEEDS_SECURITY);
	return 0;
}

/// @brief Marks that a plane has arrived if the RFID tag indicates a plane or drop-off scenario.
/// @return 0 on success, -1 if no tag is detected, or -2 if 'PLANE_OR_DROPOFF' block is not set.
int set_plane_arrived() {
	if (!RFID_check_tag()) return -1;
	if (!get_rfid_data(PLANE_OR_DROPOFF)) return -2;
	char data[BLOCK_SIZE];
	data[0] = 0x1;
	RFID_write_data_block((int)data, PLANE_ARRIVED);
	return 0;
}

/// @brief Writes a destination value to the RFID tag.
/// @param dest The destination code to be written.
/// @return 0 on success, or -1 if no tag is detected.
int set_destination(int dest) {
	if (!RFID_check_tag()) return -1;
	char data[BLOCK_SIZE];
	data[0] = dest;
	RFID_write_data_block((int)data, DESTINATION);
	return 0;
}
