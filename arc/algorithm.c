#pragma once

#include "algorithm.h"
#include "movement.c"
#include "rfid.h"
#include <stdbool.h>

// FAKE HAS TO BE IMPLEMENTED
void broadcast_plane_detected(int plane_id, int module_id) {
	printf("Plane %d detected at Module %d\n", plane_id, module_id);
}

// FAKE HAS TO BE IMPLMENTED
void broadcast_plane_left(int plane_id, int module_id) {
	printf("Plane %d left\n from Module %d\n", plane_id, module_id);
}

Tub create_tub(int id, bool passed_security, bool plane_dropoff, bool plane_arrived, int destination, int plane_id) {
	Tub tub;
	tub.id = id;
	tub.passed_security = passed_security;
	tub.plane_dropoff = plane_dropoff;
	tub.destination = destination;
	tub.plane_arrived = plane_arrived;
	tub.plane_id = plane_id;
	return tub;
}

bool check_plane_arrived(Module module, int tub_plane_id) { return module.plane_to_id[tub_plane_id] != 0; }

int determine_destination(Module* module, bool sec_check_passed, bool sec_check_needed, bool plane_dropoff,
						  int plane_id) {
	if (sec_check_needed)
		if (sec_check_passed) return module->quarantine_id;
		else {
			printf("wtf is going on\n");
			return module->security_id;
		}
	else if (plane_dropoff) return module->dropoff_id;
	else return check_plane_arrived(*module, plane_id) ? module->plane_to_id[plane_id] : module->storage_id;
}

/// @brief Saves the data of a tub to the given module.
/// @param module pointer to the Module that receives the rfid readings.
void save_RFID_data(Module* module) {
	// tub detected
	char data[RFID_LENGTH];
	get_entrance_rfid_data(data);
	int tub_id = (int)data[TUB_ID];
	bool has_passed_security = (bool)data[PASSED_SECURITY];
	bool security_bit = (bool)data[NEEDS_SECURITY];
	bool plane_dropoff = (bool)data[PLANE_OR_DROPOFF];

	int plane_id = (int)data[PLANE_ID];
	bool plane_arrived = module->plane_to_id[plane_id];
	int tub_destination = determine_destination(module, has_passed_security, security_bit, plane_dropoff, plane_id);
	// write tub destination to module
	// int tub_priority = -1;
	// printf("Tub gets: id: %d, passed_sec:%d, sec_bit:%d, plane_dropoff:%d, plane_id:%d\n", tub_id,
	// has_passed_security, security_bit, plane_dropoff, plane_id);
	module->tub = create_tub(tub_id, has_passed_security, plane_dropoff, plane_arrived, tub_destination, plane_id);
	// send tub status message -> entry
}

void change_tub_status(/* Module module */) {
	// set_security_passed(1);
	// set_needs_security(1 /* get_payload() */);
	// set_destination(1 /* determine_destination(module, get_payload(), 1, module.tub.plane_dropoff,
	// module.tub.plane_id)*/);
}

bool add_task(Module* module, Direction from, Direction to, char* request) {
	// printf("algo-72 // added task\n");
	Task task;
	task.to = to;
	task.from = from;
	copy_message(task.request, request);

	module->tasks[module->next_free] = task;
	module->next_free = (module->next_free + 1) % 7;
	return true;
}

bool request_to_leave(Module* module, int next_id, char* request) {
	led_set_color(LED_GREEN);
	int response;

	// Send 10 times or until success:
	for (int i = 0; i < 10 && send_request_movement(next_id, request) < 0; i++) {
		printf("move // packet loss\n");
		sleep(100);
	}

	// Wait for response to arrive and acquire it:
	response = await_response(module);
	if (response > 0) {
		printf("move // response got %d \n", response);
		return true;
	} else {
		return false;
	}
}

bool do_task(Module* module) {
	// printf("algo-84 // doing task\n");
	Task* task = &(module->tasks[module->current]);
	int tub_id = task->request[REQ_TUB_ID];

	if (task->to == OUT && task->from == OUT) printf("We are doing an empty task, fml\n");

	if (task->to == OUT) {
		printf("Tub %d to leave to module %d by %d\n", tub_id, module->next[task->from], task->from);
		printf("request ptr: %d, request[2] ptr: %d\n", task->request, task->request);

		int next_id = module->next[task->from];
		if (request_to_leave(module, next_id, task->request)) {
			leave_at(task->from);
		} else {
			return false;
		}
	} else if (task->from == OUT) {
		printf("Tub %d to enter module %d\n", tub_id, module->id);
		enter_at(task->to);
		send_request_response(task->request[MSG_SENDER], 1);
	} else {
		printf("Tub %d hits the griddy from to %d to %d\n", tub_id, task->from, task->to);
		move_within_module(tub_id, task->from, task->to);
	}

	module->tasks[module->current].from = OUT;
	module->tasks[module->current].to = OUT;
	module->current = (module->current + 1) % 7;
	printf("current: %d, next_free: %d\n", module->current, module->next_free);
	return true;
}

bool no_tasks(Module* module) { return (module->current == module->next_free); }

void determine_next_direction_to(Module* module, Direction* direction_lookup);