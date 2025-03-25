#pragma once
#include "network.h"
#include "algorithm.h"

#define TIMEOUT 50

int send_request_movement(int module_id, char* tub_data) {
    char data[13];
    data[SENDER] = get_own_id();
    data[MESSAGE_TYPE] = REQUEST_MOVEMENT;
    for (int i = 0; i < 10; i++) { data[i+2] = tub_data[i]; }
    return send_packet(module_id, data, sizeof(data));
}

int send_updated_tub_location(int tub_id, int location_belt) {
    char data[4];
    data[SENDER] = get_own_id();
    data[MESSAGE_TYPE] = TUB_LOGGING;
    data[2] = tub_id;
    data[3] = location_belt; // LASER_LEFT = 0, LASER_RIGHT = 1, RFID = 2
    return send_packet(0, data, sizeof(data));
}

int send_plane_status(int plane_id, bool arrival_status) {
    char data[4];
    data[SENDER] = get_own_id();
    data[MESSAGE_TYPE] = PLANE_STATUS;
    data[2] = plane_id;
    data[3] = arrival_status;
    return send_packet(0, data, sizeof(data));
}

int send_tub_status(char* tub_data) {
    char data[13];
    data[SENDER] = get_own_id();
    data[MESSAGE_TYPE] = TUB_STATUS;
    for (int i = 0; i < 10; i++) { data[i+2] = tub_data[i]; }
    return send_packet(0, data, sizeof(data));
}

int send_request_response(int module_id, int value) {
    char data[3];
    data[SENDER] = get_own_id();
    data[MESSAGE_TYPE] = REQUEST_RESPONSE;
    data[2] = value;
    return send_packet(module_id, data, sizeof(data));
}

void get_request(Request* request) {
    char* msg;
    char* data = await_message(&msg, REQUEST_MOVEMENT);
    // TODO: Put data in request fields
    decode_request(&request, data);
    free(msg);
}

char* encode_request(Request* request) {
    char* data[9];
    data[SENDER] = request->sender_id;
    data[MESSAGE_TYPE] = REQUEST_MOVEMENT;
    data[2] = request->plane_or_drop_off;
    data[3] = request->plane_id;
    data[4] = request->payload;
    data[5] = request->departure_time;
    data[6] = request->tub_id;
    data[7] = request->security_status;
    data[8] = request->plane_arrived;
    data[9] = request->destination;
    return data;
}

void decode_request(Request* request, char * data) {
    request->sender_id = data[SENDER];
    request->plane_or_drop_off = data[2];
    request->plane_id = data[3];
    request->payload = data[4];
    request->departure_time = data[5];
    request->tub_id = data[6];
    request->security_status = data[7];
    request->plane_arrived = data[8];
    request->destination = data[9];
    return request;
}

int get_response() {
    char* msg;
    bool response = await_message(&msg, REQUEST_RESPONSE);
    free(msg);
    return response;
}

int handle_request_response(char* msg) {
    return  msg[2];
}

int handle_plane_status(char* msg) {
    //TODO: Handle message
}

int handle_paths_config(char* msg) {
    //TODO: Handle message
}

int handle_tub_config(char* msg) {
    return set_tub_id(msg[2]);
}

int await_message(char* msg, int expected_type) {
    int response;
    int type;

    while (next_event() == EVENT_MESSAGE_RECEIVED) {
        printf("Received message\n");
        next_message_address(&msg);
        type = msg[1];

        if (type == REQUEST_MOVEMENT) {
            response = handle_request_movement(msg);
        } else if (type == REQUEST_RESPONSE) {
            response = handle_request_response(msg);
        } else if (type == PLANE_STATUS) {
            response = handle_plane_status(msg);
        } else if (type == PATHS_CONFIG) {
            response = handle_paths_config(msg);
        } else if (type == TUB_CONFIG) {
            response = handle_tub_config(msg);
        }

        if (expected_type == type) {
            printf("Expected message, returning response\n");
            return response;
        } 
    }

    // This return is never handled, but it is here to prevent a warning
    return -1;
}