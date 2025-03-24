// #include <stdbool.h>
// #include "../quercus_lib_pico.h"
// #include "../libc_builtin.h"

// #include "../functions/movement.c"
// #include "../functions/rfid.c"
// #include "../algo_functions.c"

// Module this;
// Request current_request;

// int next[3];
// bool is_storage;

// void init(Module* mod) {
// 	this.current = mod->current;
// 	this.next_free = mod->next_free;

// 	this.next[0] = mod->next[0];
// 	this.next[1] = mod->next[1];
// 	this.next[2] = mod->next[2];	
// }

// void handle_storage(Direction from, Direction to) {
// 	// If RFID is empty, move tub to side of RFID
// 	if (from < RFID && to < RFID && state.at[RFID] == -1) {
// 		if(state.at[from] != -1) {
// 			add_task(from, RFID, &current_request);
// 		} else if (state.at[to] != -1) {
// 			add_task(to, RFID, current_request);
// 		} else {
// 			return;
// 		}
// 	} else {
// 		// If RFID is full or needs to be passed through, move tub to adjacent storage module.
// 		if (state.at[from] != -1) {
// 			add_task(from, this.next_storage, current_request);
// 			add_task(this.next_storage, OUT, current_request);
// 		} else if (state.at[to] != -1) {
// 			add_task(to, this.next_storage, current_request);
// 			add_task(this.next_storage, OUT, current_request);
// 		} else {
// 			return;
// 		}
// 	}
// }

// void handle_request() {
// 	// Reroute tub if it's plane has arrived
// 	if(!current_request.plane_arrived && this.plane_to_id[current_request.plane_id] != 0) {
// 		current_request.destination = this.plane_to_id[current_request.plane_id];
// 		current_request.plane_arrived = true;
// 	}

// 	int origin = current_request.sender_id;
// 	int end = this.lookup[current_request.destination];

// 	Direction from;
// 	Direction to;
// 	for(int i = 0; i < 3; i++) {
// 		if(next[i] == origin) {
// 			from = i;
// 		}

// 		if(next[i] == end) {
// 			to = i;
// 		}
// 	}

// 	if (this.is_storage) {
// 		handle_storage(from, to);
// 	}

// 	// Add logic for more complicated scheduling here:


// 	// Receive tub at one of your endpoints:
// 	add_task(from, to, current_request);
// 	if (end != this.id) {
// 		// If the tub is not for you, send it to the next module.
// 		add_task(to, OUT, current_request);
// 		add_task(OUT, from, current_request);
// 	}
// }

// void get_request() {
// 	enum EventType e;
// 	while ((e = next_event())) {
// 		char* msg;
// 		if(e == EVENT_MESSAGE_RECEIVED) {
// 			handle_message(&msg, 0);
// 		} else {
// 			// Todo: rewire logic
// 			return;
// 		}
// 	}
// }
	

// void loop() {
// 	if(tasks[current].from == OUT && tasks[current].to == OUT) { 
// 		get_request();
// 		handle_request();
// 	}
// 	do_task(&tasks[current]);

// 	sleep(50);
// }

// export int main(void) {
// 	for (int i = 0; i < 100000000; i++)
// 	{
// 		loop();
// 	}
// }
