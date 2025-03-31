#pragma once

#define MAX_NUMBER_OF_MODULES 50
#define MAX_NUMBER_OF_PLANES 256
#define NON -1
#define MAX_TASKS 32
#define NUMBER_OF_DEST_TYPES 5

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
	int destination_id;
	int destination_type;

	bool passed_security;
	bool plane_dropoff;
	bool plane_arrived;
} Tub;

typedef struct Module {
	int id;

	int plane_to_id[MAX_NUMBER_OF_PLANES];			 //Plane with index plane_id is at the module with id = value of plane_to_id[plane_id].
	uint8_t id_lookup[MAX_NUMBER_OF_MODULES];		 // Index 0 will always be Pi
	uint8_t dir_lookup[MAX_NUMBER_OF_MODULES]; 		 // Index 0 will always be Pi

	uint8_t next[3];	// Module IDs of the neighbouring modules. We index by Direction.
	uint8_t nearest[5]; // Module IDs of nearest destinations. We index by ModuleType.

	bool is_storage;	  // Indicates whether this module has storage responsibilities.
	Direction to_storage; // Indicates in which direction there is another storage module.

	Tub tub;
} Module; // structure for a module containing its essential fields

typedef struct {
	int at[3];
} State; // structure for the state of the modules

