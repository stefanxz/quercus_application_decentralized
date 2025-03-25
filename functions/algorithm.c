#pragma once

#include "algorithm.h"
#include "movement.c"
#include <stdbool.h>

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
		// plane detected
		int plane_id = 1; //get_plane_id();
		// int deadline = 1; //get_deadline();
		int direction = 1; // get_direction();
		int plane_arrived = 1;// has_plane_arrived();
		module -> plane_to_id[plane_id] = 1;
		if (plane_arrived) broadcast_plane_detected(plane_id, module -> id);
		else broadcast_plane_left(plane_id, module -> id);
	} else {
		// tub detected
		int tub_id = 0;//get_tub_id();
		bool tub_has_passed_security = 0;//has_security_been_passed();
		int tub_destination = determine_destination(module, tub_has_passed_security, 0 /*get_sec_bit()*/, 0/*  get_plane_dropoff_flag() */,3 /* get_plane_id() */);
		// write tub destination to module
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
bool add_task(Module* module, Direction to, Direction from, Request request) {
	if (module->next_free == module->current) {
		// Epic fail, too many tasks
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

bool do_task(Module* module) {
	Task* task = &(module->tasks[module->current]);
	int tub = task->request.tub_id;

	if (task->to == OUT) {
		printf("Tub %d to leave module %d\n", tub, module->id);
		leave_at(tub, module->next[task->from], encode_request(&(task->request)));
	} else if (task->from == OUT) {
		printf("Tub %d to enter module %d\n", tub, module->id);
		enter_at(module->next[task->to]);
	} else {
		printf("Tub %d hits the griddy from to %d to %d", tub, task->from, task->to);
		move_within_module(tub, task->from, task->to);
	}
	
	module->tasks[module->current].from = OUT;
	module->tasks[module->current].to = OUT;
	module->current = (module->current + 1) % 7;
	return true;
}

bool no_tasks(Module* module) {
	return (module->current == module->next_free);
}
