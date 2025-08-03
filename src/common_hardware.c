#include "defs.h"

#include "libc_builtin.h"
#include "quercus_lib_pico.h"


/// @brief Reset the module to its default state: LED red, belts off, arm neutral.
void reset_module() {
	led_set_color(COLOR_RED);
	belt_small_set_speed(BELT_OFF);
	belt_big_set_speed(BELT_OFF);

	// Wait a little before resetting the arm
	float curr_pos = servo_angle_get();
	for (int i = 0; i < 10; ++i) {
		servo_angle_set(curr_pos -= curr_pos / 10);
		sleep(25);
	}
	servo_angle_set(ARM_NEUTRAL);
}

/// @brief Reads a single byte of DIR_RFID data from the specified data block type.
/// @param type The data block index/type to read from.
/// @return The first byte of the specified DIR_RFID data block, or -1 if no tag is detected.
int get_rfid_data(int type) {
	if (!RFID_check_tag()) return -1;

	char data[RFID_BLOCK_SIZE];
	RFID_read_data_block((int)data, type);
	return data[0];
}

/// @brief Reads multiple DIR_RFID data blocks (up to DATA_RFID_LENGTH) and stores the first byte of each into rfid_data.
/// @param rfid_data A pointer to a buffer that will store the read bytes.
/// @return 0 on success, or -1 if no tag is detected.
int get_entrance_rfid_data(char* rfid_data) {
	if (!RFID_check_tag()) return -1;
	char block_data[RFID_BLOCK_SIZE];
	for (int i = 0; i < DATA_RFID_LENGTH; ++i) {
		RFID_read_data_block((int)block_data, i);
		rfid_data[i] = block_data[0];
	}
	return 0;
}

/// @brief Writes a tub ID to the DIR_RFID tag.
/// @param tub_id The tub ID to be written.
/// @return 0 on success, or -1 if no tag is detected.
int set_tub_id(int tub_id) {
	if (!RFID_check_tag()) return -1;
	char data[RFID_BLOCK_SIZE];
	data[0] = tub_id;
	RFID_write_data_block((int)data, DATA_TUB_ID);
	return 0;
}

/// @brief Writes a security flag to the DIR_RFID tag.
/// @param flag The security flag to be written.
/// @return 0 on success, or -1 if no tag is detected.
int set_security_flag(int flag) {
	if (!RFID_check_tag()) return -1;
	char data[RFID_BLOCK_SIZE];
	data[0] = flag;
	RFID_write_data_block((int)data, DATA_TUB_ID);
	return 0;
}

/// @brief Sets the 'security passed' flag if the DIR_RFID tag indicates security is needed.
/// @param flag The value to set for the security-passed indicator.
/// @return 0 on success, -1 if no tag is detected, or -2 if the 'DATA_NEEDS_SECURITY' block indicates no security requirement.
int set_security_passed(int flag) {
	if (!RFID_check_tag()) return -1;
	if (!get_rfid_data(DATA_NEEDS_SECURITY)) return -2;
	// TODO: Add check whether module is security
	char data[RFID_BLOCK_SIZE];
	data[0] = flag;
	RFID_write_data_block((int)data, DATA_NEEDS_SECURITY);
	return 0;
}

/// @brief Marks that a plane has arrived if the DIR_RFID tag indicates a plane or drop-off scenario.
/// @return 0 on success, -1 if no tag is detected, or -2 if 'DATA_PLANE_OR_DROPOFF' block is not set.
int set_plane_arrived() {
	if (!RFID_check_tag()) return -1;
	if (!get_rfid_data(DATA_PLANE_OR_DROPOFF)) return -2;
	char data[RFID_BLOCK_SIZE];
	data[0] = 0x1;
	RFID_write_data_block((int)data, DATA_PLANE_ARRIVED);
	return 0;
}

/// @brief Writes a destination value to the DIR_RFID tag.
/// @param dest The destination code to be written.
/// @return 0 on success, or -1 if no tag is detected.
int set_destination(int dest) {
	if (!RFID_check_tag()) return -1;
	char data[RFID_BLOCK_SIZE];
	data[0] = dest;
	RFID_write_data_block((int)data, DATA_DESTINATION);
	return 0;
}


// wiggles the arm to free the tub
void the_wiggler() {
	// get the current angle of the servo to wiggle around it
    float angle = servo_angle_get();
    for (int i = 0; i < 31; ++i) {
		// wiggle the servo by 5 degrees to the left
        servo_angle_set(angle + 5);
        sleep(50);
		// wiggle the servo by 5 degrees to the right
        servo_angle_set(angle - 5);
        sleep(50);
    }
	// reset the servo to its original position
    servo_angle_set(angle);
}

void wiggle() {

    float angle = servo_angle_get();
    servo_angle_set(angle + 5);
    sleep(50);
    servo_angle_set(angle - 5);
    sleep(50);

    servo_angle_set(angle);
}



