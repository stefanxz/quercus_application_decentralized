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

/// @brief Checks if the module is storing a tub
/// @details The function checks the state of the module to determine if it is currently storing a tub.
/// @return ID of the tub that is currently being stored in the module
int8_t is_storing(){
	for(int i = 0; i < 3; i++){
		if(this.tub[i].id != NON) {
			return i;
		}
	}
	return NON;
}

/// @brief 
/// @param id_look_up the ID lookup table to be filled in
/// @param direction_look_up 
/// @param storage_cycle 
/// @param nearest_dest 
void await_request_path_config(uint8_t* id_lookup, uint8_t* dir_lookup, uint8_t* storage_cycle, uint8_t* nearest, uint8_t* next){
	char* msg;
	char type;

	// Wait for the path config message from the Pi module until you get it
	// if any other message is received, ignore it.
	EventType e = next_event();
	while (true) {
		if(e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(&msg);
			type = msg[MSG_TYPE];

			// If you get the message you need, return it
			if (type == PATH_CONFIG) {
				break;
			}
		}

		e = next_event();
		sleep(PAUSE);
	}

	uint8_t sender = msg[MSG_SENDER];
	if(sender != 0) {
		printf("Path config received but not from Pi.\n");
	}

	// Copy the data from the message to the lookup tables and variables
	memcpy(id_lookup, msg+2, MAX_NUMBER_OF_MODULES);
	memcpy(dir_lookup, msg+2+MAX_NUMBER_OF_MODULES, MAX_NUMBER_OF_MODULES);
	memcpy(storage_cycle, msg+2+MAX_NUMBER_OF_MODULES*2, MAX_NUMBER_OF_MODULES);
	memcpy(nearest, msg+2+MAX_NUMBER_OF_MODULES*3, NUMBER_OF_DEST_TYPES);
	memcpy(next, msg+2+MAX_NUMBER_OF_MODULES*3+NUMBER_OF_DEST_TYPES, 3);
	free(msg);
}

/// @brief Sends a request for the path configuration to the Pi module, await the response, handle it.
/// @details The function sends a message to the Pi module requesting the path configuration.
/// It then waits for a response and processes the received data.
/// @param id_lookup the ID lookup table to be filled in
/// @param dir_lookup the direction lookup table to be filled in
/// @param storage_cycle the storage cycle table to be filled in
/// @param nearest the nearest destination table to be filled in
/// @param next the next module table to be filled in
int get_path_config(uint8_t* id_lookup, uint8_t* dir_lookup, uint8_t* storage_cycle, uint8_t* nearest, uint8_t* next){
	send_request_path_config();
	await_request_path_config(id_lookup, dir_lookup, storage_cycle, nearest, next);	
}

/// @brief Checks if the module shouldb be a storage module.
/// @param storage_cycle the current storage cycle of the layout
/// @param id module ID to check
/// @return 
bool is_storage(uint8_t storage_cycle[MAX_NUMBER_OF_MODULES], uint8_t id){
	for (int i = 0; i < MAX_NUMBER_OF_MODULES; i++) {
		if(id == storage_cycle[i]) return 1;
	}
	return 0;
}

/// @brief Perform a system check by verifying the status of the laser and RFID sensors, and resetting the module.
/// @details The function checks the status of the laser and RFID sensors. 
// If any of them are not functioning properly, it prints an error message.
void sys_check() {
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
}

/// @brief Initializes the module by checking the system, setting up the ID, and subscribing to events.
/// @details The function initializes the module by checking the system, setting up the ID, and subscribing to events. 
/// It also retrieves the path configuration and sets the initial state of the module.
/// @return void
/// @note This function is called at the beginning of the program to set up the module.
void init() {
	sys_check();
	led_set_color(COLOR_YELLOW);
	this.id = get_own_id();
	uint8_t storage_cycle[MAX_NUMBER_OF_MODULES];
	led_set_color(0xff7700);
	subscribe_to_event(EVENT_MESSAGE_RECEIVED);
	
	get_path_config(this.id_lookup, this.dir_lookup, storage_cycle, this.nearest, this.next);
	
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
	
	led_set_color(COLOR_RED);

	printf("I am done with the setup.\n");
	sleep(10);
}

/// @brief Updates the state after a task is completed.
/// @param from the "from" direction of the task
/// @param to the "to" direction of the task
/// @param tub_id the ID of the tub that was processed
/// @details The function updates the state of the module by setting the "from" direction to NON and 
// the "to" direction to the tub ID to indicate that the tub has moved to that position.
void update_state(Direction from, Direction to, int tub_id) {
	if(from != OUT) state.at[from] = NON;
	if(to != OUT) state.at[to] = tub_id;
}

/// @brief Adds a task to the task ring buffer.
/// @param from which endpoint of the module the tub moves from
/// @param to which endpoint of the module the tub moves to
/// @param request related request that led to the addition of this task
void add_task(Direction from, Direction to, char* request) {
	printf("adding task: %d, %d\n", from, to, request);
	Task task;
	task.to = to;
	task.from = from;

	// POTENTIALLY SMELLY CODE
	memcpy(task.request, request, REQ_LENGTH);

	printf("task request: %d, %d, %d, %d\n", request[MSG_SENDER], request[REQ_TUB_ID], request[REQ_DEST_TYPE], request[REQ_DEST_ID]);
	tasks[task_new] = task;
	task_new = (task_new + 1) % MAX_TASKS;
}

/// @brief Checks if there are no tasks to be done in the ring buffer.
/// @details The function checks if the current task index is equal to the new task index.
/// @return true if there are no tasks, false otherwise
bool no_tasks() { return (task_current == task_new); }