#include <stdbool.h>
#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../elementary_functions/movement_functions.c"
#include "../elementary_functions/rfid_functions.c"

#define MAX_MODULES 100
#define MAX_PLANES 256

int SECOND = 1000000;

typedef struct {
    int sender_ID;
    int type;
    // Data
	int tub_ID;
    bool security_status;
    bool plane_or_dropoff;
    int  plane_ID;
    bool payload;
    bool plane_arrived;
    int  destination;
} Request; // structure for a request
Request current_request;

typedef struct {
	Direction to;
	Direction from;
	Request request;
} Task; // structure for a task
Task tasks[10];
int current;
int next_free;

int next[3];
typedef enum {
	LASER_LEFT,
	LASER_RIGHT,
	RFID,
	OUT
} Direction;

typedef struct {
	int at[3];
	bool is_storing;
} State;
State state;

// Unused for now
State projected_state;

int planes[MAX_PLANES];
int lookup[MAX_MODULES];

void init(int my_id) {
	state.at[LASER_LEFT] = -1;
	state.at[LASER_RIGHT] = -1;
	state.at[RFID] = -1;

	for(int i = 0; i < MAX_PLANES; i++) {
		planes[i] = 0;
	}

	for(int i = 0; i < MAX_MODULES; i++) {
		lookup[i] = 0;
	}
}

bool add_task(Direction to, Direction from, Request request) {
	if (next_free == current) {
		// Epic fail
		return false;
	}

	Task task;
	task.move = move;
	task.request = current_request;

	tasks[next_free] = task;
	next_free = (next_free + 1) % 7;
	return true;
}

bool do_task() {
	Task task = tasks[current];
	int tub = task.request.tub_ID;

	if (to == OUT) {
		request_leave(tub, next[task.from], task.request);
	} else if (from == OUT) {
		wait_to_enter(tub, next[task.to], task.request);
	} else {
		move_within_module(tub, task.from, task.to);
	}
	
	// Mayb
	tasks[current].from = OUT;
	tasks[current].to = OUT;
	current = (current + 1) % 7;
	return true;
}

void handle_request() {
	// Reroute tub if it's plane has arrived
	if(!current_request.plane_arrived && planes[current_request.plane_ID] != 0) {
		current_request.destination = planes[current_request.plane_ID];
		current_request.plane_arrived = true;
	}

	int origin = current_request.sender_ID;
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
