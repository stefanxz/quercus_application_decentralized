#pragma once

#include <stdbool.h>

#define PAUSE 250

// Enum to represent the type of message being sent or received.
// Each value corresponds to a specific type of message.
enum MessageTypes {
	NONE = 0,
	REQUEST_MOVEMENT = 1, // Request to move a tub to the module
	REQUEST_RESPONSE = 2, // Response to a request for movement
	PLANE_STATUS = 3, // Status update for a plane
	TUB_STATUS = 4, // Status update for a tub
	PATH_CONFIG = 5, // Configuration of the paths
	REQUEST_PATH_CONFIG = 6, // Request for path configuration
	TUB_CONFIG = 7 // Configuration of a tub
};

// Enum to represent the content that every message should have.
// Each value corresponds to a specific piece of information in the message.
enum MessageHead {
	MSG_SENDER = 0, // Sender of the message
	MSG_TYPE = 1, // Type of the message
	MSG_VALUE = 2 // Used for request responses
};

// Enum to represent the content of a request message.
// Each value corresponds to a specific piece of information in the request.
enum RequestsContent {
    REQ_TUB_ID = 2,       // Identifier for the tub being referenced in the request.
    REQ_DEST_TYPE = 3,    // Type of destination (e.g., module, plane, etc.).
    REQ_DEST_ID = 4,      // Identifier for the destination.
    REQ_PLANE_ARRIVED = 5,// Indicates whether a plane has arrived.
    REQ_PLANE_ID = 6,     // Identifier for the plane.
    REQ_SECURITY = 7,     // Security-related information for the request.
    REQ_PAYLOAD = 8,      // Payload data associated with the request.
    REQ_LENGTH = 9        // Total length of the request message.
};

// Enum to represent the content of an arrivals message.
// Each value corresponds to a specific piece of information in the arrivals data.
enum ArrivalsContent {
    ARR_PLANE_ID = 2,     // Identifier for the plane in the arrivals message.
    ARR_MODULE_ID = 3,    // Identifier for the module associated with the arrival.
    ARR_DEP_TIME = 4,     // Departure time for the plane or module.
    ARR_LENGTH = 5        // Total length of the arrivals message.
};

// Sends a message to request movement to a module with 'module_id' and with the specific 'tub_data'.
int send_request_movement(int module_id, char* tub_data);

// Sends a message to the Pi to update the location ('location_belt') of a tub with 'tub_id'.
int send_updated_tub_location(int tub_id, int location_belt);

// Sends a message to the Pi that a plane with 'plane_id' has arrived or left.
int send_plane_status(int plane_id, bool arrival_status);

// Sends a message to the Pi that a tub with 'tub_id' has arrived or left.
int send_tub_status(char* tub_data);