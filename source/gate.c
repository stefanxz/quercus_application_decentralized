#include "../lib/pico_plane.c"

/// @brief Checks if there is a plane on the RFID reader.
/// @return 1 if there is, 0 if there is nothing, -1 if it's a tub.
int check_for_plane(){
    // Check if there is a tag on the RFID reader
    if(RFID_check_tag()){
        // Check if the tag is a plane or tub
        if (get_rfid_data(TUB_OR_PLANE)) {
            // It's a plane
            return 1;
		}
    }
    // No plane detected
    return 0;
}
int8_t plane_direction = 0; //-1 for outgoing, 0 for no plane, 1 for incoming plane

/// @brief The code for a gate module. Loops infinitely, checking if there is a plane detected, changing state
/// and routing tubs accordingly. 
export int main(void) {
    //initialize the module
    init();
    while(1){
        // Check if there is a plane on the RFID reader
        int8_t new_gate_status = check_for_plane();

        // If there is a plane detected and there is already a plane here
        if(plane_direction && new_gate_status == 1) {
            // Save the plane id
            int new_plane_id = get_rfid_data(PLANE_ID);
            printf("Detecting plane: %d\n", new_plane_id);

            // Check if this plane is the one already here
            if(this.plane_to_id[new_plane_id] == this.id){
                //Plane leaves
                plane_direction = 0;
                this.plane_to_id[new_plane_id] = 0;

                // Send the plane status to all modules
                broadcast_plane_status(new_plane_id, -1, this.id);

                //Set the LED color to note that a plane has landed
                led_set_color(LED_BLUE);
                sleep(2000);
            } else {
                // If the plane is already a different plane here, do not add a new one
                printf("There is already a plane here, you may not add a new one.\n");
            }
            // If there is no plane here and there was a new one detected
        } else if(!plane_direction && new_gate_status == 1) {
            printf("Plane is not here and I am detecting a new one.\n");
            
            // Save the plane id and departure time
            int new_plane_id = get_rfid_data(PLANE_ID);
            int new_plane_dep_time = get_rfid_data(DEPARTURE_TIME);
            // Save if the plane is incoming or outgoing
            if(get_rfid_data(PLANE_DIRECTION) == 0) plane_direction = -1;
            else plane_direction = 1;

            // Save the module id to plane_to_id table at the id of the plane
            this.plane_to_id[new_plane_id] = this.id;
            // Send the plane status to all modules
            broadcast_plane_status(new_plane_id, new_plane_dep_time, this.id);

            // Set the LED color to note that a plane has landed
            led_set_color(LED_BLUE);
            sleep(2000);
        }
        // If the module is incoming
        if(plane_direction == 1){
            // Deal with tubs entering the system
            in();
        }
        // Set the LED color to note that the module will now be doing routing
        led_set_color(0xffffff);
        // Deal with routing of tubs
        loop();

        // Sleep for a bit to add reliability
        sleep(100);
    }
}