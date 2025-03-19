#include "quercus_lib_pico.h"
#include "libc_builtin.h"

#include "elementary_functions/movement_functions.c"
#include "elementary_functions/rfid_functions.c"

#include <stdbool.h>

void demo() {
	print("start\n");
	led_set_color(0xff0000);
	belt_big_set_speed(0);
	belt_small_set_speed(0);
	servo_angle_set(0);
	subscribe_to_event(EVENT_RFID_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_RFID_DETECT) {
			rfid_to_laser_right();
			sleep(1000);
			laser_right_to_rfid();
			sleep(1000);
			rfid_to_laser_left();
			sleep(1000);
			laser_left_to_rfid();
			sleep(1000);
			rfid_to_laser_right();
			sleep(1000);
			laser_right_to_laser_left();
			sleep(1000);
			laser_left_to_laser_right();
			subscribe_to_event(EVENT_RFID_DETECT);
			print("end\n");
		}
		sleep(100);
	}
}

void rfid_write_demo() {
	print("start\n");
	led_set_color(0xff0000);
	belt_big_set_speed(0);
	belt_small_set_speed(0);
	servo_angle_set(0);
	subscribe_to_event(EVENT_RFID_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_RFID_DETECT) {
			led_set_color(0x00ff00);
			char data[4];
			data[0] = 0x1;
			RFID_write_data_block((int)data, 0);
			data[0] = 0x1;
			RFID_write_data_block((int)data, 1);
			data[0] = 0x56;
			RFID_write_data_block((int)data, 2);
			data[0] = 0x0;
			RFID_write_data_block((int)data, 3);
			data[0] = 0x0;
			RFID_write_data_block((int)data, 6);
			data[0] = 0x1;
			RFID_write_data_block((int)data, 7);
			data[0] = 0x34;
			RFID_write_data_block((int)data, 8);
			subscribe_to_event(EVENT_RFID_DETECT);
			print("end\n");
		}
		sleep(100);
	}
}

void rfid_read_demo() {
	print("start\n");
	led_set_color(0xff0000);
	belt_big_set_speed(0);
	belt_small_set_speed(0);
	servo_angle_set(0);
	subscribe_to_event(EVENT_RFID_DETECT);
	enum EventType e;
	while (1) {
		e = next_event();
		if (e == EVENT_RFID_DETECT) {
			led_set_color(0x00ff00);
			int rfid = get_security_flag();
			printf("Security: %02X\n", rfid);
			rfid = get_plane_dropoff_flag();
			printf("Plane/Dropoff: %02X\n", rfid);
			rfid = get_plane_id();
			printf("Plane: %02X\n", rfid);
			rfid = get_payload();
			printf("Payload: %02X\n", rfid);
			rfid = has_security_been_passed();
			printf("Passed Security: %02X\n", rfid);
			rfid = has_plane_arrived();
			printf("Plane Arrived: %02X\n", rfid);
			rfid = get_destination();
			printf("Destination: %02X\n", rfid);
			subscribe_to_event(EVENT_RFID_DETECT);
			print("end\n");
		}
		sleep(100);
	}
}

export int main(void) {
	demo();
}
