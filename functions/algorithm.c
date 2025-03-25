#pragma once

#include "algorithm.h"
#include "rfid.h"
#include <stdbool.h>

//FAKE HAS TO BE IMPLEMENTED
void broadcast_plane_detected(int plane_id, int module_id) {
	printf("Plane %d detected at Module %d\n", plane_id, module_id);
}

//FAKE HAS TO BE IMPLMENTED
void broadcast_plane_left(int plane_id, int module_id){
	printf("Plane %d left\n from Module %d", plane_id, module_id);
}

Tub create_tub(int id, bool passed_security, bool plane_dropoff, bool plane_arrived, int destination, int plane_id)
{
	Tub tub;
	tub.id = id;
	tub.passed_security = passed_security;
	tub.plane_dropoff = plane_dropoff;
	tub.destination = destination;
	tub.plane_arrived = plane_arrived;
	tub.plane_id = plane_id;
	// tub.is_free = 1;
	return tub;
}


bool check_plane_arrived(Module module, int tub_plane_id){
	return module.plane_to_id[tub_plane_id] != 0;
}

int determine_destination(Module* module, bool sec_check_passed, bool sec_check_needed, bool plane_dropoff, int plane_id){
	return 69;
	if(sec_check_needed)
		if(sec_check_passed) return module -> quarantine_id;
		else return module -> security_id;
	else if (plane_dropoff) return module -> dropoff_id;
	else return check_plane_arrived(*module, plane_id) ? module->plane_to_id[plane_id] : module->storage_id;
}

/// @brief Saves the data of a tub to the given module.
/// @param module pointer to the Module that receives the rfid readings.
void save_RFID_data(Module* module) {
	// tub detected
	char data[END_OF_ENUM];
	get_entrance_rfid_data(data);
	int tub_id = (int)data[TUB_ID];
	bool has_passed_security = (bool)data[PASSED_SECURITY];
	bool security_bit = (bool) data[SECURITY];
	bool plane_dropoff = (bool) data[PLANE_DROPOFF];

	int plane_id = (int)data[PLANE_ID];

	int tub_destination = determine_destination(module, has_passed_security, security_bit, plane_dropoff, plane_id);
	// write tub destination to module
	// int tub_priority = -1;
	printf("Tub gets: id: %d, passed_sec:%d, sec_bit:%d, plane_dropoff:%d, plane_id:%d\n", tub_id, has_passed_security, security_bit, plane_dropoff, plane_id);
	module -> tub = create_tub(tub_id, has_passed_security, plane_dropoff, 1/*FAKE*/, tub_destination, plane_id);
	//send tub status message -> entry
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
	Task * task = &(module->tasks[module->current]);
	int tub = task->request.tub_id;

	if (task->to == OUT) {
		//leave_at(tub, module->next[task->from], task->request.tub_data);
	} else if (task->from == OUT) {
		//enter_at(tub, module->next[task->to]);
	} else {
		//move_within_module(tub, task->from, task->to);
	}
	
	module->tasks[module->current].from = OUT;
	module->tasks[module->current].to = OUT;
	module->current = (module->current + 1) % 7;
	return true;
}

bool no_tasks(Module* module) {
	return (module->current == module->next_free);
}
