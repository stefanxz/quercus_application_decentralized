#pragma once

// These standard headers only include type definitions, and do not involve
// operating system support. Therefore, we can include them.
#include "stdbool.h"
#include "stdint.h"

// # General
// Prefixed with Q for "Quercus"
#define Q_NULL -1
#define Q_MAX_NUMBER_OF_MODULES 50
#define Q_MAX_NUMBER_OF_PLANES 256
#define Q_MAX_TASKS 64
#define Q_NUMBER_OF_DEST_TYPES 5

// # LED
// common colors
#define COLOR_RED 0xFF0000
#define COLOR_GREEN 0x00FF00
#define COLOR_BLUE 0x0000FF
#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
// mixed colors
#define COLOR_YELLOW 0xFF9900
#define COLOR_CYAN 0x00FFFF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_ORANGE 0xFFA500
#define COLOR_PURPLE 0xFF00FF
#define COLOR_PINK 0xFFC0CB
#define COLOR_GRAY 0x808080
#define COLOR_BROWN 0xA52A2A

// # Motors and servos
#define BELT_OFF 0
#define BELT_LEFT_SLOW 70
#define BELT_RIGHT_SLOW -70
#define BELT_DOWN_SLOW 70
#define BELT_UP_SLOW -70

#define ARM_LEFT 60
#define ARM_RIGHT 105
#define ARM_NEUTRAL 0

// network
#define TIME_PAUSE 250

// Enum to represent the type of message being sent or received.
// Each value corresponds to a specific type of message.
// TODO: Marian peer review pls - added new message type for key distribution
enum MessageTypes {
	MSG_NONE = 0,
	MSG_REQUEST_MOVEMENT = 1,	 // Request to move a tub to the module
	MSG_REQUEST_RESPONSE = 2,	 // Response to a request for movement
	MSG_PLANE_STATUS = 3,		 // Status update for a plane
	MSG_TUB_STATUS = 4,			 // Status update for a tub
	MSG_PATH_CONFIG = 5,		 // Configuration of the paths
	MSG_REQUEST_PATH_CONFIG = 6, // Request for path configuration
	MSG_TUB_CONFIG = 7,			 // Configuration of a tub
	MSG_KEYS_FOR_YOU = 8,		 // Per-Pico keypair and neighbor pubkeys
	// TODO: CHANGED/REVIEW — Added MSG_NHN_ANNOUNCE (9) to notify the next-hop-right neighbour (NHN)
	// that it is authoritative for an incoming tub. Next to review: common_hardware.c send_nhn_announce()
	// to see how we pack and send this 2-byte message (1 byte MSG_SENDER, 1 byte MSG_TYPE).
	MSG_NHN_ANNOUNCE = 9 // Announce to next-hop-right neighbour
};
// end TODO
#define Q_NHN_QUEUE_SIZE 32
#define Q_NHN_TIMEOUT_TICKS 2000
// TODO: CHANGED/REVIEW — Introduced NHN queue sizing and timeout threshold.
// Q_NHN_QUEUE_SIZE: circular buffer capacity; each entry is a uint32_t (4 bytes) storing a loop tick timestamp.
// Q_NHN_TIMEOUT_TICKS: maximum allowed delta (in loop iterations) before NHN declares a timeout on the head item.

// Enum to represent the content that every message should have.
// Each value corresponds to a specific piece of information in the message.
enum MessageHead {
	MSG_SENDER = 0, // Sender of the message
	MSG_TYPE = 1,	// Type of the message
	MSG_VALUE = 2	// Used for request responses
};

// Enum to represent the content of a request message.
// Each value corresponds to a specific piece of information in the request.
enum RequestsContent {
	REQ_TUB_ID = 2,		   // Identifier for the tub being referenced in the request.
	REQ_DEST_TYPE = 3,	   // Type of destination (e.g., module, plane, etc.).
	REQ_DEST_ID = 4,	   // Identifier for the destination.
	REQ_PLANE_ARRIVED = 5, // Indicates whether a plane has arrived.
	REQ_PLANE_ID = 6,	   // Identifier for the plane.
	REQ_SECURITY = 7,	   // Security-related information for the request.
	REQ_PAYLOAD = 8,	   // Payload data associated with the request.
	REQ_LENGTH = 9		   // Total length of the request message.
};

