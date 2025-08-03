#include "common_loop.h"
#include "defs.h"

#include "libc_builtin.h"
#include "quercus_lib_pico.h"
#include <stdint.h>

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
	printf("adding movement task: from %d, to %d, request: %s\n", from, to, request);
	Task task;
	task.to = to;
	task.from = from;

	// Copy over the request into the task
	memcpy(task.request, request, REQ_LENGTH);

	printf("movement task request: %d, %d, %d, %d\n", request[MSG_SENDER], request[REQ_TUB_ID], request[REQ_DEST_TYPE],
		   request[REQ_DEST_ID]);
	module->tasks[module->task_new] = task;
	module->task_new = (module->task_new + 1) % Q_MAX_TASKS;
}

/// @brief Take care of stored tubs in the module.
void handle_storage(Module* module) {
	// Create a request to send to the next module
	uint8_t stor_req[REQ_LENGTH];
	// Get the position of the stored tub on the module
	Direction pos_stored_tub = is_storing(*module);

	// Fill the request to send to the next module
	stor_req[MSG_SENDER] = module->id;
	stor_req[MSG_TYPE] = MSG_REQUEST_MOVEMENT;
	stor_req[REQ_TUB_ID] = module->tub[pos_stored_tub].id;
	stor_req[REQ_PLANE_ID] = module->tub[pos_stored_tub].plane_id;
	stor_req[REQ_DEST_TYPE] = DEST_STORAGE;
	stor_req[REQ_DEST_ID] = module->next[module->to_storage];

	// Reroute the tub from its initial position towards the next module
	add_task(module, pos_stored_tub, module->to_storage, stor_req);
	add_task(module, module->to_storage, DIR_OUT, stor_req);
}

