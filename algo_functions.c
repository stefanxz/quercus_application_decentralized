#pragma once

#include<stdbool.h>

#define MAX_NUMBER_OF_MODULES 256
#define MAX_NUMBER_OF_PLANES 256

typedef enum Direction {
	LASER_LEFT = 0,
	LASER_RIGHT = 1,
	RFID = 2,
	OUT = 3
} Direction;

typedef struct State {
	int at[3];
	bool in_storage;
} State;
State state;

// Structure for Request
typedef struct Request {
	int type;
	int sender_id;
	int destination;
    int plane_id;

	// RFID info of tub
	int tub_id;
    bool security_status;
    bool plane_or_drop_off;
    bool payload;
    bool plane_arrived;
} Request;

typedef struct {
	Direction to;
	Direction from;
	Request request;
} Task; // structure for a task

typedef struct Tub {
    int id;
	int plane_id;
    int passed_security;
    int destination;
	bool plane_dropoff;
	bool plane_arrived;
} Tub;

typedef struct Module {
	int id;
	int lookup[MAX_NUMBER_OF_MODULES];
	int plane_to_id[MAX_NUMBER_OF_MODULES]; // Index 0 will be plane on this module

	// Module IDs of important modules
	int dropoff_id;
	int quarantine_id;
	int security_id;
	int storage_id;

	// Module IDs of the neighbouring modules. We index by Direction.
	int next[3];

	// Variables for the ring buffer containing the tasks
	Task tasks[10];
	int current;
	int next_free;

	// Variables for storing
	bool is_storage;
	Direction next_storage;
	
	Tub tub;
} Module; // structure for a module containing its essential fields

//FAKE HAS TO BE IMPLEMENTED
void broadcast_plane_detected(int plane_id, int module_id) {
	printf("Plane %d detected at Module %d\n", plane_id, module_id);
}

//FAKE HAS TO BE IMPLMENTED
void broadcast_plane_left(int plane_id, int module_id){
	printf("Plane %d left\n from Module %d", plane_id, module_id);
}

Tub create_tub(int id, int security_bit, int destination)
{
	Tub tub;
	tub.id = id;
	tub.passed_security = security_bit;
	tub.destination = destination;
	// tub.is_free = 1;
	return tub;
}


bool check_plane_arrived(Module module, int tub_plane_id){
	return module.plane_to_id[tub_plane_id] != 0;
}

int determine_destination(Module* module, bool sec_check_needed, bool sec_check_passed, bool plane_dropoff, int plane_id){
	if(sec_check_needed)
		if(sec_check_passed) return module -> quarantine_id;
		else return module -> security_id;
	else if (plane_dropoff) return module -> dropoff_id;
	else return check_plane_arrived(*module, plane_id) ? module->plane_to_id[plane_id] : module->storage_id;
}

void save_RFID_data(Module* module) {
	if(true/* is_plane(0) */){
		//plane detected
		int plane_id = 1;//get_plane_id();
		// int deadline = 1;//get_deadline();
		int direction = 1;//get_direction();
		int plane_arrived = 1;//has_plane_arrived();
		module -> plane_to_id[plane_id] = 1;
		if (plane_arrived) broadcast_plane_detected(plane_id, module -> id);
		else broadcast_plane_left(plane_id, module -> id);
	} else {
		//tub detected
		int tub_id = 0;//get_tub_id();
		bool tub_has_passed_security = 0;//has_security_been_passed();
		int tub_destination = determine_destination(module, tub_has_passed_security, 0 /*get_sec_bit()*/, 0/*  get_plane_dropoff_flag() */,3 /* get_plane_id() */);
		//write tub destination to module
		// int tub_priority = -1;
		
		module -> tub = create_tub(tub_id, tub_has_passed_security, tub_destination);
		//send tub status message -> entry
	}
}

void change_tub_status(/* Module module */){
    //set_security_passed(1);
    //set_needs_security(1 /* get_payload() */);
    //set_destination(1 /* determine_destination(module, get_payload(), 1, module.tub.plane_dropoff, module.tub.plane_id)*/);
}
bool add_task(Direction to, Direction from, Request request, Module* module) {
	if (module->next_free == module->current) {
		// Epic fail
		return false;
	}

	Task task;
	task.to = to;
	task.from = from;
	task.request = request;

	module->tasks[module->next_free] = task;
	module->next_free = (module->next_free + 1) % 7;
	return true;
}

bool do_task(Task* task, Module* module) {
	int tub = task->request.tub_id;

	if (task->to == OUT) {
		// request_leave(tub, module->next[task->from], task->request);
	} else if (task->from == OUT) {
		// wait_to_enter(tub, module->next[task->to], task->request);
	} else {
		// move_within_module(tub, task->from, task->to);
	}
	
	// Mayb
	module->tasks[module->current].from = OUT;
	module->tasks[module->current].to = OUT;
	module->current = (module->current + 1) % 7;
	return true;
}
