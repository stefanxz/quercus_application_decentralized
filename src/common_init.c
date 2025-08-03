#include "defs.h"
#include "common_hardware.h"

#include "quercus_lib_pico.h"
#include "libc_builtin.h"


// Function declarations
void sys_check();



/// @brief Checks if the module shouldb be a storage module.
/// @param storage_cycle the current storage cycle of the layout
/// @param id module ID to check
/// @return false if the module is not a storage module, true if it is a storage module
bool is_storage(uint8_t storage_cycle[Q_MAX_NUMBER_OF_MODULES], uint8_t id) {
	for (int i = 0; i < Q_MAX_NUMBER_OF_MODULES; i++) {
		if (id == storage_cycle[i]) return 1;
	}
	return 0;
}

/// @brief Perform a system check by verifying the status of the laser and DIR_RFID sensors, and resetting the module.
/// @details The function checks the status of the laser and DIR_RFID sensors.
// If any of them are not functioning properly, it prints an error message.
void sys_check() {
	if (!laser_left_detect()) {
		printf("Laser left aint good\n");
	}
	if (!laser_left_detect()) {
		printf("Laser right aint good\n");
	}
	if (RFID_check_tag()) {
		printf("DIR_RFID aint good\n");
	}
	belt_big_set_speed(BELT_UP_SLOW);
	belt_small_set_speed(BELT_UP_SLOW);
	sleep(200);
	belt_big_set_speed(BELT_DOWN_SLOW);
	belt_big_set_speed(BELT_DOWN_SLOW);
	sleep(200);
	reset_module();
}


/// @brief Sends a request for the path configuration to the Pi module, await the response, handle it.
/// @details The function sends a message to the Pi module requesting the path configuration.
/// It then waits for a response and processes the received data.
/// @param id_lookup the ID lookup table to be filled in
/// @param dir_lookup the direction lookup table to be filled in
/// @param storage_cycle the storage cycle table to be filled in
/// @param nearest the nearest destination table to be filled in
/// @param next the next module table to be filled in
void get_path_config(uint8_t* id_lookup, uint8_t* dir_lookup, uint8_t* storage_cycle, uint8_t* nearest, uint8_t* next) {
	// request config from pi
	char data[2] = {get_own_id(), REQUEST_PATH_CONFIG};
	send_packet(0, data, sizeof(data));

	uint8_t* msg;
	char type;

	// Wait for the path config message from the Pi module until you get it
	// if any other message is received, ignore it.
	EventType e = next_event();
	while (true) {
		if (e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(&msg);
			type = msg[MSG_TYPE];

			// If you get the message you need, return it
			if (type == MSG_PATH_CONFIG) {
				break;
			}
		}

		e = next_event();
		sleep(TIME_PAUSE);
	}

	uint8_t sender = msg[MSG_SENDER];
	if (sender != 0) {
		printf("Path config received but not from Pi.\n");
	}

	// Copy the data from the message to the lookup tables and variables
	memcpy(id_lookup, msg + 2, Q_MAX_NUMBER_OF_MODULES);
	memcpy(dir_lookup, msg + 2 + Q_MAX_NUMBER_OF_MODULES, Q_MAX_NUMBER_OF_MODULES);
	memcpy(storage_cycle, msg + 2 + Q_MAX_NUMBER_OF_MODULES * 2, Q_MAX_NUMBER_OF_MODULES);
	memcpy(nearest, msg + 2 + Q_MAX_NUMBER_OF_MODULES * 3, Q_NUMBER_OF_DEST_TYPES);
	memcpy(next, msg + 2 + Q_MAX_NUMBER_OF_MODULES * 3 + Q_NUMBER_OF_DEST_TYPES, 3);
	free(msg);
}

/// @brief Initializes the module by checking the system, setting up the ID, and subscribing to events.
/// @return void
/// @note This function is called at the beginning of the program to set up the module.
Module module_init() {
	// Check that all of the sensors are working and reset the module
	sys_check();

	// Set the LED color to yellow to indicate that the module is initializing
	led_set_color(COLOR_YELLOW);

	Module module;
	// State state;

	// Set the module ID and subscribe to events
	module.id = get_own_id();
	uint8_t storage_cycle[Q_MAX_NUMBER_OF_MODULES];
	subscribe_to_event(EVENT_MESSAGE_RECEIVED);

	get_path_config(module.id_lookup, module.dir_lookup, storage_cycle, module.nearest, module.next);
	// Set the ID lookup table and direction lookup table by getting them from the Pi
	// send_request_path_config();
	// await_request_path_config(module.id_lookup, module.dir_lookup, storage_cycle, module.nearest, module.next);
	//

	// Initialize the state of the module to be empty
	module.state.at[DIR_RFID] = Q_NULL;
	module.state.at[DIR_LASER_LEFT] = Q_NULL;
	module.state.at[DIR_LASER_RIGHT] = Q_NULL;

	// Save if the module is a storage module
	module.is_storage = is_storage(storage_cycle, module.id);

	// Set the to_storage variable to the ID of the nearest storage module
	module.to_storage = module.dir_lookup[module.nearest[DEST_STORAGE]];

	// Initialize the plane_to_id table, the tasks ring buffer and tub array to be empty
	for (int i = 0; i < Q_MAX_NUMBER_OF_PLANES; i++) {
		module.plane_to_id[i] = 0;
	}

	module.task_current = 0;
	module.task_new = 0;
	Task empty = {.from = DIR_OUT, .to = DIR_OUT};
	for (int i = 0; i < Q_MAX_TASKS; ++i) {
		module.tasks[i] = empty;
	}

	for (int i = 0; i < 3; i++) {
		module.tub[i].id = Q_NULL;
	}

	// Set the LED color to red to indicate that the module is ready
	led_set_color(COLOR_GREEN);

	printf("I am done with the setup.\n");
	sleep(10);

	return module;
}
