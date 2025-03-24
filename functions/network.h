#pragma once

#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

enum MESSAGE_TYPE {
    NONE = 0,
    REQUEST_MOVEMENT = 1,
    REQUEST_RESPONSE = 2,
    PLANE_STATUS = 3,
    TUB_STATUS = 4,
    PATHS_CONFIG = 5,
    TUB_LOGGING = 6,
    TUB_CONFIG = 7
};

enum MESSAGE_BLOCK {
    SENDER = 0,
    MESSAGE_TYPE = 1
};

// Sends a message to request movement to a module with 'module_id' and with the specific 'tub_data'.
int send_request_movement(int module_id, char* tub_data);

// Sends a message to the Pi to update the location ('location_belt') of a tub with 'tub_id'.
int send_updated_tub_location(int tub_id, int location_belt);

// Sends a message to the Pi that a plane with 'plane_id' has arrived or left.
int send_plane_status(int plane_id);

// Sends a message to the Pi that a tub with 'tub_id' has arrived or left.
int send_tub_status(char* tub_data);