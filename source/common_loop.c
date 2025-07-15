#include "defs.h"
#include "libc_builtin.h"
#include "quercus_lib_pico.h"

#include "common_hardware.c"

#include <stdbool.h>

/// @brief Checks if the module is storing a tub
/// @return ID of the tub that is currently being stored in the module
int8_t is_storing(const Module module) {
	for (int i = 0; i < 3; i++) {
		if (module.tub[i].id != Q_NULL) {
			return i;
		}
	}
	return Q_NULL;
}

/// @brief Adds a task to the task ring buffer.
/// @param from which endpoint of the module the tub moves from
/// @param to which endpoint of the module the tub moves to
/// @param request related request that led to the addition of this task
void add_task(Module* module, Direction from, Direction to, uint8_t* request) {
	printf("adding task: from %d, to %d, request: %s\n", from, to, request);
	Task task;
	task.to = to;
	task.from = from;

	// Copy over the request into the task
	memcpy(task.request, request, REQ_LENGTH);

	printf("task request: %d, %d, %d, %d\n", request[MSG_SENDER], request[REQ_TUB_ID], request[REQ_DEST_TYPE],
		   request[REQ_DEST_ID]);
	module->tasks[module->task_new] = task;
	module->task_new = (module->task_new + 1) % MAX_TASKS;
}

/// @brief Take care of stored tubs in the module.
void handle_storage(Module* module) {
	// Create a request to send to the next module
	uint8_t stor_req[REQ_LENGTH];
	// Get the position of the stored tub on the module
	Direction pos_stored_tub = is_storing(*module);

	// Fill the request to send to the next module
	stor_req[MSG_SENDER] = module->id;
	stor_req[MSG_TYPE] = REQUEST_MOVEMENT;
	stor_req[REQ_TUB_ID] = module->tub[pos_stored_tub].id;
	stor_req[REQ_PLANE_ID] = module->tub[pos_stored_tub].plane_id;
	stor_req[REQ_DEST_TYPE] = STORAGE;
	stor_req[REQ_DEST_ID] = module->next[module->to_storage];

	// Reroute the tub from its initial position towards the next module
	add_task(module, pos_stored_tub, module->to_storage, stor_req);
	add_task(module, module->to_storage, OUT, stor_req);
}

/// @brief Library for handling routing in the Quercus decentralized application.
/// @param msg The incoming message containing movement request details.
/// @return Returns a status code indicating the success or failure of the operation.
int handle_request_movement(Module* module, uint8_t* msg) {

	// Get the plane arrival status and the plane ID from the message
	int plane_arrived = msg[REQ_PLANE_ARRIVED];
	int plane = msg[REQ_PLANE_ID];

	// Reroute tub if its plane has arrived
	if (!plane_arrived && module->plane_to_id[plane] != 0 && msg[REQ_DEST_TYPE] == STORAGE) {
		msg[REQ_DEST_ID] = module->plane_to_id[plane];
		msg[REQ_PLANE_ARRIVED] = 1;
		msg[REQ_DEST_TYPE] = PLANE;
	}

	// Get the sender and destination IDs from the message
	int origin = msg[MSG_SENDER];
	int end = module->id_lookup[msg[REQ_DEST_ID]];

	// Check if the sender id is valid
	if (end < 1) {
		printf("Bad request received: %d\n", end);
		return -1;
	}

	Direction from;
	Direction to;

	// Determine the beginning and final positions of the tub
	for (int i = 0; i < 3; i++) {
		if (module->next[i] == origin) from = i;
		if (module->next[i] == end) to = i;
	}

	// If the sender is this module, set the source position to RFID
	if (origin == module->id) from = RFID;

	// If the current module is storage and if a tub is being stored
	if (module->is_storage && is_storing(*module) != Q_NULL) {
		printf("I will handle storage\n");
		// Handle the stored tub
		handle_storage(module);
	}

	// Receive tub at one of your endpoints:
	add_task(module, OUT, from, msg);

	// If the destination is the current module
	if (end == module->id) {
		// If the destination type is storage and the current module is a storage module
		if (msg[REQ_DEST_TYPE] == STORAGE && module->is_storage) {
			// Store the tub at the position it is at
			printf("Saving plane id %d to position %d\n", plane, from);
			module->tub[from].plane_id = plane;
		} else if (msg[REQ_DEST_TYPE] == SECURITY) {
			// If the destination type is security, send the tub to the RFID
			add_task(module, from, RFID, msg);
		} else {
			// The tub should exit the system
			add_task(module, from, RFID, msg);
			add_task(module, RFID, OUT, msg);
		}
		return 0;
	}
	// If the destination is not the current module, route the tub to the next module
	add_task(module, from, to, msg);
	add_task(module, to, OUT, msg);

	return 0;
}

