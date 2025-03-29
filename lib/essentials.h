#pragma once

#define MAX_NUMBER_OF_MODULES 8
#define MAX_NUMBER_OF_PLANES 256
#define NON -1
#define MAX_TASKS 32

#include <stdbool.h>

typedef enum Direction {
	LASER_LEFT = 0,
	LASER_RIGHT = 1,
	RFID = 2,
	OUT = 3,
} Direction;

typedef enum DestinationType {
	PLANE = 0,
	DROPOFF = 1,
	SECURITY = 2,
	STORAGE = 3,
	QUARANTINE = 4,
} DestinationType;

typedef struct {
	Direction to;
	Direction from;
	char request[13]; // MSG_HEAD + RFID_LENGTH
} Task;				  // structure for a task

typedef struct Tub {
	int id;
	int plane_id;
	int destination;

	bool passed_security;
	bool plane_dropoff;
	bool plane_arrived;
} Tub;

typedef struct Module {
	int id;

	int plane_to_id[MAX_NUMBER_OF_PLANES];			 // Index 0 will be plane on this module
	int id_lookup[MAX_NUMBER_OF_MODULES + 1];		 // Index 0 will always be Pi
	Direction dir_lookup[MAX_NUMBER_OF_MODULES + 1]; // Index 0 will always be Pi

	int next[3];	// Module IDs of the neighbouring modules. We index by Direction.
	int nearest[5]; // Module IDs of nearest destinations. We index by ModuleType.

	bool is_storage;	  // Indicates whether this module has storage responsibilities.
	Direction to_storage; // Indicates in which direction there is another storage module.

	Tub tub;
} Module; // structure for a module containing its essential fields

typedef struct {
	int at[3];
	bool is_storing;
} State; // structure for the state of the modules