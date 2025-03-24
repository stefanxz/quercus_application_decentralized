#pragma once
#include "network.h"

int send_request_movement(int module_id, char* tub_data) {
    char data[12];
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

int send_plane_status(int plane_id) {
    char data[3];
    data[SENDER] = get_own_id();
    data[MESSAGE_TYPE] = PLANE_STATUS;
    data[2] = plane_id;
    return send_packet(0, data, sizeof(data));
}

int send_tub_status(char* tub_data) {
    char data[12];
    data[SENDER] = get_own_id();
    data[MESSAGE_TYPE] = TUB_STATUS;
    for (int i = 0; i < 10; i++) { data[i+2] = tub_data[i]; }
    return send_packet(0, data, sizeof(data));
}