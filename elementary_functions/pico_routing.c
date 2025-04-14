#pragma once
#include "pico_basis.c"

/// @brief Take care of stored tubs in the module.
void handle_storage() {
	// Create a request to send to the next module
	char stor_req[REQ_LENGTH];
	// Get the position of the stored tub on the module
	Direction pos_stored_tub = is_storing();
	
	// Fill the request to send to the next module
	stor_req[MSG_SENDER] = this.id;
	stor_req[MSG_TYPE] = REQUEST_MOVEMENT;
	stor_req[REQ_TUB_ID] = this.tub[pos_stored_tub].id;
	stor_req[REQ_PLANE_ID] = this.tub[pos_stored_tub].plane_id;
	stor_req[REQ_DEST_TYPE] = STORAGE;
	stor_req[REQ_DEST_ID] = this.next[this.to_storage];
	
	// Reroute the tub from its initial position towards the next module
	add_task(pos_stored_tub, this.to_storage, stor_req);
	add_task(this.to_storage, OUT, stor_req);

}

/// @brief Handles routing logic from a source 'from' to a specified destination 'destination_id'.
/// @param from The source from which the routing process begins.
/// @param destination_id The unique identifier of the destination to route to.
void reroute_stored_tub(Direction from, uint8_t destination_id, DestinationType dest_type) {
	// Create and fill the request to send to the next module
	char request[REQ_LENGTH];
	request[MSG_SENDER] = this.id;
	request[MSG_TYPE] = REQUEST_MOVEMENT;
	request[REQ_TUB_ID] = state.at[from];
	request[REQ_DEST_TYPE] = dest_type;
	request[REQ_DEST_ID] = destination_id;

	// Determine the direction to the destination
	Direction to = this.dir_lookup[this.id_lookup[destination_id]];

	// Reroute the tub from its initial position towards the next module
	add_task(from, to, request);
	add_task(to, OUT, request);

}



/// @brief Library for handling routing in the Quercus decentralized application.
/// @param msg The incoming message containing movement request details.
/// @return Returns a status code indicating the success or failure of the operation.
int handle_request_movement(char* msg) {
	
	// Get the plane arrival status and the plane ID from the message
	int plane_arrived = msg[REQ_PLANE_ARRIVED];
	int plane = msg[REQ_PLANE_ID];

	// Reroute tub if its plane has arrived
	if (!plane_arrived && this.plane_to_id[plane] != 0 && msg[REQ_DEST_TYPE] == STORAGE) {
		msg[REQ_DEST_ID] = this.plane_to_id[plane];
		msg[REQ_PLANE_ARRIVED] = 1;
		msg[REQ_DEST_TYPE] = PLANE;
	}

	// Get the sender and destination IDs from the message
	int origin = msg[MSG_SENDER];
	int end = this.id_lookup[msg[REQ_DEST_ID]];

	// Check if the sender id is valid
	if(end < 1) {
		printf("Bad request received: %d\n", end);
		return -1;
	} 

	Direction from;
	Direction to;
	
	// Determine the beginning and final positions of the tub
	for (int i = 0; i < 3; i++) {
		if (this.next[i] == origin) from = i;
		if (this.next[i] == end) to = i;
	}

	// If the sender is this module, set the source position to RFID
	if(origin == this.id) from = RFID;

	// If the current module is storage and if a tub is being stored
	if (this.is_storage && is_storing() != NON) {
		printf("I will handle storage\n");
		// Handle the stored tub
		handle_storage();
	}

	// Receive tub at one of your endpoints:
	add_task(OUT, from, msg);

	// If the destination is the current module
	if (end == this.id) {
		// If the destination type is storage and the current module is a storage module
		if(msg[REQ_DEST_TYPE] == STORAGE && this.is_storage) {
			// Store the tub at the position it is at
			printf("Saving plane id %d to position %d\n", plane, from);
			this.tub[from].plane_id = plane;
		} else if (msg[REQ_DEST_TYPE] == SECURITY) {
			// If the destination type is security, send the tub to the RFID
			add_task(from, RFID, msg);
		} else {
			// The tub should exit the system
			add_task(from, RFID, msg);
			add_task(RFID, OUT, msg);
		}
		return 0;
	}
	// If the destination is not the current module, route the tub to the next module
	add_task(from, to, msg);
	add_task(to, OUT, msg);

	return 0;
}