/// @brief Move the tub within the module from one endpoint to another.
/// @param tub_id The ID of the tub that is moving.
/// @param from The starting endpoint (DIR_LASER_LEFT, DIR_LASER_RIGHT, DIR_RFID).
/// @param to The destination endpoint (DIR_LASER_LEFT, DIR_LASER_RIGHT, DIR_RFID).
void move_within_module(int from, int to) {
	if (from == to) return;
	led_set_color(COLOR_GREEN);

	if (from == DIR_RFID) {
		belt_small_set_speed(BELT_UP_SLOW);
		if (to == DIR_LASER_LEFT) {
			servo_angle_set(ARM_RIGHT);
			belt_big_set_speed(BELT_LEFT_SLOW);
		} else {
			servo_angle_set(ARM_LEFT);
			belt_big_set_speed(BELT_RIGHT_SLOW);
		}
	} else if (from == DIR_LASER_LEFT) {
		belt_big_set_speed(BELT_RIGHT_SLOW);
		if (to == DIR_RFID) {
			servo_angle_set(ARM_RIGHT);
			belt_small_set_speed(BELT_DOWN_SLOW);
		}
	} else {
		belt_big_set_speed(BELT_LEFT_SLOW);
		if (to == DIR_RFID) {
			servo_angle_set(ARM_LEFT);
			belt_small_set_speed(BELT_DOWN_SLOW);
		}
	}

	// TODO: Implement timeout?
    int start_wait_time = get_uptime(); // uptime in ms
    float servo_angle = servo_angle_get();
	for (int i = 0; true; i++) {
		if (to == DIR_RFID && RFID_check_tag()) {
			break;
		} else if (to == DIR_LASER_LEFT && !laser_left_detect()) {
			break;
		} else if (to == DIR_LASER_RIGHT && !laser_right_detect()) {
			break;
		}
        int curr_wait_time = get_uptime();
        if (curr_wait_time - start_wait_time > 4000) { // 4 seconds
            if (i % 10 == 0) {
                servo_angle_set(servo_angle + 5);
            } else if (i % 10 == 5) {
                servo_angle_set(servo_angle - 5);
            }
        }
        sleep(10);
	}
    servo_angle_set(servo_angle);
	sleep(20);
	reset_module();

}

/// @brief Push a tub out of the module at a specific exit point.
/// @param exit the exit point from which the tub leaves (DIR_LASER_LEFT, DIR_LASER_RIGHT, DIR_RFID).
void leave_at(int exit) {
	led_set_color(COLOR_GREEN);

	if (exit == DIR_LASER_LEFT) {
		belt_big_set_speed(BELT_LEFT_SLOW);
	} else if (exit == DIR_LASER_RIGHT) {
		belt_big_set_speed(BELT_RIGHT_SLOW);
	} else {
		belt_small_set_speed(BELT_DOWN_SLOW);
	}

	// Wait for tub to leave:
	sleep(1500);
	reset_module();
}


/// @brief Take on a tub at a specific entrance point.
/// @param entrance the entrance point where the tub enters (DIR_LASER_LEFT, DIR_LASER_RIGHT, DIR_RFID).
void enter_at(int entrance) {
	if (entrance == DIR_RFID) {
		belt_small_set_speed(BELT_UP_SLOW);
	} else if (entrance == DIR_LASER_LEFT) {
		belt_big_set_speed(BELT_RIGHT_SLOW);
	} else {
		belt_big_set_speed(BELT_LEFT_SLOW);
	}

	// TODO: Implement timeout
	while (1) {
		if (entrance == DIR_RFID && RFID_check_tag()) {
			break;
		} else if (entrance == DIR_LASER_LEFT && !laser_left_detect()) {
			break;
		} else if (entrance == DIR_LASER_RIGHT && !laser_right_detect()) {
			break;
		}
		sleep(10);
	}
	reset_module();
}



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
	data[MSG_TYPE] = MSG_PLANE_STATUS;
	data[2] = plane_id;
	data[3] = arrival_status;
	return send_packet(0, data, sizeof(data));
}


// TODO: delete this function, it is not used anywhere relevant
int send_tub_status(char* tub_data) {
	char data[13];
	data[MSG_SENDER] = get_own_id();
	data[MSG_TYPE] = MSG_TUB_STATUS;
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
	data[MSG_TYPE] = MSG_REQUEST_RESPONSE;
	data[MSG_VALUE] = value;
	return send_packet(module_id, data, sizeof(data));
}

/// @brief Send a request to receive your path configuration to the Pi module.
/// @return 0 for success, < 0 for failure
int send_request_path_config(){
	char data[2] = {get_own_id(), MSG_REQUEST_PATH_CONFIG};
	return send_packet(0, data, sizeof(data));
}
