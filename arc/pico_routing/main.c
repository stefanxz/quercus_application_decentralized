#include "../libc_builtin.h"
#include "../quercus_lib_pico.h"
#include <stdbool.h>

#include "../functions/algorithm.c"
#include "../functions/movement.c"
#include "../functions/rfid.c"

Module this;
State state;
char current_request[REQ_LENGTH];

void init(Module* mod) {
	Task empty = {.from = OUT, .to = OUT};

	// Initialize the module's state
	state.at[RFID] = NON;
	state.at[LASER_LEFT] = NON;
	state.at[LASER_RIGHT] = NON;

	this.current = mod->current;
	this.next_free = mod->next_free;

	this.security_id = mod->security_id;
	this.quarantine_id = mod->quarantine_id;
	this.storage_id = mod->storage_id;
	this.dropoff_id = mod->dropoff_id;

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

	this.next_storage = mod->next_storage;
}

void handle_storage(Direction from) {
	if(state.at[this.next_storage] != NON) {
		printf("HEEEAYAYAYAAY, %d\n", this.next_storage);
		add_task(&this, this.next_storage, OUT, current_request);	
	}
	if(state.at[from] != NON) {
		printf("WHATS GOING ON %d\n", this.next_storage);
		add_task(&this, from, this.next_storage, current_request);
	}

	// If RFID is empty, move tub to side of RFID
	// if (from < RFID && to < RFID && state.at[RFID] == NON) {
	// 	printf("RFID is free.\n");
	// 	//STINKY asf
	// 	if (state.at[from] != NON) {
	// 		printf("I am villainous: %d\n", from);
	// 		add_task(&this, from, RFID, current_request);
	// 	} else if (state.at[to] != NON) {
	// 		printf("I am evil: %d\n", to);
	// 		add_task(&this, to, RFID, current_request);
	// 	} else {
	// 		return;
	// 	}
	// } else {
	// 	// If RFID is full or needs to be passed through, move tub to adjacent storage module.
	// 	state.is_storing = false;
	// 	if (state.at[from] != NON) {
	// 		printf("nyahahaha: %d, %d\n", from, this.next_storage);
	// 		add_task(&this, from, this.next_storage, current_request);
	// 		add_task(&this, this.next_storage, OUT, current_request);
	// 	} else if (state.at[to] != NON) {
	// 		printf("muhahaha: %d, %d\n", to, this.next_storage);
	// 		add_task(&this, to, this.next_storage, current_request);
	// 		add_task(&this, this.next_storage, OUT, current_request);
	// 	} else {
	// 		printf("I should really not be here\n");
	// 		return;
	// 	}
	// }
}

void handle_request() {
	int plane_arrived = current_request[REQ_PLANE_ARRIVED];
	int plane = current_request[REQ_PLANE_ID];

	// Reroute tub if its plane has arrived
	if (!plane_arrived && this.plane_to_id[plane] != 0) {
		current_request[REQ_DEST_ID] = this.plane_to_id[plane];
		current_request[REQ_PLANE_ARRIVED] = 1;
	}

	int origin = current_request[MSG_SENDER];
	int end = this.id_lookup[current_request[REQ_DEST_ID]];

	Direction from;
	// TODO: Look into this
	Direction to;

	for (int i = 0; i < 3; i++) {
		if (this.next[i] == origin) {
			from = i;
		}

		if (this.next[i] == end) {
			to = i;
		}
	}

	handle_storage(from);

	// Add logic for more complicated scheduling here:
	// TODO: Implement not always responding with a go-ahead to a request
	printf("I am sending the response. Origin = %d\n", origin);

	// Receive tub at one of your endpoints:
	add_task(&this, OUT, from, current_request);

	if(end == this.id) {
		printf("sad griddy :( \n");
		state.is_storing = true;
		return;
	}
	add_task(&this, from, to, current_request);
	add_task(&this, to, OUT, current_request);
}

void update_state() {
	int from = this.tasks[this.current].from;
	int to = this.tasks[this.current].to;

	if(from != OUT) state.at[from] = NON;
	if(to != OUT) state.at[to] = this.tasks[this.current].request[REQ_TUB_ID];
	printf("state: %d, %d, %d\n", state.at[0], state.at[1], state.at[2]);
}

void loop() {
	if (no_tasks(&this)) {
		// printf("No tasks.\n");
		if (await_request(&this, &current_request)) handle_request();
		sleep(200);
	} else {
		update_state(&this);
		do_task(&this);
	}
}