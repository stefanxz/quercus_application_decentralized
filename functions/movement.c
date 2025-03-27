#pragma once

#include <stdbool.h>
#include "algorithm.h"

#include "network.c"

const int ARM_LEFT = 60;
const int ARM_RIGHT = 105;
const int ARM_NEUTRAL = 0;

const int LED_RED = 0xff0000;
const int LED_GREEN = 0x00ff00;
const int LED_BLUE = 0x0000ff;

const int BELT_OFF = 0;
const int BELT_LEFT_SLOW = 20;
const int BELT_RIGHT_SLOW = -20;
const int BELT_DOWN_SLOW = 20;
const int BELT_UP_SLOW = -20;

void reset_module() {
	led_set_color(LED_RED);
	belt_small_set_speed(BELT_OFF);
	belt_big_set_speed(BELT_OFF);

	// Wait a little before resetting the arm
	float curr_pos = servo_angle_get();
	for(int i =0; i < 10; ++i){
		servo_angle_set(curr_pos -= curr_pos/10);	
		sleep(25);
	}
	servo_angle_set(ARM_NEUTRAL);
}

void move_within_module(int tub_id, int start, int dest) {
	if (start == dest) return;
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

	// TODO: Implement timeout
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
	sleep(20);
	reset_module();

	if (tub_id >= 0) { 
		send_updated_tub_location(tub_id, dest); 
	}
}

int leave_at(int module_id, int exit_point, char* tub_data) {
	led_set_color(LED_GREEN);
    int response;

	// Send until success:
	for(int i = 0; i < 10, send_request_movement(module_id, tub_data) < 0; i++) {
		printf("move // packet loss\n");
		sleep(100);
	}

	// Wait for response to arrive and acquire it:
	response = get_response();

	if (response > 0) {
		printf("move // response got %d \n", response);
		if (exit_point == LASER_LEFT) {
			belt_big_set_speed(BELT_LEFT_SLOW);
		} else if (exit_point == LASER_RIGHT) {
			belt_big_set_speed(BELT_RIGHT_SLOW);
		} else {
			belt_small_set_speed(BELT_DOWN_SLOW);
		}
		// Wait for tub to leave:
		sleep(2000);

		reset_module();
		return response;
	} else {
		return NON;
	}
}

bool enter_at(int from) {
	if(from == RFID) {
		belt_small_set_speed(BELT_UP_SLOW);
	} else if (from == LASER_LEFT) {
		belt_big_set_speed(BELT_RIGHT_SLOW);
	} else {
		belt_big_set_speed(BELT_LEFT_SLOW);
	}

	for (int i = 0; i < 1000; i++) {
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
	return true;
}