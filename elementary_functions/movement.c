#pragma once

#include "movement.h"
#include "network.c"

/// @brief Reset the module to its default state: LED red, belts off, arm neutral.
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
/// @param from The starting endpoint (LASER_LEFT, LASER_RIGHT, RFID).
/// @param to The destination endpoint (LASER_LEFT, LASER_RIGHT, RFID).
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

	// TODO: Implement timeout?
    int start_wait_time = get_uptime(); // uptime in ms
    float servo_angle = servo_angle_get();
	for (int i = 0; true; i++) {
		if (to == RFID && RFID_check_tag()) {
			break;
		} else if (to == LASER_LEFT && !laser_left_detect()) {
			break;
		} else if (to == LASER_RIGHT && !laser_right_detect()) {
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
/// @param exit the exit point from which the tub leaves (LASER_LEFT, LASER_RIGHT, RFID).
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


/// @brief Take on a tub at a specific entrance point.
/// @param entrance the entrance point where the tub enters (LASER_LEFT, LASER_RIGHT, RFID).
void enter_at(int entrance) {
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
}
