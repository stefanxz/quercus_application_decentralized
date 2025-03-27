#pragma once
#include "../libc_builtin.h"
#include "../quercus_lib_pico.h"
#include <stdbool.h>

const int BLOCK_SIZE = 4;

enum DataOnRFID {
	TUB_OR_PLANE = 0,
	PLANE_OR_DROPOFF = 1,
	PLANE_ID = 2,
	PAYLOAD = 3,
	DEPARTURE_TIME = 4,
	TUB_ID = 5,
	NEEDS_SECURITY = 6,
	PASSED_SECURITY = 7,
	PLANE_ARRIVED = 8,
	DESTINATION = 9,
	PLANE_DIRECTION = 10, // 0 = outgoing, 1 = incoming
	RFID_LENGTH = 11
};

// Retrieve data from the rfid based on the provided type, returns the first byte of the requested data block.
int get_rfid_data(int type);

// Retrieve all data from the rfid, used for when a tub or plane enters the system.
int get_entrance_rfid_data(char* rfid_data);

// Updates the ID of the Tub.
int set_tub_id(int tub_id);

// Updates whether a Tub needs to go to security.
int set_security_flag(int flag);

// Updates whether a Tub has already passed through security.
int set_security_passed(int flag);

// Updates whether the Plane the Tub is assigned to has already arrived.
int set_plane_arrived();

// Updates the id of the Hardware Module that the Tub has currently set as its destination.
int set_destination(int dest);