// /// @brief Handles the response to a request movement message.
// int handle_request_response(Module* module, char* msg) { return msg[MSG_VALUE]; }

/// @brief Handles routing logic from a source 'from' to a specified destination 'destination_id'.
/// @param from The source from which the routing process begins.
/// @param destination_id The unique identifier of the destination to route to.
void reroute_stored_tub(Module* module, Direction from, uint8_t destination_id, DestinationType dest_type) {
	// Create and fill the request to send to the next module
	uint8_t request[REQ_LENGTH];
	request[MSG_SENDER] = module->id;
	request[MSG_TYPE] = REQUEST_MOVEMENT;
	request[REQ_TUB_ID] = module->state.at[from];
	request[REQ_DEST_TYPE] = dest_type;
	request[REQ_DEST_ID] = destination_id;

	// Determine the direction to the destination
	Direction to = module->dir_lookup[module->id_lookup[destination_id]];

	// Reroute the tub from its initial position towards the next module
	add_task(module, from, to, request);
	add_task(module, to, OUT, request);
}

/// @brief Handles the status of a plane, including its arrival and departure.
/// @param msg The incoming message containing plane status details.
/// @return Returns 2 if it noted the plane arrival but is already busy and will not handle the arrival;
///
/// 		Returns 1 if it can already reroute its tub towards the landed plane;
///
///			Returns 0 if the plane was leaving;
///
/// 		Returns -1 if an error occurred with the plane schedule.
int handle_plane_status(Module* module, uint8_t* msg) {
	// If the message sender is not the Pi
	if (msg[MSG_SENDER] != 0) {
		return Q_NULL;
	}
	// If the plane has not been saved in the system yet
	if (module->plane_to_id[msg[ARR_PLANE_ID]] == 0) {
		// Save that the plane is coming.
		module->plane_to_id[msg[ARR_PLANE_ID]] = msg[ARR_MODULE_ID];
		printf("I got a plane update: plane %d landed on %d with departure time %d\n", msg[ARR_PLANE_ID],
			   msg[ARR_MODULE_ID], msg[ARR_DEP_TIME]);

		// If the module is busy, it will not handle the plane.
		if (module->task_current != module->task_new) {
			printf("I am doing something, the next module will deal with the plane.\n");
			return 2;
		}
		// Save the direction towards the plane
		int pos = module->dir_lookup[msg[ARR_MODULE_ID]];

		// If a tub is stored at the position towards the plane
		if (module->tub[pos].id != Q_NULL) {
			// If that tub has to go to the plane, route it there.
			if (module->tub[pos].plane_id == msg[ARR_PLANE_ID]) {
				printf("I am rerouting a tub stored at direction towards the plane to it.\n");
				// If that tub has to go to the plane, route it there.
				reroute_stored_tub(module, pos, msg[ARR_MODULE_ID], PLANE);
			} else {
				printf("The plane is not for my tub.\n");
			}
		}

		// Iterate over all positions in the module to check if there are any stored tubs
		int new_pos = pos + 1;
		while (new_pos != pos) {

			if (new_pos == OUT) {
				new_pos = 0;
				continue;
			}

			// IF there is a tub at the position
			if (module->tub[new_pos].id != Q_NULL) {
				printf("plane_id %d at pos %d\n", module->tub[new_pos].plane_id, new_pos);
				sleep(100);
				// If that tub has to go to the plane, route it there.
				if (module->tub[new_pos].plane_id == msg[ARR_PLANE_ID]) {
					printf("I am rerouting a tub stored at %d towards %d\n", new_pos, msg[ARR_MODULE_ID]);
					reroute_stored_tub(module, new_pos, msg[ARR_MODULE_ID], PLANE);
				} else {
					printf("Plane came, but not for my tub.\n");
				}
			}
			new_pos++;
		}
		return 1;
	}
	// If the plane is leaving, remove it from the system.
	if (module->plane_to_id[msg[ARR_PLANE_ID]] != 0 && module->plane_to_id[msg[ARR_PLANE_ID]] == msg[ARR_MODULE_ID]) {
		printf("Plane %d at module %d left.\n", msg[ARR_PLANE_ID], msg[ARR_MODULE_ID]);
		module->plane_to_id[msg[ARR_PLANE_ID]] = 0;
		return 0;
	}

	printf("Something is wrong with the plane schedule.\n");
	return -1;
}

