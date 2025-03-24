#include "movement.h"
#include "network.c"
#include "../algo_functions.c"
#include <stdbool.h>

void reset_module() {
	led_set_color(LED_RED);
	belt_small_set_speed(BELT_OFF);
	belt_big_set_speed(BELT_OFF);
	servo_angle_set(ARM_NEUTRAL);
}

void move_within_module(int start, int dest, int tub_id) {
	if (start == dest) { return; }
	led_set_color(LED_GREEN);

	if (start == RFID) {
		belt_small_set_speed(BELT_UP_SLOW);
		if (dest == LASER_LEFT) {
			servo_angle_set(ARM_RIGHT);
			belt_big_set_speed(BELT_LEFT_SLOW);
		} else {
			servo_angle_set(ARM_LEFT);
			belt_big_set_speed(BELT_RIGHT_SLOW);
		}
	} else if (start == LASER_LEFT) {
		belt_big_set_speed(BELT_RIGHT_SLOW);
		if (dest == RFID) {
			servo_angle_set(ARM_RIGHT);
			belt_small_set_speed(BELT_DOWN_SLOW);
		}
	} else {
		belt_big_set_speed(BELT_LEFT_SLOW);
		if (dest == RFID) {
			servo_angle_set(ARM_LEFT);
			belt_small_set_speed(BELT_DOWN_SLOW);
		}
	}

	while (1) {
		if (dest == RFID && RFID_check_tag()) {
			break;
		} else if (dest == LASER_LEFT && !laser_left_detect()) {
			break;
		} else if (dest == LASER_RIGHT && !laser_right_detect()) {
			break;
		}
		sleep(10);
	}

	reset_module();

	if (tub_id >= 0) { send_updated_tub_location(tub_id, dest); }
}

int move_to_neighbour(int module_id, int exit_point, char* tub_data) {
	led_set_color(LED_GREEN);
	if(send_request_movement(module_id, tub_data) < 0) { 
		return -1; 
	}

    char* msg;
    int response;
	int loop_count = 0;
    while (1) {
		if (loop_count > 80) {
			if(send_request_movement(module_id, tub_data) < 0) { 
				return -1; 
			}	
		}

        if (next_event() == EVENT_MESSAGE_RECEIVED) {
            next_message_address(&msg);
			//!!!
			//I realised later that the handle_message cant really be a thing outside of main.c, 
			//so idk if this function still can be used, but depends on how the applciation is coded.
			//You would need to make ur variables actually free up memory instead of only locally in main.c
			//In case you don't its fine, but copy this function then over to ur main.c
			//!!!
			if (msg[1] == REQUEST_RESPONSE) {
				response = 1;
            	//response = handle_message(msg, REQUEST_RESPONSE);
				if (response > 0) {
					if (exit_point == LASER_LEFT) {
						belt_big_set_speed(BELT_LEFT_SLOW);
					} else if (exit_point == LASER_RIGHT) {
						belt_big_set_speed(BELT_RIGHT_SLOW);
					} else {
						belt_small_set_speed(BELT_DOWN_SLOW);
					}
					sleep(5000); //TODO: Test this timing.
					reset_module();
				}
				free(msg);
				return response;
			}
			free(msg);
        }

        sleep(100);
		++loop_count;
    }
}

bool wait_to_enter(int tub, int from, Request req) {
	if(from == RFID) {
		belt_small_set_speed(BELT_UP_SLOW);
	} else if (from == LASER_LEFT) {
		belt_big_set_speed(BELT_RIGHT_SLOW);
	} else {
		belt_big_set_speed(BELT_LEFT_SLOW);
	}

	while (1) {
		if (from == RFID && RFID_check_tag()) {
			break;
		} else if (from == LASER_LEFT && !laser_left_detect()) {
			break;
		} else if (from == LASER_RIGHT && !laser_right_detect()) {
			break;
		}
		sleep(10);
	}

	reset_module();
}