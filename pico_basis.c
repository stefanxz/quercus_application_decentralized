#pragma once
#include "libc_builtin.h"
#include "quercus_lib_pico.h"

#include "lib/essentials.h"
#include "lib/network.h"
#include "lib/movement.h"
#include "lib/movement.c"

#include <stdbool.h>

const Task empty = {.to = OUT, .from = OUT};

Module this;
State state;

Task tasks[MAX_TASKS];
int8_t task_current = 0;
int8_t task_new = 0;

int8_t is_storing(){
	for(int i = 0; i < 3; i++){
		if(this.tub[i].id != NON) {
			return i;
		}
	}
	return NON;
}

int await_path_config_message(char** msg_ptr, bool persistent) {
	int response = NON;
	char type;
	do {
		EventType e = next_event();
		// Sift mailbox for messages:
		while (e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(msg_ptr);
			type = (*msg_ptr)[MSG_TYPE];
			if (type == REQUEST_MOVEMENT) {
				// complicated decision as to whether to accept or reject the request here i guess
			} else if (type == REQUEST_RESPONSE) {
				// response = handle_request_response(*msg_ptr);
			} else if (type == PLANE_STATUS) {
				// response = handle_plane_status(*msg_ptr);
			} else if (type == PATH_CONFIG) {
				response = 1;
			} else if (type == TUB_CONFIG) {
				// response = handle_tub_config(*msg_ptr);
			}

			// If you get the message you need, return it
			if (type == PATH_CONFIG) {
				return response;
			}
			e = next_event();

			sleep(PAUSE);
		}
	} while (persistent);
	return response;
}

 int await_request_path_config(uint8_t* id_look_up, uint8_t* direction_look_up, uint8_t* storage_cycle, uint8_t* nearest_dest, uint8_t* next){
	char* msg;
	int response = await_path_config_message(&msg, true);
	uint8_t sender = msg[0];
	if(sender != 0) {
		printf("Path config received but not from Pi.\n");
	}
	if (response >= 0) {
		memcpy(id_look_up, msg+2, MAX_NUMBER_OF_MODULES);
		memcpy(direction_look_up, msg+2+MAX_NUMBER_OF_MODULES, MAX_NUMBER_OF_MODULES);
		memcpy(storage_cycle, msg+2+MAX_NUMBER_OF_MODULES*2, MAX_NUMBER_OF_MODULES);
		memcpy(nearest_dest, msg+2+MAX_NUMBER_OF_MODULES*3, NUMBER_OF_DEST_TYPES);
		memcpy(next, msg+2+MAX_NUMBER_OF_MODULES*3+NUMBER_OF_DEST_TYPES, 3);
		free(msg);
		return 1;
	} else {
		return 0;
	}
}

int get_path_config(uint8_t* id_look_up, uint8_t* direction_look_up, uint8_t* storage_cycle, uint8_t* nearest_dest, uint8_t* next){
	send_request_path_config();
	await_request_path_config(id_look_up, direction_look_up, storage_cycle, nearest_dest, next);
	
}

bool is_storage(uint8_t storage_cycle[MAX_NUMBER_OF_MODULES], uint8_t id){
	for (int i = 0; i < MAX_NUMBER_OF_MODULES; i++) {
		if(id == storage_cycle[i]) return 1;
	}
	return 0;
}

int sys_check(){
	if(!laser_left_detect()) {
		printf("Laser left aint good\n");
	}
	if(!laser_left_detect()) {
		printf("Laser right aint good\n");
	}
	if(RFID_check_tag()) {
		printf("RFID aint good\n");
	}
	belt_big_set_speed(BELT_UP_SLOW);
	belt_small_set_speed(BELT_UP_SLOW);
	sleep(200);
	belt_big_set_speed(BELT_DOWN_SLOW);
	belt_big_set_speed(BELT_DOWN_SLOW);
	sleep(200);
	reset_module();
	return 0;
}

void init() {
	sys_check();
	this.id = get_own_id();
	uint8_t storage_cycle[MAX_NUMBER_OF_MODULES];
	led_set_color(0xff7700);
	subscribe_to_event(EVENT_MESSAGE_RECEIVED);
	
	get_path_config(this.id_lookup, this.dir_lookup, storage_cycle, this.nearest, this.next);
	
	printf("id_look_up: %d, %d, %d, %d, %d\n", this.id_lookup[3], this.id_lookup[4], this.id_lookup[5], this.id_lookup[6], this.id_lookup[7]);
	printf("dir_look_up: %d, %d, %d, %d, %d\n", this.dir_lookup[3], this.dir_lookup[4], this.dir_lookup[5], this.dir_lookup[6], this.dir_lookup[7]);
	printf("next: %d, %d, %d\n", this.next[0], this.next[1], this.next[2]);
	// Initialize the module's state
	state.at[RFID] = NON;
	state.at[LASER_LEFT] = NON;
	state.at[LASER_RIGHT] = NON;

	this.is_storage = is_storage(storage_cycle, this.id);
	
	this.to_storage = this.dir_lookup[this.nearest[STORAGE]];
	
	for (int i = 0; i < MAX_NUMBER_OF_PLANES; i++) {
		this.plane_to_id[i] = 0;
	}

	task_current = 0;
	task_new = 0;
	Task empty = {.from = OUT, .to = OUT};
	for (int i = 0; i < MAX_TASKS; ++i) {
		tasks[i] = empty;
	}

	for(int i = 0; i < 3; i++) {
		this.tub[i].id = NON;
	}

	printf("I am done with the setup.\n");
	sleep(10);
}

// CHANGE FAST AF
void update_state(Direction from, Direction to, int tub_id) {
	if(from != OUT) state.at[from] = NON;
	if(to != OUT) state.at[to] = tub_id;
}

void copy_message(char* request, char* msg) {
	for (int i = 0; i < REQ_LENGTH; i++) {
		request[i] = msg[i];
	}
}

bool add_task(Direction from, Direction to, char* request) {
	printf("adding task: %d, %d\n", from, to, request);
	Task task;
	task.to = to;
	task.from = from;

	// POTENTIALLY SMELLY
	copy_message(task.request, request);

	tasks[task_new] = task;
	task_new = (task_new + 1) % MAX_TASKS;
	return true;
}

bool no_tasks() { return (task_current == task_new); }