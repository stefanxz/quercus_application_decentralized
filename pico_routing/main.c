#include <stdbool.h>
#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../functions/movement.c"
#include "../functions/rfid.c"
#include "../functions/algorithm.c"

Module this;
State state;
char current_request[MSG_HEAD + RFID_LENGTH];

void init(Module* mod) {
	Task empty = {.from = OUT, .to = OUT};
    // Initialize the module's state
    state.at[RFID] = -1;
    state.at[LASER_LEFT] = -1;
    state.at[LASER_RIGHT] = -1;

	this.current = mod->current;
	this.next_free = mod->next_free;

    this.id = mod->id;
    this.is_storage = mod->is_storage;

    for (int i = 0; i < MAX_NUMBER_OF_PLANES; i++) {
		this.id_lookup[i] = mod->id_lookup[i];
	}

	for (int i = 0; i < MAX_NUMBER_OF_PLANES; i++) {
		this.plane_to_id[i] = 0;
	}
	
	for (int i = 0; i < 7; ++i) {
		this.tasks[i] = empty;
	}

	this.next[0] = mod->next[0];
	this.next[1] = mod->next[1];
	this.next[2] = mod->next[2];
}

void handle_storage(Direction from, Direction to) {
	// If RFID is empty, move tub to side of RFID
	if (from < RFID && to < RFID && state.at[RFID] == -1) {
		if(state.at[from] != -1) {
			add_task(&this, from, RFID, current_request);
		} else if (state.at[to] != -1) {
			add_task(&this, to, RFID, current_request);
		} else {
			return;
		}
	} else {
		// If RFID is full or needs to be passed through, move tub to adjacent storage module.
		if (state.at[from] != -1) {
			add_task(&this, from, this.next_storage, current_request);
			add_task(&this, this.next_storage, OUT, current_request);
		} else if (state.at[to] != -1) {
			add_task(&this, to, this.next_storage, current_request);
			add_task(&this, this.next_storage, OUT, current_request);
		} else {
			return;
		}
	}
}

void handle_request() {
	int plane_arrived = current_request[MSG_HEAD + PLANE_ARRIVED];
	int plane_id = current_request[MSG_HEAD + PLANE_ID];

	// Reroute tub if its plane has arrived
	if(!plane_arrived && this.plane_to_id[plane_id] != 0) {
		current_request[MSG_HEAD + DESTINATION]= this.plane_to_id[plane_id];
		current_request[MSG_HEAD + PLANE_ARRIVED] = 1;
	}

	int origin = current_request[SENDER];
	int end = this.id_lookup[(int) current_request[DESTINATION]];

	Direction from;
	Direction to;

	for(int i = 0; i < 3; i++) {
		if(this.next[i] == origin) {
			from = i;
		}

		if(this.next[i] == end) {
			to = i;
		}
	}

	if (this.is_storage) {
		handle_storage(from, to);
	}

	// Add logic for more complicated scheduling here:
	// TODO: Implement not always responding with a go-ahead to a request
	printf("I am sending the response. Origin = %d\n", origin);
	send_request_response(origin, 1);
	
	// Receive tub at one of your endpoints:
	add_task(&this, OUT, from, current_request);
	
	// if (end != this.id) {
		// If the tub is not for you, send it to the next module.
	add_task(&this, from, to, current_request);
	add_task(&this, to, OUT, current_request);
	// }
}
	

void loop() {
	if(no_tasks(&this)) { 
		// printf("No tasks.\n");
		if (get_request(&current_request)) handle_request();
		sleep(1000);
	} else{
		do_task(&this);
	}
}

// export int main(void) {
// 	subscribe_to_event(EVENT_MESSAGE_RECEIVED);
// 	for (int i = 0; i < 100000000; i++) {
// 		loop();
// 	}
// }
