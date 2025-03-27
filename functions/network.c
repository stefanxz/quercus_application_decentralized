#pragma once
#include "network.h"
#include "algorithm.h"
#include "rfid.h"

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

void copy_message(char* request, char* msg) {
	for (int i = 0; i < REQ_LENGTH; i++) {
		request[i] = msg[i];
	}
}

int handle_request_response(char* msg) { return msg[MSG_VALUE]; }

int handle_plane_status(Module* module, char* msg) {
	if (msg[MSG_SENDER] != 0) {
		printf("net // what the pico doing??");
		return NON;
	}
	if (msg[ARR_DIRECTION] == 1) {
		module->plane_to_id[msg[ARR_PLANE_ID]] = msg[ARR_MODULE_ID];
	} else {
		module->plane_to_id[msg[ARR_PLANE_ID]] = NON;
	}
	return 0;
}

int handle_paths_config(Module* module, char* msg) {
	module->next[LASER_LEFT] = msg[CON_LASER_LEFT];
	module->next[LASER_RIGHT] = msg[CON_LASER_RIGHT];
	module->next[RFID] = msg[CON_RFID];
	return 0;
}

int handle_tub_config(char* msg) {
	// return set_tub_id(msg[2]);
	return 0;
}

int await_message(Module* module, char** msg_ptr, int expected, bool persistent) {
	int response = NON;
	char type;

	for (int i = 0; i < 10 || persistent; i++) {
		//printf("im jaking it\n");
		sleep(PAUSE);

		EventType e = next_event();
		// Sift mailbox for messages:
		while (e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(msg_ptr);
			type = (*msg_ptr)[MSG_TYPE];

			printf("Type: %d, Sender:%d, Dest:%d, Tub_id: %d, Tub_plane:%d\n", type, (*msg_ptr)[MSG_SENDER],
				   (*msg_ptr)[REQ_DEST_ID], (*msg_ptr)[REQ_TUB_ID]);

			if (type == REQUEST_MOVEMENT) {
				// complicated decision as to whether to accept or reject the request here i guess
				response = 1;
			} else if (type == REQUEST_RESPONSE) {
				response = handle_request_response(*msg_ptr);
			} else if (type == PLANE_STATUS) {
				response = handle_plane_status(module, *msg_ptr);
			} else if (type == PATHS_CONFIG) {
				response = handle_paths_config(module, *msg_ptr);
			} else if (type == TUB_CONFIG) {
				response = handle_tub_config(*msg_ptr);
			}

			// If you get the message you need, return it
			if (expected == type) {
				printf("net-113 // expected response got, return\n");
				return response;
			}
			e = next_event();
		}
	}
	// printf("Grindset, %d\n", response);
	// This return is never handled, but it is here to prevent a warning
	return response;
}

bool await_response(Module* module) {
	char* msg;
	int response = await_message(module, &msg, REQUEST_RESPONSE, true);

	if (response >= 0) {
		free(msg);
		return true;
	} else {
		return false;
	}
}

bool await_request(Module* module, char* request) {
	char* msg;
	int response = await_message(module, &msg, REQUEST_MOVEMENT, false);

	if (response >= 0) {
		printf("Destination: %d, Sender: %d, Tub id: %d\n", msg[REQ_DEST_TYPE], msg[MSG_SENDER], msg[REQ_TUB_ID]);
		copy_message(request, msg);
		free(msg);
		return true;
	} else {
		return false;
	}
}