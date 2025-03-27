#pragma once

#include <stdbool.h>
#include "../libc_builtin.h"
#include "../quercus_lib_pico.h"

enum MessageTypes {
	NONE = 0,
	REQUEST_MOVEMENT = 1,
	REQUEST_RESPONSE = 2,
	PLANE_STATUS = 3,
	TUB_STATUS = 4,
	PATHS_CONFIG = 5,
	REQUEST_PATH_CONFIG = 6,
	TUB_CONFIG = 7
};

enum MessageHead {
	MSG_SENDER = 0,
	MSG_TYPE = 1,
	MSG_VALUE = 2 // Used for request responses
};

enum RequestsContent {
	REQ_TUB_ID = 2,
	REQ_DEST_TYPE = 3,
	REQ_DEST_ID = 4,
	REQ_PLANE_ARRIVED = 5,
	REQ_PLANE_ID = 6,
	REQ_SECURITY = 7,
	REQ_PAYLOAD = 8,
	REQ_LENGTH = 9
};

enum ArrivalsContent {
	ARR_PLANE_ID = 2,
	ARR_MODULE_ID = 3,
	ARR_DIRECTION = 4,
	ARR_LENGTH = 5
};

enum LayoutContent {
	CON_LASER_LEFT = 2,
	CON_LASER_RIGHT = 3,
	CON_RFID = 4,
	CON_STORAGE = 5,
	CON_LOOKUP = 6,
	CON_LENGTH = 7
};

// Sends a message to request movement to a module with 'module_id' and with the specific 'tub_data'.
int send_request_movement(int module_id, char* tub_data);

// Sends a message to the Pi to update the location ('location_belt') of a tub with 'tub_id'.
int send_updated_tub_location(int tub_id, int location_belt);

// Sends a message to the Pi that a plane with 'plane_id' has arrived or left.
int send_plane_status(int plane_id, bool arrival_status);

// Sends a message to the Pi that a tub with 'tub_id' has arrived or left.
int send_tub_status(char* tub_data);