#include <stdbool.h>
#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../elementary_functions/movement_functions.c"
#include "../elementary_functions/rfid_functions.c"
#include "../algo_functions.c"

int SECOND = 1000000;

Request current_request;

Task tasks[10];
int current;
int next_free;

int next[3];

// Unused for now
State projected_state;

int planes[MAX_NUMBER_OF_PLANES];
int lookup[MAX_NUMBER_OF_MODULES];

void init(int my_id) {
	state.at[LASER_LEFT] = -1;
	state.at[LASER_RIGHT] = -1;
	state.at[RFID] = -1;

	for(int i = 0; i < MAX_NUMBER_OF_PLANES; i++) {
		planes[i] = 0;
	}

	for(int i = 0; i < MAX_NUMBER_OF_MODULES; i++) {
		lookup[i] = 0;
	}
}


void handle_request() {
	// Reroute tub if it's plane has arrived
	if(!current_request.plane_arrived && planes[current_request.plane_id] != 0) {
		current_request.destination = planes[current_request.plane_id];
		current_request.plane_arrived = true;
	}

	int origin = current_request.origin_module;
	int end = lookup[current_request.destination];

	Direction from;
	Direction to;
	for(int i = 0; i < 3; i++) {
		if(next[i] == origin) {
			from = i;
		}

		if(next[i] == end) {
			to = i;
		}
	}

	add_task(OUT, from, current_request);
	add_task(from, to, current_request);
	add_task(to, OUT, current_request);
}

void get_request() {
	enum EventType e;
	while ((e = next_event())) {
		char* msg;
		if(e == EVENT_MESSAGE_RECEIVED) {
			handle_message(&msg, 0);
		} else {
			// Todo: rewire logic
			return;
		}
	}
}
	

void loop() {
	if(tasks[current].move == IDLE) {
		get_request();
		handle_request();
	}
	do_task();

	sleep(50);
}

export int main(void) {
	for (int i = 0; i < 100000000; i++)
	{
		loop();
	}
}
