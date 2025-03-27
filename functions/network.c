#pragma once
#include "network.h"
#include "algorithm.h"
#include "rfid.h"

#define PAUSE 250 // in ms

int send_request_movement(int module_id, char* data) {
    
    // printf("the id is: %d\n", get_own_id());
    data[SENDER] = get_own_id();
    // data[MESSAGE_TYPE] = REQUEST_MOVEMENT;
    // for (int i = 0; i < 10; i++) { data[i+2] = tub_data[i]; }
    // printf("net-12 // im sending it\n");
    printf("I am sending data with sender: %d and dest:%d\n", data[SENDER], data[DESTINATION+MSG_HEAD]);
    return send_packet(module_id, data, (RFID_LENGTH+MSG_HEAD));//HARDCODED, WATCHOUT
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

void copy_message(char* dest, char* src) {
    for (int i = 0; i < RFID_LENGTH + MSG_HEAD; i++) {
        dest[i] = src[i];
    }
}

int handle_request_response(char* msg) {
    return  msg[MESSAGE_TYPE];
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

int await_message(char** msg_ptr, int expected_type) {
    int response = NON;
    char type;

    for (int i = 0; i < 10; i++) {
        sleep(PAUSE);

        EventType e = next_event();
        // Sift mailbox for messages
        while (e == EVENT_MESSAGE_RECEIVED) {
            next_message_address(msg_ptr);
            type = (*msg_ptr)[MESSAGE_TYPE];

            printf("Type: %d, Sender:%d, Dest:%d, Tub_id: %d, Tub_plane:%d\n", type, (*msg_ptr)[SENDER], (*msg_ptr)[DESTINATION+MSG_HEAD],  (*msg_ptr)[TUB_ID+MSG_HEAD],  (*msg_ptr)[TUB_OR_PLANE+MSG_HEAD]);
            
            if(type == REQUEST_MOVEMENT) {
                response = 1;
            } else if (type == REQUEST_RESPONSE) {
                response = handle_request_response(*msg_ptr);
            } else if (type == PLANE_STATUS) {
                response = handle_plane_status(*msg_ptr);
            } else if (type == PATHS_CONFIG) {
                response = handle_paths_config(*msg_ptr);
            } else if (type == TUB_CONFIG) {
                response = handle_tub_config(*msg_ptr);
            }

            // If you get the message you need, return it
            if (expected_type == type) {
                // printf("net-113 // expected response got, return\n");
                return response;
            }
            e = next_event(); 
        }
    }
    // printf("Grindset, %d\n", response);
    // This return is never handled, but it is here to prevent a warning
    return response;
}

bool get_response() {
    char* msg;
    // printf("I am waiting for a response.\n");
    int response = await_message(&msg, REQUEST_RESPONSE);
    // printf("You make my head spin right round\n");
    if(response >= 0) {
        free(msg);
        return true;
    } else {
        return false;
    }
}

bool get_request(char* request) {
    char* msg;
    int response = await_message(&msg, REQUEST_MOVEMENT);
    

    if(response >= 0){
        printf("Destination: %d, Sender: %d, Tub id: %d\n", msg[DESTINATION+MSG_HEAD], msg[SENDER], msg[TUB_ID + MSG_HEAD]);
        copy_message(request, msg);
        free(msg);
        return true;
    } else {
        return false;
    }
}