/// @brief Handles the response to a request movement message.
int handle_request_response(char* msg) { 
	return msg[MSG_VALUE]; 
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
int handle_plane_status(char* msg) {
	// If the message sender is not the Pi
	if (msg[MSG_SENDER] != 0) {
		return NON;
	}
	// If the plane has not been saved in the system yet
	if(this.plane_to_id[msg[ARR_PLANE_ID]] == 0) {
		// Save that the plane is coming.
		this.plane_to_id[msg[ARR_PLANE_ID]] = msg[ARR_MODULE_ID];
		printf("I got a plane update: plane %d landed on %d with departure time %d\n", msg[ARR_PLANE_ID], msg[ARR_MODULE_ID], msg[ARR_DEP_TIME]);
		
		// If the module is busy, it will not handle the plane.
		if(!no_tasks()) {
			printf("I am doing something, the next module will deal with the plane.\n");
			return 2;
		}
		// Save the direction towards the plane
		int pos = this.dir_lookup[msg[ARR_MODULE_ID]];
		
		// If a tub is stored at the position towards the plane
		if(this.tub[pos].id != NON){
			// If that tub has to go to the plane, route it there.
			if(this.tub[pos].plane_id == msg[ARR_PLANE_ID]){
				printf("I am rerouting a tub stored at direction towards the plane to it.\n");
				// If that tub has to go to the plane, route it there.
				reroute_stored_tub(pos, msg[ARR_MODULE_ID], PLANE);
			}
			else{
				printf("The plane is not for my tub.\n");
			}
		}

		// Iterate over all positions in the module to check if there are any stored tubs
		int new_pos = pos+1;
		while(new_pos != pos){
			
			if(new_pos == OUT) {
				new_pos = 0;
				continue;
			}

			// IF there is a tub at the position
			if(this.tub[new_pos].id != NON){
				printf("plane_id %d at pos %d\n", this.tub[new_pos].plane_id, new_pos);
				sleep(100);
				// If that tub has to go to the plane, route it there.
				if(this.tub[new_pos].plane_id == msg[ARR_PLANE_ID]){
					printf("I am rerouting a tub stored at %d towards %d\n", new_pos, msg[ARR_MODULE_ID]);
					reroute_stored_tub(new_pos, msg[ARR_MODULE_ID], PLANE);
				}
				else{
					printf("Plane came, but not for my tub.\n");
				}
			}
			new_pos++;
		}
		return 1;
	}
	// If the plane is leaving, remove it from the system.
	if (this.plane_to_id[msg[ARR_PLANE_ID]] != 0 && this.plane_to_id[msg[ARR_PLANE_ID]] == msg[ARR_MODULE_ID]){
		printf("Plane %d at module %d left.\n", msg[ARR_PLANE_ID], msg[ARR_MODULE_ID]);
		this.plane_to_id[msg[ARR_PLANE_ID]] = 0;
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
int await_message(char** msg_ptr, int expected, bool persistent) {
	int response = NON;
	char type;
	do {
		EventType e = next_event();
		// Sift mailbox for messages:
		while (e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(msg_ptr);
			type = (*msg_ptr)[MSG_TYPE];

			printf("Type: %d, Sender:%d, Dest:%d, Tub_id: %d\n", type, (*msg_ptr)[MSG_SENDER],
				   (*msg_ptr)[REQ_DEST_ID], (*msg_ptr)[REQ_TUB_ID]);

			if (type == REQUEST_MOVEMENT) {
				// complicated decision as to whether to accept or reject the request here i guess
				response = handle_request_movement(*msg_ptr);
			} else if (type == REQUEST_RESPONSE) {
				response = handle_request_response(*msg_ptr);
			} else if (type == PLANE_STATUS) {
				response = handle_plane_status(*msg_ptr);
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
			//sleep between messages;
			sleep(50);
		}
		//sleep between checking the mailbox;
		sleep(PAUSE);
	} while (persistent);
	return response;
}

/// @brief Sends a request for a response to a specific sender.
/// @param sender The ID of the sender to whom the request is sent.
bool await_response() {
	printf("|| waiting for response...\n");
	char* msg;
	int response;
	
	for (int i = 0; i < 60; i++){
		response = await_message(&msg, REQUEST_RESPONSE, true);
		if (response >= 0) {
			free(msg);
			return 1;
		}
	}
	return 0;
}

int await_request_movement() {
	char* msg;
	int response = await_message(&msg, REQUEST_MOVEMENT, false);

	if (response == 1) {
		printf("Destination Type: %d, Sender: %d, Tub id: %d\n", msg[REQ_DEST_TYPE], msg[MSG_SENDER], msg[REQ_TUB_ID]);
	}
	if (response >= 0){
		free(msg);
	}
	return response;
}


/*
	ACTUALLY DO STUFF
*/

bool request_to_leave(int next_id, char* request) {
	led_set_color(LED_GREEN);

	while(true) {
		printf("am requesting to module: %d\n", next_id);
		// Send 10 times or until success:
		while(send_request_movement(next_id, request) < 0) {
			printf("|| my packet got lost\n");
			sleep(100);
		}

		if (await_response()) {
			printf("|| got response from %d\n", next_id);
			return true;
		}
		// Timeout:
		printf("|| got no response for 1 gazillion years from %d\n", next_id);
		sleep(500);
	}
}

void clear_tub_data(Tub* tub){
	tub->id = NON;
	tub->destination_id = NON;
	tub->destination_type = NON;
	tub->plane_id = NON;
}

void save_tub_from_request(Tub* tub, char req[REQ_LENGTH]){
	tub->id = req[REQ_TUB_ID];
	tub->destination_id = req[REQ_DEST_ID];
	tub->destination_type = req[REQ_DEST_TYPE];
	tub->plane_id = req[REQ_PLANE_ID];
	//Rest of the fields are not necessary.
}

bool do_task() {
	Task task = tasks[task_current];
	int tub_id = task.request[REQ_TUB_ID];
	if (task.to == OUT && task.from == OUT) printf("I am doing an empty task, not good.\n");

	if (task.to == OUT) {
		printf("Tub %d to leave to module %d by %d\n", tub_id, this.next[task.from], task.from);

		int next_id = this.next[task.from];
		if(this.next[task.from] == 0) {
			clear_tub_data(&this.tub[task.from]);
			leave_at(task.from);
		} else if (request_to_leave(next_id, task.request)) {
			clear_tub_data(&this.tub[task.from]);
			leave_at(task.from);
		} else {
			return false;
		}
	} else if (task.from == OUT) {
		printf("Tub %d to enter module %d\n", tub_id, this.id);
		
		while(send_request_response(task.request[MSG_SENDER], true) < 0) {
			printf("response \\ packet loss");
			sleep(100);
		};
		save_tub_from_request(&this.tub[task.to], task.request);
		enter_at(task.to);
		printf("I am sending the response. Origin = %d\n", task.request[MSG_SENDER]);
	} else {
		
		if(task.request[REQ_DEST_TYPE] == SECURITY) {
			this.should_check = 1;
		}

		printf("Tub %d hits the griddy from to %d to %d\n", tub_id, task.from, task.to);
		clear_tub_data(&this.tub[task.from]);
		save_tub_from_request(&this.tub[task.to], task.request);
		move_within_module(tub_id, task.from, task.to);
	}

	tasks[task_current] = empty;
	task_current = (task_current + 1) % MAX_TASKS;

	update_state(task.from, task.to, task.request[REQ_TUB_ID]);
	return true;

}


void loop() {
	if (no_tasks()) {
		await_request_movement();
	} else {
		do_task();
	}
}