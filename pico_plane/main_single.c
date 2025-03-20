#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../elementary_functions/movement_functions.c"
#include "../elementary_functions/rfid_functions.c"

#include <stdbool.h>
#include <stdio.h>

#define MAX_NUMBER_OF_TUBS 2
#define MAX_NUMBER_OF_PLANES 100
#define MAX_NUMBER_OF_MODULES 8


// Structure for Request
typedef struct
{
    int origin_module;
    int type_message;
    // Data
    bool security_status;
    bool plane_or_drop_off;
    int  plane_id;
    bool payload;
    bool plane_arrived;
    int  destination;

} Request;

// Structure for Tub
typedef struct
{
    int id;
	int plane_id;
	bool plane_dropoff;
    // int priority;
    int passed_security;
    int destination;
	bool plane_arrived;
	// bool is_free;
} Tub;

typedef struct{
	int id;
	int module_id;
	// int departure;
	bool direction; // 0 -> incoming, 1 -> outgoing
	bool arrived;
} Plane;

//Structure for Module
typedef struct
{
	int id;
	int lookup[MAX_NUMBER_OF_MODULES];
	int dropoff_id;
	int quarantine_id;
	int security_id;
	int storage_id;
	Tub tub;
	// Index 0 will be plane on this module
	Plane planes[MAX_NUMBER_OF_PLANES];
} Module;

Plane create_plane(int id, int departure, int module_id, bool direction, bool arrived){
	Plane plane;
	plane.id = id;
	// plane.departure = departure;
	plane.direction = direction; 
	plane.arrived = arrived;
	plane.module_id = module_id;
	return plane;
}

Tub create_tub(int id, int priority, int security_bit, int destination)
{
	Tub tub;
	tub.id = id;
	// tub.priority = priority;
	tub.passed_security = security_bit;
	tub.destination = destination;
	// tub.is_free = 1;
	return tub;
}

// void assign_tub_priority();

// FAKE HAS TO BE IMPLEMENTED
bool is_plane(bool result){
	return result;
}

bool has_tub_plane_arrived(Module module){
		return module.planes[1].arrived;
}

// int get_tub_priority(Plane plane){
// 	return plane.departure;
// }

//FAKE HAS TO BE IMPLEMENTED
void broadcast_plane_detected(Plane plane){
	printf("Plane detected: %d, %d, %d, %d\n", plane.id, plane.module_id, plane.direction, plane.arrived);
}

//FAKE HAS TO BE IMPLMENTED
void broadcast_plane_left(int plane_id){
	printf("Plane left: %d\n", plane_id);
}

bool check_plane_arrived(Module module, int plane_id){
	return (module.planes[1].id == plane_id) && module.planes[1].arrived;
}

int determine_destination(Module module, bool sec_check_needed, bool sec_check_passed, bool plane_dropoff, int plane_id){
	if(sec_check_needed)
		if(sec_check_passed) return module.quarantine_id;
		else return module.security_id;
	else if (plane_dropoff) return module.dropoff_id;
	else return check_plane_arrived(module, plane_id) ? module.planes[1].module_id : module.storage_id;
}

void save_RFID_data(Module* module) {
	if(is_plane(0)){
		//plane detected
		int plane_id = 1;//get_plane_id();
		int deadline = 1;//get_deadline();
		int direction = 1;//get_direction();
		int plane_arrived = 1;//has_plane_arrived();
		module -> planes[0] = create_plane(plane_id, deadline, module -> id, direction, plane_arrived);
		if (plane_arrived) broadcast_plane_detected(module -> planes[0]);
		else broadcast_plane_left(module -> planes[0].id);
	} else {
		if (!module -> planes[0].direction) {
			//tub detected
			int tub_id = 0;//get_tub_id();
			bool tub_has_passed_security = 0;//has_security_been_passed();
			int tub_destination = determine_destination(*module, tub_has_passed_security, 0 /*get_sec_bit()*/, 0/*  get_plane_dropoff_flag() */,3 /* get_plane_id() */);//get_tub_destination();
			
			int tub_priority = -1;
			
			module -> tub = create_tub(tub_id, tub_priority, tub_has_passed_security, tub_destination);
			//send tub status message -> entry
		} else {
			//send tub status message -> exit
		}
	}
}

enum MessageType {
    NONE = 0,
    REQUEST_MOVEMENT = 1,
    REQUEST_RESPONSE = 2,
    PLANE_STATUS = 3,
    TUB_STATUS = 4, // Notify entering/leaving system
    PATHS_CONFIG = 5,
	LOGGING = 6
};

int get_message_type(char* msg){
	return msg[1];
}

int main(void) {
	Module module;
	Module* module_ptr = &module;
	enum EventType e;
	char * msg;
	subscribe_to_event(EVENT_MESSAGE_RECEIVED | EVENT_RFID_DETECT);
	while(1) {
		e = next_event();
		if (e == EVENT_RFID_DETECT) {
			save_RFID_data(module_ptr);
		} else if (e == EVENT_MESSAGE_RECEIVED) {
			next_message_address(&msg);
			int type = get_message_type(&msg);
			if (type == REQUEST_MOVEMENT) {
				//Send accept message
				//Move the actuators
			} else if (type == PLANE_STATUS){
				//Read plane status
				//Save to module
			} else if (type == TUB_STATUS) {
				// Don't care
			} else if (type == PATHS_CONFIG) {
				// Save lookup table
			} else if (type == LOGGING) {
				printf("MESSAGE TYPE WAS WRONG, I DO NOT DO LOGGING\n");
			}
		}
		sleep(10);
	}
	return 0;
}
