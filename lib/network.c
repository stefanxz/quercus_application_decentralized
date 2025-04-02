#pragma once

#include "network.h"
#include "essentials.h"

/// @brief Send a request for movement to another module.
/// @param module_id id of the module to which the request is sent
/// @param data the request data to be sent
/// @return 0 for success, < 0 for failure
int send_request_movement(int module_id, char* data) {
	data[MSG_SENDER] = get_own_id();
	printf("net // req with sender: %d and dest:%d\n", data[MSG_SENDER], data[REQ_DEST_ID]);
	return send_packet(module_id, data, (REQ_LENGTH)); // HARDCODED, WATCHOUT
}

// TODO: delete this function, it is not used anywhere relevant
int send_updated_tub_location(int tub_id, int location_belt) {
	char data[4];
	data[MSG_SENDER] = get_own_id();
	data[MSG_TYPE] = 17;//FAKE TUB_LOGGING;
	data[2] = tub_id;
	data[3] = location_belt;
	return send_packet(0, data, sizeof(data));
}

/// @brief Send a message that a plane has arrived or departed to the Pi module.
/// @param plane_id the id of the plane
/// @param arrival_status the status of the plane (true for arrival, false for departure)
/// @return 0 for success, < 0 for failure
int send_plane_status(int plane_id, bool arrival_status) {
	char data[4];
	data[MSG_SENDER] = get_own_id();
	data[MSG_TYPE] = PLANE_STATUS;
	data[2] = plane_id;
	data[3] = arrival_status;
	return send_packet(0, data, sizeof(data));
}


// TODO: delete this function, it is not used anywhere relevant
int send_tub_status(char* tub_data) {
	char data[13];
	data[MSG_SENDER] = get_own_id();
	data[MSG_TYPE] = TUB_STATUS;
	for (int i = 0; i < 10; i++) {
		data[i + 2] = tub_data[i];
	}
	return send_packet(0, data, sizeof(data));
}

/// @brief Send a message containing a response to another module's request.
/// @param module_id the id of the module to which the response is sent
/// @param value the value to be sent in the response (0 for failure, 1 for success)
/// @return 0 for success, < 0 for failure
int send_request_response(int module_id, int value) {
	char data[3];
	data[MSG_SENDER] = get_own_id();
	data[MSG_TYPE] = REQUEST_RESPONSE;
	data[2] = value;
	return send_packet(module_id, data, sizeof(data));
}

/// @brief Send a request to receive your path configuration to the Pi module.
/// @return 0 for success, < 0 for failure
int send_request_path_config(){
	char data[2] = {get_own_id(), REQUEST_PATH_CONFIG};
	return send_packet(0, data, sizeof(data));
}
