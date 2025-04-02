#pragma once

#define MAX_NUMBER_OF_MODULES 50
#define MAX_NUMBER_OF_PLANES 256
#define NON -1
#define MAX_TASKS 32
#define NUMBER_OF_DEST_TYPES 5

#include <stdbool.h>

// common colors
#define COLOR_RED 0xFF0000
#define COLOR_GREEN 0x00FF00
#define COLOR_BLUE 0x0000FF
#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
// mixed colors
#define COLOR_YELLOW 0xFFFF00
#define COLOR_CYAN 0x00FFFF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_ORANGE 0xFFA500
#define COLOR_PURPLE 0xFF00FF
#define COLOR_PINK 0xFFC0CB
#define COLOR_GRAY 0x808080
#define COLOR_BROWN 0xA52A2A

// Module endpoints encoded as directions. Out is not a direction, but a special additional value.
typedef enum Direction {
	LASER_LEFT = 0,
	LASER_RIGHT = 1,
	RFID = 2,
	OUT = 3,
} Direction;

// Priority levels for the tubs. The higher the number, the higher the priority.
typedef enum Priority {
	PRIO_LO = 0,
	PRIO_ME = 1,
	PRIO_HI = 2,
} Priority;

// Types of destinations for the tubs.
typedef enum DestinationType {
	PLANE = 0,
	DROPOFF = 1,
	SECURITY = 2,
	STORAGE = 3,
	QUARANTINE = 4,
} DestinationType;

// Task structures for the modules. Each module has a list of tasks to perform.
typedef struct {
	Direction to;
	Direction from;
	char request[13]; // MSG_HEAD + RFID_LENGTH
} Task;				  // structure for a task

// Tub structure that contains all related information about a tub.
typedef struct Tub {
	int id;
	int plane_id;
	int destination_id;
	int destination_type;
	Priority priority;
	bool passed_security;
	bool plane_dropoff;
	bool plane_arrived;
} Tub;

// Module structure that contains all related fields of a module.
typedef struct Module {
	int id;

	int plane_to_id[MAX_NUMBER_OF_PLANES];			 //Plane with index plane_id is at the module with id = value of plane_to_id[plane_id].
	uint8_t id_lookup[MAX_NUMBER_OF_MODULES];		 // Index 0 will always be Pi
	uint8_t dir_lookup[MAX_NUMBER_OF_MODULES]; 		 // Index 0 will always be Pi

	uint8_t next[3];	// Module IDs of the neighbouring modules. We index by Direction.
	uint8_t nearest[5]; // Module IDs of nearest destinations. We index by ModuleType.

	bool is_storage;	  // Indicates whether this module has storage responsibilities.
	bool should_check;
	Direction to_storage; // Indicates in which direction there is another storage module.

	Tub tub;
} Module;

// Structure for the state of the module. It contains the current state of the module and the tub.
typedef struct {
	int at[3];
} State;

