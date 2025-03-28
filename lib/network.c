#pragma once

#include "network.h"
#include "essentials.h"

#define PAUSE 250 // in ms

int send_request_movement(int module_id, char* data) {
	data[MSG_SENDER] = get_own_id();
	printf("net // req with sender: %d and dest:%d\n", data[MSG_SENDER], data[REQ_DEST_ID]);
	return send_packet(module_id, data, (REQ_LENGTH)); // HARDCODED, WATCHOUT
}

int send_updated_tub_location(int tub_id, int location_belt) {
	char data[4];
	data[MSG_SENDER] = get_own_id();
	data[MSG_TYPE] = 17;//FAKE TUB_LOGGING;
	data[2] = tub_id;
	data[3] = location_belt;
	return send_packet(0, data, sizeof(data));
}

int send_plane_status(int plane_id, bool arrival_status) {
	char data[4];
	data[MSG_SENDER] = get_own_id();
	data[MSG_TYPE] = PLANE_STATUS;
	data[2] = plane_id;
	data[3] = arrival_status;
	return send_packet(0, data, sizeof(data));
}

int send_tub_status(char* tub_data) {
	char data[13];
	data[MSG_SENDER] = get_own_id();
	data[MSG_TYPE] = TUB_STATUS;
	for (int i = 0; i < 10; i++) {
		data[i + 2] = tub_data[i];
	}
	return send_packet(0, data, sizeof(data));
}

int send_request_response(int module_id, int value) {
	char data[3];
	data[MSG_SENDER] = get_own_id();
	data[MSG_TYPE] = REQUEST_RESPONSE;
	data[2] = value;
	return send_packet(module_id, data, sizeof(data));
}