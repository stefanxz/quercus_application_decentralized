#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../elementary_functions/movement_functions.c"
#include "../elementary_functions/rfid_functions.c"

#include <stdbool.h>

#define MAX_NUMBER_OF_TUBS 2
#define MAX_NUMBER_OF_PLANES 2
#define MAX_NUMBER_OF_MODULES 8

//Structure for Module
typedef struct
{
	int id;
	int plane_to_id[255];
	int lookup[MAX_NUMBER_OF_MODULES];
	int dropoff_id;
	int quarantine_id;
	int security_id;
	int storage_id;
	Tub tub;
} Module;

typedef struct
{
    int id;
    // int priority;
    int plane_id;
    bool plane_dropoff;
    int passed_security;
    int destination;
	bool plane_arrived;
	// bool is_free;
} Tub;


void change_tub_status(Module module){
    set_security_passed(1);
    set_needs_security(1 /* get_payload() */);
    set_destination(1 /* determine_destination(module, get_payload(), 1, module.tub.plane_dropoff, module.tub.plane_id)*/);
}

int determine_destination(Module module, bool sec_check_needed, bool sec_check_passed, bool plane_dropoff, int tub_plane_id){
	if(sec_check_needed)
		if(sec_check_passed) return module.quarantine_id;
		else return module.security_id;
	else if (plane_dropoff) return module.dropoff_id;
	else return check_plane_arrived1(module, tub_plane_id) ? module.plane_to_id[tub_plane_id] : module.storage_id;
}

bool check_plane_arrived1(Module module, int tub_plane_id){
	return module.plane_to_id[tub_plane_id] != 0;
}

export int main(void) {

}