/// @brief Handle a movement request.
/// @param msg The incoming message containing movement request details.
/// @return Returns a status code indicating the success or failure of the operation.
int handle_request_movement(Module* module, uint8_t* msg) {

	// Get the plane arrival status and the plane ID from the message
	bool plane_arrived = msg[REQ_PLANE_ARRIVED];
	uint8_t plane = msg[REQ_PLANE_ID];

	// Reroute tub if its plane has arrived
	if (!plane_arrived && module->plane_to_module_id[plane] != 0 && msg[REQ_DEST_TYPE] == DEST_STORAGE) {
		msg[REQ_DEST_ID] = module->plane_to_module_id[plane];
		msg[REQ_PLANE_ARRIVED] = 1;
		msg[REQ_DEST_TYPE] = DEST_PLANE;
	}

	// Get the sender and destination IDs from the message
	uint8_t sender_id = msg[MSG_SENDER];
	uint8_t destination_id = module->id_lookup[msg[REQ_DEST_ID]];

	// Check if the destination id is valid
	if (destination_id < 1) {
		printf("Bad request received: %d\n", destination_id);
		return -1;
	}

	Direction from;
	Direction to;

	// Determine the beginning and final positions of the tub
	for (int i = 0; i < 3; i++) {
		if (module->next[i] == sender_id) {
			from = i;
		}
		if (module->next[i] == destination_id) {
			to = i;
		}
	}

	// If the sender is this module, set the source position to DIR_RFID
	if (sender_id == module->id) {
		from = DIR_RFID;
	}

	// If the current module is storage and if a tub is being stored
	if (module->is_storage && is_storing(*module) != Q_NULL) {
		printf("I will handle storage\n");
		// Handle the stored tub
		handle_storage(module);
	}

	// Receive tub at one of your endpoints:
	add_task(module, DIR_OUT, from, msg);

	// If the destination is the current module
	if (destination_id == module->id) {
		// If the destination type is storage and the current module is a storage module
		if (msg[REQ_DEST_TYPE] == DEST_STORAGE && module->is_storage) {
			// Store the tub at the position it is at
			printf("Saving plane id %d to position %d\n", plane, from);
			module->tub[from].plane_id = plane;
		} else if (msg[REQ_DEST_TYPE] == DEST_SECURITY) {
			// If the destination type is security, send the tub to the DIR_RFID
			add_task(module, from, DIR_RFID, msg);
		} else {
			// The tub should exit the system
			add_task(module, from, DIR_RFID, msg);
			add_task(module, DIR_RFID, DIR_OUT, msg); // assumption: only rfid sides can be entries and exits
		}
		return 0;
	}
	// If the destination is not the current module, route the tub to the next module
	add_task(module, from, to, msg);
	add_task(module, to, DIR_OUT, msg);

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
	request[MSG_TYPE] = MSG_REQUEST_MOVEMENT;
	request[REQ_TUB_ID] = module->state.at[from];
	request[REQ_DEST_TYPE] = dest_type;
	request[REQ_DEST_ID] = destination_id;

	// Determine the direction to the destination
	Direction to = module->dir_lookup[module->id_lookup[destination_id]];

	// Reroute the tub from its initial position towards the next module
	add_task(module, from, to, request);
	add_task(module, to, DIR_OUT, request);
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
		return -1;
	}
	uint8_t arrived_plane_id = msg[ARR_PLANE_ID];
	// If the plane has not been saved in the system yet
	if (module->plane_to_module_id[arrived_plane_id] == 0) {
		// Save that the plane is coming.
		module->plane_to_module_id[arrived_plane_id] = msg[ARR_MODULE_ID];
		printf("I got a plane update: plane %d landed on %d with departure time %d\n", arrived_plane_id,
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
			if (module->tub[pos].plane_id == arrived_plane_id) {
				printf("I am rerouting a tub stored at direction towards the plane to it.\n");
				// If that tub has to go to the plane, route it there.
				reroute_stored_tub(module, pos, msg[ARR_MODULE_ID], DEST_PLANE);
			} else {
				printf("The plane is not for my tub.\n");
			}
		}

		// Iterate over all positions in the module to check if there are any stored tubs
		int new_pos = pos + 1;
		while (new_pos != pos) {

			if (new_pos == DIR_OUT) {
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
					reroute_stored_tub(module, new_pos, msg[ARR_MODULE_ID], DEST_PLANE);
				} else {
					printf("Plane came, but not for my tub.\n");
				}
			}
			new_pos++;
		}
		return 1;
	}
	// If the plane is leaving, remove it from the system.
	if (module->plane_to_module_id[msg[ARR_PLANE_ID]] != 0 &&
		module->plane_to_module_id[msg[ARR_PLANE_ID]] == msg[ARR_MODULE_ID]) {
		printf("Plane %d at module %d left.\n", msg[ARR_PLANE_ID], msg[ARR_MODULE_ID]);
		module->plane_to_module_id[msg[ARR_PLANE_ID]] = 0;
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
int await_message(Module* module, uint8_t** msg_ptr_ptr, int expected, bool persistent) {
	int response = Q_NULL;
	uint8_t type;
	do {
		EventType e = next_event();
		// Sift mailbox for messages:
		if (e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(msg_ptr_ptr);
			// msg_ptr_ptr -> msg_string -> first byte of msg
			uint8_t* msg_string = *msg_ptr_ptr;
			type = msg_string[MSG_TYPE];

			printf("Type: %d, Sender:%d, Dest:%d, Tub_id: %d\n", type, msg_string[MSG_SENDER], msg_string[REQ_DEST_ID],
				   msg_string[REQ_TUB_ID]);

			if (type == MSG_REQUEST_MOVEMENT) {
				// complicated decision as to whether to accept or reject the request
				response = handle_request_movement(module, msg_string);
			} else if (type == MSG_REQUEST_RESPONSE) {
				// simply return the value for request_response messages
				response = msg_string[MSG_VALUE];
			} else if (type == MSG_PLANE_STATUS) {
				response = handle_plane_status(module, msg_string);
			} else if (type == MSG_PATH_CONFIG) {
				// unimplemented
			} else if (type == MSG_TUB_CONFIG) {
				// unimplemented
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
		sleep(TIME_PAUSE);
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
		response = await_message(module, &msg, MSG_REQUEST_RESPONSE, true);
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
	if (task.to == DIR_OUT && task.from == DIR_OUT) {
		printf("I am doing an empty task, not good.\n");
	}

	if (task.to == DIR_OUT) {
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
	} else if (task.from == DIR_OUT) {
		printf("Tub %d to enter module %d\n", tub_id, module->id);

		while (send_request_response(task.request[MSG_SENDER], true) < 0) {
			printf("response \\ packet loss");
			sleep(100);
		};
		save_tub_from_request(&module->tub[task.to], task.request);
		enter_at(task.to);
		printf("I am sending the response. Origin = %d\n", task.request[MSG_SENDER]);
	} else {

		if (task.request[REQ_DEST_TYPE] == DEST_SECURITY) {
			module->should_check = 1;
		}

		printf("Tub %d hits the griddy from to %d to %d\n", tub_id, task.from, task.to);
		clear_tub_data(&module->tub[task.from]);
		save_tub_from_request(&module->tub[task.to], task.request);
		move_within_module(task.from, task.to);
	}

	module->tasks[module->task_current] = EMPTY_TASK;
	module->task_current = (module->task_current + 1) % Q_MAX_TASKS;

	// update the module's state
	if (task.from != DIR_OUT) module->state.at[task.from] = Q_NULL;
	if (task.to != DIR_OUT) module->state.at[task.to] = tub_id;
	return true;
}

void loop(Module* module) {
	if (module->task_current == module->task_new) {
		// no new tasks, wait for the next one
		uint8_t* msg;
		int response = await_message(module, &msg, MSG_REQUEST_MOVEMENT, false);

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
