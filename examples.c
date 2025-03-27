#include "functions/movement.c"
#include "functions/network.c"
#include "functions/rfid.c"
#include "libc_builtin.h"
#include "quercus_lib_pico.h"

int current_tub_id = 0;

// int handle_request_movement(char* msg) {
//     //TODO: Replace test code below
//     char test[3];
//     test[0] = 0;
//     test[1] = REQUEST_RESPONSE;
//     test[2] = 1; // Accept or reject.
//     send_packet(msg[0], test, sizeof(test));
//     return msg[2];
// }

// int handle_request_response(char* msg) {
//     //TODO: Handle message
//     return  msg[2];
// }

// int handle_plane_status(char* msg) {
//     //TODO: Handle message
// }

// int handle_paths_config(char* msg) {
//     //TODO: Handle message
// }

// int handle_tub_config(char* msg) {
//     return set_tub_id(msg[2]);
// }

// int handle_message(char* msg, int expected_type) {
//     int message_type = msg[1];
//     int response;

//     if (message_type == REQUEST_MOVEMENT) {
//         response = handle_request_movement(msg);
//     } else if (message_type == REQUEST_RESPONSE) {
//         response = handle_request_response(msg);
//     } else if (message_type == PLANE_STATUS) {
//         response = handle_plane_status(msg);
//     } else if (message_type == PATHS_CONFIG) {
//         response = handle_paths_config(msg);
//     } else if (message_type == TUB_CONFIG) {
//         response = handle_tub_config(msg);
//     }

//     if (expected_type == message_type) {
//         return response;
//     }
//     return -1;
// }

void demo() {
	printf("start\n");
	reset_module();
	while (1) {
		if (RFID_check_tag()) {
			move_within_module(0, RFID, LASER_RIGHT);
			move_within_module(0, LASER_RIGHT, RFID);
			move_within_module(0, RFID, LASER_LEFT);
			move_within_module(0, LASER_LEFT, RFID);
			move_within_module(0, RFID, LASER_RIGHT);
			move_within_module(0, LASER_RIGHT, LASER_LEFT);
			move_within_module(0, LASER_LEFT, LASER_RIGHT);
			move_within_module(0, LASER_RIGHT, RFID);
			printf("end\n");
		}

		sleep(50);
	}
}

// NOT HOW IT SHOULD BE CODED IN THE APPLICATION, THIS IS JUST TO PUT DATA ON THE RFID!!!
// void rfid_write_demo() {
// 	print("start\n");
// 	led_set_color(0xff0000);
// 	belt_big_set_speed(0);
// 	belt_small_set_speed(0);
// 	servo_angle_set(0);
// 	subscribe_to_event(EVENT_RFID_DETECT);
// 	enum EventType e;
// 	while (1) {
// 		e = next_event();
// 		if (e == EVENT_RFID_DETECT) {
// 			led_set_color(0x00ff00);
// 			char data[4];
// 			data[0] = 0x1;
// 			RFID_write_data_block((int)data, 0);
// 			data[0] = 0x1;
// 			RFID_write_data_block((int)data, 1);
// 			data[0] = 0x56;
// 			RFID_write_data_block((int)data, 2);
// 			data[0] = 0x0;
// 			RFID_write_data_block((int)data, 3);
// 			data[0] = 0x0;
// 			RFID_write_data_block((int)data, 6);
// 			data[0] = 0x1;
// 			RFID_write_data_block((int)data, 7);
// 			data[0] = 0x34;
// 			RFID_write_data_block((int)data, 8);
// 			subscribe_to_event(EVENT_RFID_DETECT);
// 			print("end\n");
// 		}
// 		sleep(100);
// 	}
// }

