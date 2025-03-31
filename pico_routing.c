#pragma once
#include "pico_basis.c"

/*
	STORAGE LOGIC
*/
void handle_storage(Direction from, char* msg) {
	if (state.at[this.to_storage] != NON) {
		add_task(this.to_storage, OUT, msg);
	}
	if (state.at[from] != NON) {
		add_task(from, this.to_storage, msg);
	}
}

void reroute_stored_tub(Direction from, uint8_t destination_id) {
	printf("I am rerouting\n");
	char request[REQ_LENGTH];
	request[MSG_SENDER] = this.id;
	request[MSG_TYPE] = REQUEST_MOVEMENT;
	request[REQ_TUB_ID] = state.at[from];
	request[REQ_DEST_TYPE] = PLANE;
	request[REQ_DEST_ID] = destination_id;
	Direction to;
	to = this.dir_lookup[this.id_lookup[destination_id]];
	add_task(from, to, request);
	add_task(to, OUT, request);
}

/*
	NETWORK TOP LAYER SHIT - HANDLE REQUESTS, ECT.
*/


int handle_request_movement(char* msg) {
	int plane_arrived = msg[REQ_PLANE_ARRIVED];
	int plane = msg[REQ_PLANE_ID];

	// Reroute tub if its plane has arrived
	if (!plane_arrived && this.plane_to_id[plane] != 0) {
		msg[REQ_DEST_ID] = this.plane_to_id[plane];
		msg[REQ_PLANE_ARRIVED] = 1;
	}

	int origin = msg[MSG_SENDER];
	int end = this.id_lookup[msg[REQ_DEST_ID]];
	if(end < 1) {
		printf("skibidi request received: %d\n", end);
		return -1;
	} 

	Direction from;
	Direction to;

	for (int i = 0; i < 3; i++) {
		if (this.next[i] == origin) from = i;
		if (this.next[i] == end) to = i;
	}

	if (this.is_storage && state.is_storing) {
		handle_storage(from, msg);
	}

	// Receive tub at one of your endpoints:
	add_task(OUT, from, msg);

	if (end == this.id) {
		if(msg[REQ_DEST_TYPE] == STORAGE && this.is_storage) {
			state.is_storing = true;
			this.tub.plane_id = plane;
		} else if (msg[REQ_DEST_TYPE] == PLANE && msg[REQ_DEST_ID] == this.id){
			add_task(from, RFID, msg);
			add_task(RFID, OUT, msg);
		}
		return 0;
	}
	add_task(from, to, msg);
	add_task(to, OUT, msg);

	// TODO: Implement not always giving the go-ahed
	return 0;
}

int handle_request_response(char* msg) { 
	return msg[MSG_VALUE]; 
}

int handle_plane_status(char* msg) {
	if (msg[MSG_SENDER] != 0) {
		return NON;
	}
	if(this.plane_to_id[msg[ARR_PLANE_ID]] == 0) {
		this.plane_to_id[msg[ARR_PLANE_ID]] = msg[ARR_MODULE_ID];
		printf("I got a plane update, %d, %d\n", msg[ARR_PLANE_ID], msg[ARR_MODULE_ID]);
		//HEAVILY RELIES ON ONLY A SINGLE TUB ON MODULE/IN SYSTEM
		Direction from;
		if(is_storing()) {
			for(int i =0; i < 3; i++){
				if(state.at[i] > 0){
					from = i;
					break;
				}
			}
			if(this.tub.plane_id == msg[ARR_PLANE_ID]){
				reroute_stored_tub(from, msg[ARR_MODULE_ID]);
			}
		}
	} else if (this.plane_to_id[msg[ARR_PLANE_ID]] != 0 && this.plane_to_id[msg[ARR_PLANE_ID]] == msg[ARR_MODULE_ID]){
		printf("Destroy me daddy\n");
		this.plane_to_id[msg[ARR_PLANE_ID]] = 0;
		//More logic to handle planes leaving might be needed.
	} else {
		printf("Something is wrong with the plane schedule.\n");
	}
	return 0;
}

int handle_paths_config(char* msg) {
	this.next[LASER_LEFT] = msg[CON_LASER_LEFT];
	this.next[LASER_RIGHT] = msg[CON_LASER_RIGHT];
	this.next[RFID] = msg[CON_RFID];
	return 0;
}

int handle_tub_config(char* msg) {
	// return set_tub_id(msg[2]);
	return 0;
}

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

			sleep(PAUSE);
		}
	} while (persistent);
	return response;
}

int await_response() {
	printf("I am waiting for a response\n");
	char* msg;
	int response = await_message(&msg, REQUEST_RESPONSE, true);
	if (response >= 0) {
		free(msg);
		return 1;
	} else {
		return 0;
	}
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

	printf("I am sending a request to module: %d\n", next_id);
	// Send 10 times or until success:
	for (int i = 0; i < 10 && send_request_movement(next_id, request) < 0; i++) {
		printf("move // packet loss\n");
		sleep(100);
	}

	return await_response();
}

bool do_task() {
	Task task = tasks[task_current];
	int tub_id = task.request[REQ_TUB_ID];
	if (task.to == OUT && task.from == OUT) printf("We are doing an empty task, fml\n");

	if (task.to == OUT) {
		printf("Tub %d to leave to module %d by %d\n", tub_id, this.next[task.from], task.from);

		int next_id = this.next[task.from];
		if(this.next[task.from] == 0) {
			leave_at(task.from);
		} else if (request_to_leave(next_id, task.request)) {
			leave_at(task.from);
		} else {
			return false;
		}
	} else if (task.from == OUT) {
		printf("Tub %d to enter module %d\n", tub_id, this.id);
		send_request_response(task.request[MSG_SENDER], true);
		enter_at(task.to);
		printf("I am sending the response. Origin = %d\n", task.request[MSG_SENDER]);
	} else {
		printf("Tub %d hits the griddy from to %d to %d\n", tub_id, task.from, task.to);
		move_within_module(tub_id, task.from, task.to);
	}

	tasks[task_current] = empty;
	task_current = (task_current + 1) % MAX_TASKS;

	update_state(task.from, task.to, task.request[REQ_TUB_ID]);
	printf("current: %d, next_free: %d\n", task_current, task_new);
	return true;
}


void loop() {
	if (no_tasks()) {
		await_request_movement();
	} else {
		do_task();
	}
}