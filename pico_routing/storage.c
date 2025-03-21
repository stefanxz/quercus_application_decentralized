#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../elementary_functions/movement_functions.c"
#include "../elementary_functions/rfid_functions.c"

#include <stdbool.h>

#define MAX_MODULES 100
#define MAX_PLANES 256

int SECOND = 1000000;

// Enum for task
typedef enum
{
	IDLE,
	WAIT_AT_RFID,
	WAIT_AT_LEFT,
	WAIT_AT_RIGHT,
	LEAVE_AT_LEFT,
	LEAVE_AT_RIGHT,
	LEAVE_AT_RFID,
	MOVE_RFID_TO_LEFT,
	MOVE_RFID_TO_RIGHT,
	MOVE_LEFT_TO_RIGHT,
	MOVE_LEFT_TO_RFID,
	MOVE_RIGHT_TO_LEFT,
	MOVE_RIGHT_TO_RFID
} Move;

// Structure for a request
typedef struct
{
    int sender_ID;
    int type;
    // Data
    bool security_status;
    bool plane_or_dropoff;
    int  plane_ID;
    bool payload;
    bool plane_arrived;
    int  destination;

} Request;
Request current_request;

typedef struct
{
	Move move;
	Request request;
} Task;
Task tasks[10];
int current;
int next_free;

int next[3];

bool is_storage;
ModulePoints next_storage;

typedef enum
{
	LASER_LEFT,
	LASER_RIGHT,
	RFID
} ModulePoints;

typedef struct
{
	bool at[3];
	int stored_tub_ID;
	ModulePoints stored_at;
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

bool add_task(int move) {
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
	switch (tasks[current].move) {
		case IDLE:
			break;
		case MOVE_RFID_TO_LEFT:
			move_within_module(0, RFID, LASER_LEFT);
			state.at[LASER_LEFT] = true;
			state.at[RFID] = false;
			break;
		case MOVE_LEFT_TO_RFID:
			move_within_module(0, LASER_LEFT, RFID);
			state.at[LASER_LEFT] = false;
			state.at[RFID] = true;
			break;
		case MOVE_RFID_TO_RIGHT:
			move_within_module(0, RFID, LASER_RIGHT);
			state.at[LASER_RIGHT] = true;
			state.at[RFID] = false;
			break;
		case MOVE_RIGHT_TO_RFID:
			move_within_module(0, LASER_RIGHT, RFID);
			state.at[LASER_RIGHT] = false;
			state.at[RFID] = true;
			break;
		case MOVE_LEFT_TO_RIGHT:
			move_within_module(0, LASER_LEFT, LASER_RIGHT);
			state.at[LASER_LEFT] = false;
			state.at[LASER_RIGHT] = true;
			break;
		case MOVE_RIGHT_TO_LEFT:
			move_within_module(0, LASER_RIGHT, LASER_LEFT);
			state.at[LASER_RIGHT] = false;
			state.at[LASER_LEFT] = true;
			break;
		case LEAVE_AT_LEFT:
			request_leave(0, next[LASER_LEFT], tasks[current].request);
			state.at[LASER_LEFT] = false;
			break;
		case LEAVE_AT_RIGHT:
			request_leave(0, next[LASER_RIGHT], tasks[current].request);
			state.at[LASER_RIGHT] = false;
			break;
		case LEAVE_AT_RFID:
			request_leave(0, next[RFID], tasks[current].request);
			state.at[RFID] = false;
			break;
	}
	tasks[current].move = IDLE;
	current = (current + 1) % 7;
	return true;
}

// Simple implementation for now
void handle_storage() {
	if (state.stored_tub_ID != -1) {
		if(next_storage == LASER_LEFT) {
			if(state.stored_at == RFID) {
				add_task(MOVE_RFID_TO_LEFT);
			}
			add_task(LEAVE_AT_LEFT);
		} else if(next_storage == LASER_RIGHT) {
			if(state.stored_at == RFID) {
				add_task(MOVE_RFID_TO_RIGHT);
			}
			add_task(LEAVE_AT_RIGHT);
		} else {
			add_task(LEAVE_AT_RFID);
		}
	}
	state.stored_tub_ID = -1;
}

void handle_request() {
	// Reroute tub if it's plane has arrived
	if(!current_request.plane_arrived && planes[current_request.plane_ID] != 0) {
		current_request.destination = planes[current_request.plane_ID];
		current_request.plane_arrived = true;
	}

	int origin = current_request.sender_ID;
	int end = lookup[current_request.destination];

	if(is_storage) {
		handle_storage();
	}

	if (origin = next[LASER_LEFT]) {
		add_task(WAIT_AT_LEFT);
		if(end == next[LASER_RIGHT]) {
			add_task(MOVE_LEFT_TO_RFID);
			add_task(LEAVE_AT_RFID);
		} else {
			add_task(MOVE_LEFT_TO_RIGHT);
			add_task(LEAVE_AT_RIGHT);
		}
	} else if (current_request.sender_ID == next[LASER_RIGHT]) {
		add_task(WAIT_AT_RIGHT);
		if(end == next[RFID]) {
			add_task(MOVE_RIGHT_TO_RFID);
			add_task(LEAVE_AT_RFID);
		} else {
			add_task(MOVE_RIGHT_TO_LEFT);
			add_task(LEAVE_AT_LEFT);
		}
	} else if (current_request.sender_ID == next[RFID]) {
		add_task(WAIT_AT_RFID);
		if(end == next[LASER_LEFT]) {
			add_task(MOVE_RFID_TO_LEFT);
			add_task(LEAVE_AT_LEFT);
		} else {
			add_task(MOVE_RFID_TO_RIGHT);
			add_task(LEAVE_AT_RIGHT);
		}
	}
}

void get_request() {
	enum EventType e;
	while ((e = next_event())) {
		char* msg;
		if(e == EVENT_MESSAGE_RECEIVED) {
			handle_message(&msg, 0);
		} else {
			// Handle other events ?
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
	for (int i = 0; i < 100000000; i++) {
		loop();
	}
}
