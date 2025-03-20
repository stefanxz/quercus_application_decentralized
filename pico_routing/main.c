#include "quercus_lib_pico.h"
#include "libc_builtin.h"

#include "elementary_functions/movement_functions.c"
#include "elementary_functions/rfid_functions.c"

#include <stdbool.h>

#define MAX_MODULES 100
#define MAX_PLANES 256

int SECOND = 1000000;

// Enum for task
typedef enum
{
	IDLE,
	MOVE_RFID_TO_LEFT,
	MOVE_LEFT_TO_RFID,

	MOVE_RFID_TO_RIGHT,
	MOVE_RIGHT_TO_RFID,

	MOVE_LEFT_TO_RIGHT,
	MOVE_RIGHT_TO_LEFT,

	LEAVE_AT_LEFT,
	LEAVE_AT_RIGHT,
	LEAVE_AT_RFID,

	WAIT_AT_RFID,
	WAIT_AT_LEFT,
	WAIT_AT_RIGHT
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

// Neighbouring modules
typedef struct
{
    int left;
    int right;
    int rfid;
} NextModule;
NextModule next;

typedef enum
{
	LASER_LEFT,
	LASER_RIGHT,
	RFID
} ModulePoints;

typedef struct
{
	bool at_left;
	bool at_right;
	bool at_RFID;
} State;
State state;

// Unused for now
State projected_state;

int planes[MAX_PLANES];
int lookup[MAX_MODULES];

void init(int my_id) {
	state.at_left = false;
	state.at_right = false;
	state.at_RFID = false;

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
			// PUT IN ID 
			move_within_module(0, RFID, LASER_LEFT);
			state.at_left = true;
			state.at_RFID = false;
			break;
		case MOVE_LEFT_TO_RFID:
			// PUT IN ID 
			move_within_module(0, LASER_LEFT, RFID);
			state.at_left = false;
			state.at_RFID = true;
			break;
		case MOVE_RFID_TO_RIGHT:
			// PUT IN ID 
			move_within_module(0, RFID, LASER_RIGHT);
			state.at_right = true;
			state.at_RFID = false;
			break;
		case MOVE_RIGHT_TO_RFID:
			// PUT IN ID 
			move_within_module(0, LASER_RIGHT, RFID);
			state.at_right = false;
			state.at_RFID = true;
			break;
		case MOVE_LEFT_TO_RIGHT:
			// PUT IN ID 
			move_within_module(0, LASER_LEFT, LASER_RIGHT);
			state.at_left = false;
			state.at_right = true;
			break;
		case MOVE_RIGHT_TO_LEFT:
			// PUT IN ID 
			move_within_module(0, LASER_RIGHT, LASER_LEFT);
			state.at_right = false;
			state.at_left = true;
			break;
		case LEAVE_AT_LEFT:
			// PUT IN ID 
			request_leave(0, next.left, tasks[current].request);
			state.at_left = false;
			break;
		case LEAVE_AT_RIGHT:
			// PUT IN ID 
			request_leave(0, next.right, tasks[current].request);
			state.at_right = false;
			break;
		case LEAVE_AT_RFID:
			// PUT IN ID 
			request_leave(0, next.left, tasks[current].request);
			state.at_RFID = false;
			break;
	}
	tasks[current].move = IDLE;
	current = (current + 1) % 7;
	return true;
}

void handle_request() {
	// Reroute tub if it's plane has arrived
	
	if(!current_request.plane_arrived && planes[current_request.plane_ID] != 0) {
		current_request.destination = planes[current_request.plane_ID];
		current_request.plane_arrived = true;
	}

	int which_module = lookup[current_request.destination];

	if (current_request.sender_ID == next.left) {
		add_task(WAIT_AT_LEFT);
		if(which_module == next.rfid) {
			add_task(MOVE_LEFT_TO_RFID);
			add_task(LEAVE_AT_RFID);
		} else {
			add_task(MOVE_LEFT_TO_RIGHT);
			add_task(LEAVE_AT_RIGHT);
		}
	} else if (current_request.sender_ID == next.right) {
		add_task(WAIT_AT_RIGHT);
		if(which_module == next.rfid) {
			add_task(MOVE_RIGHT_TO_RFID);
			add_task(LEAVE_AT_RFID);
		} else {
			add_task(MOVE_RIGHT_TO_LEFT);
			add_task(LEAVE_AT_LEFT);
		}
	} else if (current_request.sender_ID == next.rfid) {
		add_task(WAIT_AT_RFID);
		if(which_module == next.left) {
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
	init(3, 3, 0);
	for (int i = 0; i < 100000000; i++)
	{
		loop();
	}
}
