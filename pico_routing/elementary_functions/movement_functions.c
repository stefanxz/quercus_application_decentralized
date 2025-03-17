#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"
#include <stdbool.h>
#include "movement_functions.h"

void laser_right_to_rfid() {
	led_set_color(LED_GREEN);
	servo_angle_set(ARM_LEFT);
	belt_big_set_speed(BELT_LEFT_SLOW);
	belt_small_set_speed(BELT_DOWN_SLOW);
	subscribe_to_event(EVENT_RFID_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_RFID_DETECT) {
			servo_angle_set(ARM_NEUTRAL);
			belt_big_set_speed(BELT_OFF);
			belt_small_set_speed(BELT_OFF);
			led_set_color(LED_BLUE);
			break;
		}
		sleep(100);
	}
	unsubscribe_from_event(EVENT_RFID_DETECT);
}

void laser_left_to_rfid() {
	led_set_color(LED_GREEN);
	servo_angle_set(ARM_RIGHT);
	belt_big_set_speed(BELT_RIGHT_SLOW);
	belt_small_set_speed(BELT_DOWN_SLOW);
	subscribe_to_event(EVENT_RFID_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_RFID_DETECT) {
			servo_angle_set(ARM_NEUTRAL);
			belt_big_set_speed(BELT_OFF);
			belt_small_set_speed(BELT_OFF);
			led_set_color(LED_BLUE);
			break;
		}
		sleep(100);
	}
	unsubscribe_from_event(EVENT_RFID_DETECT);
}

void laser_right_to_laser_left() {
	led_set_color(LED_GREEN);
	servo_angle_set(ARM_NEUTRAL);
	belt_big_set_speed(BELT_LEFT_SLOW);
	subscribe_to_event(EVENT_LASER_LEFT_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_LASER_LEFT_DETECT) {
			servo_angle_set(ARM_NEUTRAL);
			belt_big_set_speed(BELT_OFF);
			led_set_color(LED_BLUE);
			break;
		}
		sleep(100);
	}
	unsubscribe_from_event(EVENT_LASER_LEFT_DETECT);
}

void laser_left_to_laser_right() {
	led_set_color(LED_GREEN);
	servo_angle_set(ARM_NEUTRAL);
	belt_big_set_speed(BELT_RIGHT_SLOW);
	subscribe_to_event(EVENT_LASER_RIGHT_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_LASER_RIGHT_DETECT) {
			servo_angle_set(ARM_NEUTRAL);
			belt_big_set_speed(BELT_OFF);
			belt_small_set_speed(BELT_OFF);
			led_set_color(LED_BLUE);
			break;
		}
		sleep(100);
	}
	unsubscribe_from_event(EVENT_LASER_RIGHT_DETECT);
}

void rfid_to_laser_right() {
	led_set_color(LED_GREEN);
	servo_angle_set(ARM_LEFT);
	belt_small_set_speed(BELT_UP_SLOW);
	belt_big_set_speed(BELT_RIGHT_SLOW);
	subscribe_to_event(EVENT_LASER_RIGHT_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_LASER_RIGHT_DETECT) {
			servo_angle_set(ARM_NEUTRAL);
			belt_small_set_speed(BELT_OFF);
			belt_big_set_speed(BELT_OFF);
			led_set_color(LED_BLUE);
			break;
		}
		sleep(100);
	}
	unsubscribe_from_event(EVENT_LASER_RIGHT_DETECT);
}

void rfid_to_laser_left() {
	led_set_color(LED_GREEN);
	servo_angle_set(ARM_RIGHT);
	belt_small_set_speed(BELT_UP_SLOW);
	belt_big_set_speed(BELT_LEFT_SLOW);
	subscribe_to_event(EVENT_LASER_LEFT_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_LASER_LEFT_DETECT) {
			servo_angle_set(ARM_NEUTRAL);
			belt_small_set_speed(BELT_OFF);
			belt_big_set_speed(BELT_OFF);
			led_set_color(LED_BLUE);
			break;
		}
		sleep(100);
	}
	unsubscribe_from_event(EVENT_LASER_LEFT_DETECT);
}

void swap_laser_right_and_rfid() {
	laser_right_to_laser_left();
	led_set_color(LED_GREEN);
	servo_angle_set(60);
	belt_big_set_speed(-10);
	sleep(1500);
	belt_small_set_speed(-10);
	belt_big_set_speed(-1);
	sleep(2500);
	belt_small_set_speed(100);
	servo_angle_set(70);
	sleep(500);
	servo_angle_set(80);
	sleep(500);
	servo_angle_set(90);
	sleep(500);
	servo_angle_set(100);
	sleep(500);
	servo_angle_set(105);
	belt_big_set_speed(-2);
	subscribe_to_event(EVENT_LASER_RIGHT_DETECT);
	subscribe_to_event(EVENT_RFID_DETECT);
	enum EventType e;
	int count = 0;
	while (1) {
		while ((e = next_event())) {
			if (e == EVENT_RFID_DETECT) {
				print("rfid\n");
				servo_angle_set(ARM_NEUTRAL);
				belt_small_set_speed(BELT_OFF);
				unsubscribe_from_event(EVENT_RFID_DETECT);
				count++;
				if (count) { break; }
			} 
			if (e == EVENT_LASER_RIGHT_DETECT) {
				print("laser\n");
				belt_big_set_speed(BELT_OFF);
				unsubscribe_from_event(EVENT_LASER_RIGHT_DETECT);
				count++;
				if (count) { break; }
			}
		}
		if (count == 2) { break; }
		sleep(100);
	}
}

void swap_laser_left_and_rfid() {
laser_left_to_laser_right();
	led_set_color(LED_GREEN);
	servo_angle_set(105);
	belt_big_set_speed(10);
	sleep(1500);
	belt_small_set_speed(-10);
	belt_big_set_speed(1);
	sleep(2500);
	belt_small_set_speed(100);
	servo_angle_set(100);
	sleep(500);
	servo_angle_set(90);
	sleep(500);
	servo_angle_set(80);
	sleep(500);
	servo_angle_set(70);
	sleep(500);
	servo_angle_set(60);
	belt_big_set_speed(2);
	subscribe_to_event(EVENT_LASER_LEFT_DETECT);
	subscribe_to_event(EVENT_RFID_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_RFID_DETECT) {
			print("rfid\n");
			servo_angle_set(ARM_NEUTRAL);
			belt_small_set_speed(BELT_OFF);
			unsubscribe_from_event(EVENT_RFID_DETECT);
			break;
		} 
		sleep(100);
	}
	while (1) {
		e = next_event();
		if (e == EVENT_LASER_LEFT_DETECT) {
			print("laser\n");
			belt_big_set_speed(BELT_OFF);
			unsubscribe_from_event(EVENT_LASER_LEFT_DETECT);
			break;
		} 
		sleep(100);
	}
}