/// @brief Waits for a message of a specific type and handles it accordingly.
/// @param msg_ptr Pointer to the message to be processed.
/// @param expected The expected message type to wait for.
/// @param persistent If true, keeps waiting for messages until the expected one is received.
/// @return Returns the response code based on the message type received.
int await_message(Module* module, uint8_t** msg_ptr, int expected, bool persistent) {
	int response = Q_NULL;
	char type;
	do {
		EventType e = next_event();
		// Sift mailbox for messages:
		while (e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(msg_ptr);
			type = (*msg_ptr)[MSG_TYPE];

			printf("Type: %d, Sender:%d, Dest:%d, Tub_id: %d\n", type, (*msg_ptr)[MSG_SENDER], (*msg_ptr)[REQ_DEST_ID],
				   (*msg_ptr)[REQ_TUB_ID]);

			if (type == REQUEST_MOVEMENT) {
				// complicated decision as to whether to accept or reject the request here i guess
				response = handle_request_movement(module, *msg_ptr);
			} else if (type == REQUEST_RESPONSE) {
			response = (int)msg_ptr[MSG_VALUE];
			} else if (type == PLANE_STATUS) {
				response = handle_plane_status(module, *msg_ptr);
			} else if (type == PATH_CONFIG) {
				// response = handle_paths_config(*msg_ptr);
			} else if (type == TUB_CONFIG) {
				// response = handle_tub_config(*msg_ptr);
			}

			// If you get the message you need, return it
			if (expected == type) {
				return response;
			}
			e = next_event();
			// sleep between messages;
			sleep(50);
		}
		// sleep between checking the mailbox;
		sleep(PAUSE);
	} while (persistent);
	return response;
}

void clear_tub_data(Tub* tub) {
	tub->id = Q_NULL;
	tub->destination_id = Q_NULL;
	tub->destination_type = Q_NULL;
	tub->plane_id = Q_NULL;
}

/// @brief Sends a request for a response to a specific sender.
/// @param sender The ID of the sender to whom the request is sent.
bool await_response(Module* module) {
	printf("|| waiting for response...\n");
	uint8_t* msg;
	int response;

	for (int i = 0; i < 60; i++) {
		response = await_message(module, &msg, REQUEST_RESPONSE, true);
		if (response >= 0) {
			free(msg);
			return 1;
		}
	}
	return 0;
}

bool request_to_leave(Module* module, int next_id, char* request) {
	led_set_color(COLOR_GREEN);

	while (true) {
		printf("am requesting to module: %d\n", next_id);
		// Send 10 times or until success:
		while (send_request_movement(next_id, request) < 0) {
			printf("|| my packet got lost\n");
			sleep(100);
		}

		if (await_response(module)) {
			printf("|| got response from %d\n", next_id);
			return true;
		}
		// Timeout:
		printf("|| got no response for 1 gazillion years from %d\n", next_id);
		sleep(500);
	}
}

