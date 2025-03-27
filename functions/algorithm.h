#pragma once

#define MAX_NUMBER_OF_MODULES 7
#define MAX_NUMBER_OF_PLANES 256
#define NON -1

#include <stdbool.h>

typedef enum Direction {
	LASER_LEFT = 0,
	LASER_RIGHT = 1,
	RFID = 2,
	OUT = 3
} Direction;

typedef struct {
	Direction to;
	Direction from;
	char request[13]; // MSG_HEAD + RFID_LENGTH
} Task; // structure for a task

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
	int id_lookup[MAX_NUMBER_OF_MODULES + 1]; // Index 0 will always be Pi
	Direction direction_lookup[MAX_NUMBER_OF_MODULES + 1]; // Index 0 will always be Pi
	int plane_to_id[MAX_NUMBER_OF_PLANES]; // Index 0 will be plane on this module

	// Module IDs of important modules
	int dropoff_id;
	int quarantine_id;
	int security_id;
	int storage_id;

	// Module IDs of the neighbouring modules. We index by Direction.
	int next[3];

	// Variables for the ring buffer containing the tasks
	Task tasks[7];
	int current;
	int next_free;

	// Variables for storing
	bool is_storage;
	Direction next_storage;

	Tub tub;
} Module; // structure for a module containing its essential fields

typedef struct {
    int at[3];
    bool is_storing;
} State; // structure for the state of the modules