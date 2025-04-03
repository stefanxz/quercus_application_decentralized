#include "../pico_plane.c"

/// @brief Checks if there is a plane on the RFID reader.
/// @return 1 if there is, 0 if there is nothing, -1 if it's a tub.
int check_for_plane(){
    if(RFID_check_tag()){
        if (get_rfid_data(TUB_OR_PLANE)) {
            return 1;
		}
    }
    return 0;
}
int8_t plane_direction = 0; //-1 for outgoing, 0 for no plane, 1 for incoming plane

export int main(void) {
	reset_module();
    init();
    led_set_color(0xff00000);
    while(1){
        int8_t new_gate_status = check_for_plane();
        if(plane_direction && new_gate_status == 1) {
            int new_plane_id = get_rfid_data(PLANE_ID);
            printf("Detecting plane: %d\n", new_plane_id);
            if(this.plane_to_id[new_plane_id] == this.id){
                plane_direction = 0;
                this.plane_to_id[new_plane_id] = 0;
                broadcast_plane_status(new_plane_id, -1, this.id);

                led_set_color(LED_BLUE);
                sleep(2000);
            } else {
                printf("There is already a plane here, you may not add a new one.\n");
            }
        } else if(!plane_direction && new_gate_status == 1) {
            printf("Plane is not here and I am detecting a new one.\n");
            int new_plane_id = get_rfid_data(PLANE_ID);
            int new_plane_dep_time = get_rfid_data(DEPARTURE_TIME);
            if(get_rfid_data(PLANE_DIRECTION) == 0) plane_direction = -1;
            else plane_direction = 1;
            this.plane_to_id[new_plane_id] = this.id;
            broadcast_plane_status(new_plane_id, new_plane_dep_time, this.id);

            led_set_color(LED_BLUE);
            sleep(2000);
        }
        if(plane_direction == 1){
            in(&this);
        }
        led_set_color(0xffffff);
        loop();
        sleep(100);
    }
}