void save_tub_from_request(Tub* tub, char req[REQ_LENGTH]) {
	tub->id = req[REQ_TUB_ID];
	tub->destination_id = req[REQ_DEST_ID];
	tub->destination_type = req[REQ_DEST_TYPE];
	tub->plane_id = req[REQ_PLANE_ID];
	// Rest of the fields are not necessary.
}

bool do_task(Module* module) {
	Task task = module->tasks[module->task_current];
	int tub_id = task.request[REQ_TUB_ID];
	if (task.to == OUT && task.from == OUT) printf("I am doing an empty task, not good.\n");

	if (task.to == OUT) {
		printf("Tub %d to leave to module %d by %d\n", tub_id, module->next[task.from], task.from);

		int next_id = module->next[task.from];
		if (module->next[task.from] == 0) {
			clear_tub_data(&module->tub[task.from]);
			leave_at(task.from);
		} else if (request_to_leave(module, next_id, task.request)) {
			clear_tub_data(&module->tub[task.from]);
			leave_at(task.from);
		} else {
			return false;
		}
	} else if (task.from == OUT) {
		printf("Tub %d to enter module %d\n", tub_id, module->id);

		while (send_request_response(task.request[MSG_SENDER], true) < 0) {
			printf("response \\ packet loss");
			sleep(100);
		};
		save_tub_from_request(&module->tub[task.to], task.request);
		enter_at(task.to);
		printf("I am sending the response. Origin = %d\n", task.request[MSG_SENDER]);
	} else {

		if (task.request[REQ_DEST_TYPE] == SECURITY) {
			module->should_check = 1;
		}

		printf("Tub %d hits the griddy from to %d to %d\n", tub_id, task.from, task.to);
		clear_tub_data(&module->tub[task.from]);
		save_tub_from_request(&module->tub[task.to], task.request);
		move_within_module(task.from, task.to);
	}

	module->tasks[module->task_current] = EMPTY_TASK;
	module->task_current = (module->task_current + 1) % MAX_TASKS;

	// update the module's state
	if (task.from != OUT) module->state.at[task.from] = Q_NULL;
	if (task.to != OUT) module->state.at[task.to] = tub_id;
	return true;
}

void loop(Module* module) {
	if (module->task_current == module->task_new) {
		// no new tasks, wait for the next one
		uint8_t* msg;
		int response = await_message(module, &msg, REQUEST_MOVEMENT, false);

		if (response == 1) {
			printf("Destination Type: %d, Sender: %d, Tub id: %d\n", msg[REQ_DEST_TYPE], msg[MSG_SENDER],
				   msg[REQ_TUB_ID]);
		}
		if (response >= 0) {
			free(msg);
		}

	} else {
		do_task(module);
	}
}


// bool check_plane_arrived(int tub_plane_id) { return this.plane_to_id[tub_plane_id] > 0; }

/// @brief Determine the destination of a tub based on its security check status and other parameters.
/// @param sec_check_passed Indicates if the tub has passed through the security module.
/// @param sec_check_needed Indicates if a security check is needed for the tub.
/// @param plane_dropoff Indicates if the tub is going to a plane or drop-off.
/// @param plane_id ID of the plane associated with the tub.
/// @param destination Pointer to store the determined destination ID.
/// @param destination_type Pointer to store the determined destination type.
void determine_destination(Module* module, bool sec_check_passed, bool sec_check_needed, bool plane_dropoff, int plane_id,
						   int* destination, int* destination_type) {
	// If the tub needs a security check
	if (sec_check_needed) {
		// If the security check has passed, send it to quarantine; otherwise, send it to security
		if (sec_check_passed) {
			*destination_type = QUARANTINE;
			*destination = module->nearest[QUARANTINE];
		} else {
			*destination_type = SECURITY;
			*destination = module->nearest[SECURITY];
		}
		// If the tub is going to a drop-off, send it to the nearest drop-off
	} else if (plane_dropoff) {
		*destination_type = DROPOFF;
		*destination = module->nearest[DROPOFF];
		// If the tub is going to a plane and its plane has arrived, send it to it
	} else if (module->plane_to_id[plane_id] > 0) {
	    // plane arrived
		*destination_type = PLANE;
		*destination = module->plane_to_id[plane_id];
		// If the tub is going to storage, send it to the nearest storage module
	} else {
		*destination_type = STORAGE;
		*destination = module->nearest[STORAGE];
	}
	printf("reading: %d, %d\n", *destination, *destination_type);
}

