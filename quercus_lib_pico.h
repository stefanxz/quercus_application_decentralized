#pragma once

#define export __attribute__((visibility("default"))) __attribute__((used))

#define NULL ((void*)0)

#include <stdint.h>

typedef enum EventType {
	EVENT_NONE = 0,
	EVENT_LASER_RIGHT_DETECT = 1,
	EVENT_LASER_LEFT_DETECT = 2,
	EVENT_ENCODER_LONG_STOPPED = 4,
	EVENT_ENCODER_SHORT_STOPPED = 8,
	EVENT_CURRENT_THRESHOLD_REACHED = 16,
	EVENT_POWER_THRESHOLD_REACHED = 32,
	EVENT_MESSAGE_RECEIVED = 64, // special event, needs buffer for data
	EVENT_MESSAGE_ALLOC_FAILED = 128,
	EVENT_RFID_DETECT = 256,
} EventType;

extern void print(const char* str);
extern void sleep(int ms);

extern int get_own_id();
extern char* get_network_map();

extern void subscribe_to_event(int event_type);
extern void unsubscribe_from_event(int event_type);
extern enum EventType next_event();

extern int laser_right_detect();
extern int laser_left_detect();
extern int laser_right_get();
extern int laser_right_set(int on);
extern int laser_left_get();
extern int laser_left_set(int on);

extern int servo_angle_set(float angle);
extern float servo_angle_get();

extern int led_set_color(int color);
extern int led_set_rgb(int r, int g, int b);
extern int led_get_color();

extern float CV_sensor_get_voltage();
extern float CV_sensor_get_power();
extern float CV_sensor_get_current();

extern int belt_small_set_speed(float speed);
extern float belt_small_get_speed();
extern int64_t belt_small_get_encoder_count();
extern double belt_small_get_encoder_freq();

extern int belt_big_set_speed(float speed);
extern float belt_big_get_speed();
extern int64_t belt_big_get_encoder_count();
extern double belt_big_get_encoder_freq();

extern int RFID_check_tag();
// extern int RFID_write_string(int data_ptr);
extern int RFID_write_data_block(int data_ptr, int offset);
// extern int RFID_read_string(int data_ptr, int str_len);
extern int RFID_read_data_block(int data_ptr, int offset);
extern int RFID_get_uid(int uid_pointer);

/*
 * Call this function after receiving an EVENT_MESSAGE_RECEIVED event from next_event()
 *
 *
 * The address of the buffer containing the content of the message will be written to
 * address_ptr. The length of the message is returned by this function.
 * If no message is pending, returns 0;
 * If the network event had resulted in a failed allocation, and there was no
 * memory left in the WAMR heap, then returns -1.
 * Other errors are other negative numbers.
 */
extern int next_message_address(char** address_ptr);

/**
 * Send a packet to the IP address with the last octet given by last_dest_octet.
 * The packet is sent with the data in the buffer at app_data, with the size of data_size.
 */
extern int send_packet(int last_dest_octet, char* app_data, int data_size);

/*
extern int get_pico_id(void);

extern int read_rfid(void);
extern int read_laser(void);
extern void run_motor(int speed);
extern void stop_motor(void);

typedef int(*request_callback)(void* payload);
extern void on_request(request_callback callback);
extern void send_request(int target, void* payload);
*/
