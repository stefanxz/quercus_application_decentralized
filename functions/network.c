#pragma once
#include "network.h"
#include "algorithm.h"
#include "rfid.h"

#define TIMEOUT 50

int send_request_movement(int module_id, char* tub_data) {
    char data[13];
    data[SENDER] = get_own_id();
    data[MESSAGE_TYPE] = REQUEST_MOVEMENT;
    for (int i = 0; i < 10; i++) { data[i+2] = tub_data[i]; }
    printf("net-12 // im sending it\n");
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

char* encode_request(Request* request) {
    char data[END_OF_ENUM];
    data[PLANE_DROPOFF] = request->plane_or_drop_off;
    data[PLANE_ID] = request->plane_id;
    data[PAYLOAD] = request->payload;
    data[DEPARTURE_TIME] = request->departure_time;
    data[TUB_ID] = request->tub_id;
    data[SECURITY] = request->security_status;
    data[PLANE_ARRIVED] = request->plane_arrived;
    data[DESTINATION] = request->destination;
    return &data;
}

void decode_request(Request* request, char* data) {
    request->sender_id = data[SENDER];
    request->plane_or_drop_off = data[PLANE_DROPOFF];
    request->plane_id = data[PLANE_ID];
    request->payload = data[PAYLOAD];
    request->departure_time = data[DEPARTURE_TIME];
    request->tub_id = data[TUB_ID];
    request->security_status = data[SECURITY];
    request->plane_arrived = data[PLANE_ARRIVED];
    request->destination = data[DESTINATION];
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
    // return set_tub_id(msg[2]);
    return 0;
}

int await_message(char* msg, int expected_type) {
    int response = -1;
    int type;
    EventType e = next_event();
    while (e == EVENT_MESSAGE_RECEIVED) {
        printf("e: %d\n", e);
        printf("net-98 // i'm jaking it.\n");
        next_message_address(&msg);
        type = msg[1];

        if(type == REQUEST_MOVEMENT) {
            response = 1;
        }
        else if (type == REQUEST_RESPONSE) {
            response = handle_request_response(msg);
        } else if (type == PLANE_STATUS) {
            response = handle_plane_status(msg);
        } else if (type == PATHS_CONFIG) {
            response = handle_paths_config(msg);
        } else if (type == TUB_CONFIG) {
            response = handle_tub_config(msg);
        }

        if (expected_type == type) {
            printf("net-113 // expected response got, return\n");
            return response;
        }
        e = next_event(); 
    }

    // This return is never handled, but it is here to prevent a warning
    return -1;
}

bool get_response() {
    char* msg;
    int response = await_message(&msg, REQUEST_RESPONSE);
    free(msg);
    return (response == NON ? false : true);
}

bool get_request(Request* request) {
    char* msg;
    char* data;

    int response = await_message(&msg, REQUEST_MOVEMENT);
    decode_request(request, msg);
    free(msg);
    return (response == NON ? false : true);
}