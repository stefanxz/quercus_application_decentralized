#pragma once

#include "movement.h"
#include "network.c"

void reset_module() {
	led_set_color(LED_RED);
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

void move_within_module(int tub_id, int from, int to) {
	if (from == to) return;
	led_set_color(LED_GREEN);

	if (from == RFID) {
		belt_small_set_speed(BELT_UP_SLOW);
		if (to == LASER_LEFT) {
			servo_angle_set(ARM_RIGHT);
			belt_big_set_speed(BELT_LEFT_SLOW);
		} else {
			servo_angle_set(ARM_LEFT);
			belt_big_set_speed(BELT_RIGHT_SLOW);
		}
	} else if (from == LASER_LEFT) {
		belt_big_set_speed(BELT_RIGHT_SLOW);
		if (to == RFID) {
			servo_angle_set(ARM_RIGHT);
			belt_small_set_speed(BELT_DOWN_SLOW);
		}
	} else {
		belt_big_set_speed(BELT_LEFT_SLOW);
		if (to == RFID) {
			servo_angle_set(ARM_LEFT);
			belt_small_set_speed(BELT_DOWN_SLOW);
		}
	}

	// TODO: Implement timeout
	while (1) {
		if (to == RFID && RFID_check_tag()) {
			break;
		} else if (to == LASER_LEFT && !laser_left_detect()) {
			break;
		} else if (to == LASER_RIGHT && !laser_right_detect()) {
			break;
		}
		sleep(10);
	}
	sleep(20);
	reset_module();

	if (tub_id >= 0) {
		send_updated_tub_location(tub_id, to);
	}
}

void leave_at(int exit) {
	led_set_color(LED_GREEN);

	if (exit == LASER_LEFT) {
		belt_big_set_speed(BELT_LEFT_SLOW);
	} else if (exit == LASER_RIGHT) {
		belt_big_set_speed(BELT_RIGHT_SLOW);
	} else {
		belt_small_set_speed(BELT_DOWN_SLOW);
	}

	// Wait for tub to leave:
	sleep(1500);
	reset_module();
}

bool enter_at(int entrance) {
	if (entrance == RFID) {
		belt_small_set_speed(BELT_UP_SLOW);
	} else if (entrance == LASER_LEFT) {
		belt_big_set_speed(BELT_RIGHT_SLOW);
	} else {
		belt_big_set_speed(BELT_LEFT_SLOW);
	}

	// TODO: Implement timeout
	while (1) {
		if (entrance == RFID && RFID_check_tag()) {
			break;
		} else if (entrance == LASER_LEFT && !laser_left_detect()) {
			break;
		} else if (entrance == LASER_RIGHT && !laser_right_detect()) {
			break;
		}
		sleep(10);
	}
	reset_module();
	return true;
}