// void rfid_read_demo() {
// 	print("start\n");
// 	led_set_color(0xff0000);
// 	belt_big_set_speed(0);
// 	belt_small_set_speed(0);
// 	servo_angle_set(0);
// 	subscribe_to_event(EVENT_RFID_DETECT);
// 	enum EventType e;
// 	while (1) {
// 		e = next_event();
// 		if (e == EVENT_RFID_DETECT) {
// 			led_set_color(0x00ff00);
// 			char data[4];
// 			int rfid = get_rfid_data(SECURITY);
// 			printf("Security: %02X\n", rfid);
// 			rfid = get_rfid_data(PLANE_DROPOFF);
// 			printf("Plane/Dropoff: %02X\n", rfid);
// 			rfid = get_rfid_data(PLANE_ID);
// 			printf("Plane: %02X\n", rfid);
// 			rfid = get_rfid_data(PAYLOAD);
// 			printf("Payload: %02X\n", rfid);
// 			rfid = get_rfid_data(PASSED_SECURITY);
// 			printf("Passed Security: %02X\n", rfid);
// 			rfid = get_rfid_data(PLANE_ARRIVED);
// 			printf("Plane Arrived: %02X\n", rfid);
// 			rfid = get_rfid_data(DESTINATION);
// 			printf("Destination: %02X\n", rfid);
// 			subscribe_to_event(EVENT_RFID_DETECT);
// 			print("end\n");
// 		}
// 		sleep(100);
// 	}
// }

// void demo_comm(int neighbour) {
// 	print("start\n");
// 	reset_module();
// 	subscribe_to_event(EVENT_MESSAGE_RECEIVED);
// 	char* msg;
// 	while (1) {
// 		if (RFID_check_tag()) {
// 			move_within_module(0, RFID, LASER_RIGHT);
// 			char data[END_OF_ENUM];
//             //Fill data with tub data.
// 			move_to_neighbour(neighbour, data, LASER_RIGHT);
// 		}
// 		if (next_event() == EVENT_MESSAGE_RECEIVED) {
// 			next_message_address(&msg);
// 			int response = handle_message(msg, REQUEST_MOVEMENT);
// 			belt_big_set_speed(BELT_LEFT_SLOW);
// 			while(1) {
// 				if (RFID_check_tag()) {
//                     send_updated_tub_location(msg[TUB_ID+2], LASER_RIGHT);
// 					move_within_module(0, LASER_RIGHT, LASER_LEFT);
// 					move_within_module(0, LASER_LEFT, RFID);
// 			        free(msg);
// 					break;
// 				}
// 			}
// 		}
// 		sleep(10);
// 	}
// 	print("end");
// }

// void demo_loop(int neighbour) {
//     print("start\n");
//     reset_module();
// 	subscribe_to_event(EVENT_MESSAGE_RECEIVED);
// 	char* msg;
//     char data[END_OF_ENUM];
//     //Fill data with tub data
//     while (1) {
//         if (RFID_check_tag()) {
// 			belt_small_set_speed(BELT_OFF);
// 			move_within_module(-1, RFID, LASER_RIGHT);
// 			move_to_neighbour(neighbour, data, LASER_RIGHT);
// 		}
//         if (next_event() == EVENT_MESSAGE_RECEIVED) {
// 			next_message_address(&msg);
// 			int response = handle_message(msg, REQUEST_MOVEMENT);
// 			belt_small_set_speed(BELT_UP_SLOW);
//             free(msg);
// 		}
//         sleep(10);
//     }
// 	print("end\n");
// }

// void tub_and_plane_entrance() {
//     print("start\n");
//     reset_module();
// 	belt_small_set_speed(BELT_UP_SLOW);
// 	subscribe_to_event(EVENT_MESSAGE_RECEIVED);
//     char rfid_data[END_OF_ENUM];
// 	char* msg;
//     while (1) {
//         if (RFID_check_tag()) {
// 			belt_small_set_speed(BELT_OFF);
//             get_entrance_rfid_data(rfid_data);
//             if (rfid_data[TUB_PLANE]) {
//                 send_tub_status(rfid_data);
//             } else {
//                 send_plane_status(rfid_data[PLANE_ID]);
//                 break;
//             }
//         }
//         if (next_event() == EVENT_MESSAGE_RECEIVED) {
//             next_message_address(&msg);
//             if (msg[MSG_TYPE] == TUB_CONFIG) {
//                 set_tub_id(msg[2]);
//                 break;
//             } else {
//                 // Handle Message otherwise
//             }
//         }
//         sleep(10);
//     }
//     print("end\n");
// }