/// @brief Determine the priority of a tub based on its destination type.
/// @param tub Pointer to the tub whose priority is to be determined.
void determine_priority(Tub* tub) {
	if (tub->destination_type == PLANE) {
		tub->priority = PRIO_HI;
	} else if (tub->destination_type == STORAGE) {
		tub->priority = PRIO_LO;
	} else {
		tub->priority = PRIO_ME;
	}
	printf("Tub %d is assigned priorit: %d\n", tub->id, tub->priority);
}

/// @brief Saves the data of a tub to this module.
void save_RFID_data(Module* module) {
	// tub detected
	char data[RFID_LENGTH];
	get_entrance_rfid_data(data);
	int tub_id = (int)data[TUB_ID];
	bool has_passed_security = (bool)data[PASSED_SECURITY];
	bool security_bit = (bool)data[NEEDS_SECURITY];
	bool plane_dropoff = (bool)data[PLANE_OR_DROPOFF];

	int plane_id = (int)data[PLANE_ID];
	bool plane_arrived = module->plane_to_id[plane_id];
	int tub_destination_id;
	int tub_destination_type;
	determine_destination(module, has_passed_security, security_bit, plane_dropoff, plane_id, &tub_destination_id,
						  &tub_destination_type);
	printf("I am going to destination: %d with type: %d\n", tub_destination_id, tub_destination_type);
	// this.tub[RFID] = create_tub(tub_id, has_passed_security, plane_dropoff, plane_arrived, tub_destination_id,
	// tub_destination_type, plane_id);
	module->tub[RFID] = (Tub){
		.id = tub_id,
		.passed_security = has_passed_security,
		.plane_dropoff = plane_dropoff,
		.plane_arrived = plane_arrived,
		.destination_id = tub_destination_id,
		.destination_type = tub_destination_type,
		.plane_id = plane_id,
	};
	determine_priority(&module->tub[RFID]);
}

/// @brief Detects and handles the entry of a tub into the module via RFID reader
void handle_tub_rfid_entry(Module* module) {
	// Set the LED color to cyan to indicate waiting for a tag
	led_set_color(COLOR_CYAN);

	// If the RFID detects a tag
	if (RFID_check_tag()) {
		sleep(PAUSE);
		// If the detected tag is a plane, exit out of the function as it might mean the current plane is leaving
		if (get_rfid_data(TUB_OR_PLANE) == 1) {
			return;
			// If the detected tag is a tub
		} else {
			// If the module is in storage mode, handle the storage, if there is any
			if (is_storing(*module) != Q_NULL) {
				printf("I am storing at %d\n", is_storing(*module));
				handle_storage(module);
			}

			// Save the RFID data to the module
			save_RFID_data(module);

			// Create a request message for the movement of the tub
			uint8_t request[REQ_LENGTH];
			request[MSG_SENDER] = module->id;
			request[MSG_TYPE] = REQUEST_MOVEMENT;

			request[REQ_TUB_ID] = module->tub[RFID].id;

			request[REQ_PLANE_ID] = module->tub[RFID].plane_id;
			request[REQ_PLANE_ARRIVED] = module->tub[RFID].plane_arrived;
			request[REQ_DEST_ID] = module->tub[RFID].destination_id;
			request[REQ_DEST_TYPE] = module->tub[RFID].destination_type;

			// Reroute the tub from the RFID towards its destination
			add_task(module, RFID, module->dir_lookup[module->tub[RFID].destination_id], request);
			add_task(module, module->dir_lookup[module->tub[RFID].destination_id], OUT, request);

			// Update the state of the module to indicate that the tub is at the RFID position
			module->state.at[RFID] = module->tub[RFID].id;
		}
	}
}
