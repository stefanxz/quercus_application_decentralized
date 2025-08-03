#include "common.h"


/// @brief Send a packet to the Pi to indicate the status of a plane.
/// @param plane_id ID of the plane to be updated.
/// @param module_id ID of the module sending the update.
/// @return 0 on success, < 0 on failure.
int broadcast_plane_status(int plane_id, int dep_time, int module_id) {
	char data[ARR_LENGTH];
	data[MSG_SENDER] = module_id;
	data[MSG_TYPE] = MSG_PLANE_STATUS;
	data[ARR_PLANE_ID] = plane_id;
	data[ARR_MODULE_ID] = module_id;
	data[ARR_DEP_TIME] = dep_time;
	return send_packet(0, data, sizeof(data));
}

/// @brief Checks if there is a plane on the DIR_RFID reader.
/// @return 1 if there is, 0 if there is nothing, -1 if it's a tub.
int check_for_plane(){
    // Check if there is a tag on the DIR_RFID reader
    if(RFID_check_tag()){
        // Check if the tag is a plane or tub
        if (get_rfid_data(DATA_TUB_OR_PLANE)) {
            // It's a plane
            return 1;
		}
    }
    // No plane detected
    return 0;
}

/// @brief The code for a gate module. Loops infinitely, checking if there is a plane detected, changing state
/// and routing tubs accordingly.
export int main(void) {
    //initialize the module
    Module module = module_init();
    int8_t plane_direction = 0; //-1 for outgoing, 0 for no plane, 1 for incoming plane

    while(1){
        // Check if there is a plane on the DIR_RFID reader
        int8_t new_gate_status = check_for_plane();

        // If there is a plane detected and there is already a plane here
        if(plane_direction && new_gate_status == 1) {
            // Save the plane id
            int new_plane_id = get_rfid_data(DATA_PLANE_ID);
            printf("Detecting plane: %d\n", new_plane_id);

            // Check if this plane is the one already here
            if(module.plane_to_id[new_plane_id] == module.id){
                //Plane leaves
                plane_direction = 0;
                module.plane_to_id[new_plane_id] = 0;

                // Send the plane status to all modules
                broadcast_plane_status(new_plane_id, -1, module.id);

                //Set the LED color to note that a plane has landed
                led_set_color(COLOR_BLUE);
                sleep(2000);
            } else {
                // If the plane is already a different plane here, do not add a new one
                printf("There is already a plane here, you may not add a new one.\n");
            }
            // If there is no plane here and there was a new one detected
        } else if(!plane_direction && new_gate_status == 1) {
            printf("Plane is not here and I am detecting a new one.\n");

            // Save the plane id and departure time
            int new_plane_id = get_rfid_data(DATA_PLANE_ID);
            int new_plane_dep_time = get_rfid_data(DATA_DEPARTURE_TIME);
            // Save if the plane is incoming or outgoing
            if(get_rfid_data(DATA_PLANE_DIRECTION) == 0) plane_direction = -1;
            else plane_direction = 1;

            // Save the module id to plane_to_id table at the id of the plane
            module.plane_to_id[new_plane_id] = module.id;
            // Send the plane status to all modules
            broadcast_plane_status(new_plane_id, new_plane_dep_time, module.id);

            // Set the LED color to note that a plane has landed
            led_set_color(COLOR_BLUE);
            sleep(2000);
        }
        // If the module is incoming
        if(plane_direction == 1){
            // Deal with tubs entering the system
            handle_tub_rfid_entry(&module);
        }
        // Set the LED color to note that the module will now be doing routing
        led_set_color(COLOR_WHITE);
        // Deal with routing of tubs
        loop(&module);

        // Sleep for a bit to add reliability
        sleep(100);
    }
}