// Enum to represent the content of an arrivals message.
// Each value corresponds to a specific piece of information in the arrivals data.
enum ArrivalsContent {
	ARR_PLANE_ID = 2,  // Identifier for the plane in the arrivals message.
	ARR_MODULE_ID = 3, // Identifier for the module associated with the arrival.
	ARR_DEP_TIME = 4,  // Departure time for the plane or module.
	ARR_LENGTH = 5	   // Total length of the arrivals message.
};

// Module endpoints encoded as directions. Out is not a direction, but a special additional value.
typedef enum Direction {
	DIR_LASER_LEFT = 0,
	DIR_LASER_RIGHT = 1,
	DIR_RFID = 2,
	DIR_OUT = 3,
} Direction;

// Priority levels for the tubs. The higher the number, the higher the priority.
typedef enum Priority {
	PRIO_LO = 0,
	PRIO_ME = 1,
	PRIO_HI = 2,
} Priority;

// Types of destinations for the tubs.
typedef enum DestinationType {
	DEST_PLANE = 0,
	DEST_DROPOFF = 1,
	DEST_SECURITY = 2,
	DEST_STORAGE = 3,
	DEST_QUARANTINE = 4,
} DestinationType;

// Task structures for the modules. Each module has a list of tasks to perform.
typedef struct {
	Direction to;
	Direction from;
	char request[13]; // MSG_HEAD + DATA_RFID_LENGTH
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

// Structure for the state of the module. It contains the current state of the module and the tub.
typedef struct {
	int at[3];
} State;

// Module structure that contains all related fields of a module.
// TODO: Marian peer review pls - added per-pico key storage in Module
typedef struct Module {
	int id;

	int plane_to_module_id[Q_MAX_NUMBER_OF_PLANES]; // Plane with index plane_id is at the module with id = value of
													// plane_to_id[plane_id].
	uint8_t id_lookup[Q_MAX_NUMBER_OF_MODULES];		// Index 0 will always be Pi
	uint8_t dir_lookup[Q_MAX_NUMBER_OF_MODULES];	// Index 0 will always be Pi

	uint8_t
		next[4]; // Module IDs of the neighbouring modules. We index by Direction. 4th now is not the next-hop neighbour
	uint8_t nearest[5]; // Module IDs of nearest destinations. We index by ModuleType.

	bool is_storage; // Indicates whether this module has storage responsibilities.
	bool should_check;
	Direction to_storage; // Indicates in which direction there is another storage module.

	Tub tub[3]; // Array of tubs, indexed by their position

	State state;

	Task tasks[Q_MAX_TASKS];
	int8_t task_current;
	int8_t task_new;

	// Crypto: Ed25519 keys
	uint8_t my_sk[64];
	uint8_t my_pk[32];
	// Neighbor public keys in order: LEFT, RIGHT, RFID, NEXT_RIGHT_HOP
	uint8_t neighbor_pk[4][32];

	// NHN authoritative queue and timing
	// TODO: CHANGED/REVIEW — Added nhn_queue (uint32_t timestamps, 4 bytes each), nhn_head/tail (ring buffer indices),
	// and loop_tick (monotonic per-iteration counter). Next to review: common_loop.c await_message() handling for
	// MSG_NHN_ANNOUNCE, and loop() timeout/dequeue logic.
	uint32_t nhn_queue[Q_NHN_QUEUE_SIZE]; // circular buffer of loop ticks when NHN messages received
	int nhn_head;
	int nhn_tail;
	uint32_t loop_tick; // increments every loop iteration
} Module;				// structure for a module containing its essential fields
// end TODO

const Task EMPTY_TASK = {.to = DIR_OUT, .from = DIR_OUT};

const int RFID_BLOCK_SIZE = 4;

// TODO: Marian peer review pls - added Ed25519 size constants
// Ed25519 sizes (Monocypher)
#define ED25519_SK_LEN 32
#define ED25519_PK_LEN 32
#define ED25519_SIG_LEN 64
// end TODO

enum DataOnRFID {
	DATA_TUB_OR_PLANE = 0,
	DATA_PLANE_OR_DROPOFF = 1,
	DATA_PLANE_ID = 2,
	DATA_PAYLOAD = 3,
	DATA_DEPARTURE_TIME = 4,
	DATA_TUB_ID = 5,
	DATA_NEEDS_SECURITY = 6,
	DATA_PASSED_SECURITY = 7,
	DATA_PLANE_ARRIVED = 8,
	DATA_DESTINATION = 9,
	DATA_PLANE_DIRECTION = 10, // 0 = outgoing, 1 = incoming
	DATA_RFID_LENGTH = 11
};
