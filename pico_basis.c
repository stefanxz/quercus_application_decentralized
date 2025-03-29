#include "libc_builtin.h"
#include "quercus_lib_pico.h"

#include "lib/essentials.h"
#include "lib/movement.c"
#include "lib/network.c"

#include <stdbool.h>

const Task empty = {.to = OUT, .from = OUT};

Module this;
State state;

Task tasks[MAX_TASKS];
int task_current = 0;
int task_new = 0;

void init(Module* mod) {
	// Initialize the module's state
	state.at[RFID] = NON;
	state.at[LASER_LEFT] = NON;
	state.at[LASER_RIGHT] = NON;

	this.next[LASER_LEFT] = mod->next[LASER_LEFT];
	this.next[LASER_RIGHT] = mod->next[LASER_RIGHT];
	this.next[RFID] = mod->next[RFID];

	this.nearest[PLANE] = mod->nearest[PLANE];
	this.nearest[QUARANTINE] = mod->nearest[QUARANTINE];
	this.nearest[STORAGE] = mod->nearest[STORAGE];
	this.nearest[SECURITY] = mod->nearest[SECURITY];

	this.id = mod->id;
	this.is_storage = mod->is_storage;

	for (int i = 0; i < MAX_NUMBER_OF_MODULES; i++) {
		this.id_lookup[i] = mod->id_lookup[i];
	}

	for (int i = 0; i < MAX_NUMBER_OF_PLANES; i++) {
		this.plane_to_id[i] = 0;
	}

	task_current = 0;
	task_new = 0;
	Task empty = {.from = OUT, .to = OUT};
	for (int i = 0; i < MAX_TASKS; ++i) {
		tasks[i] = empty;
	}
}

// CHANGE FAST AF
void update_state(Direction from, Direction to, int tub_id) {
	if(from != OUT) state.at[from] = NON;
	if(to != OUT) state.at[to] = tub_id;
}

void copy_message(char* request, char* msg) {
	for (int i = 0; i < REQ_LENGTH; i++) {
		request[i] = msg[i];
	}
}

bool add_task(Direction from, Direction to, char* request) {
	Task task;
	task.to = to;
	task.from = from;

	// POTENTIALLY SMELLY
	copy_message(task.request, request);

	tasks[task_new] = task;
	task_new = (task_new + 1) % MAX_TASKS;
	return true;
}

bool no_tasks() { return (task_current == task_new); }