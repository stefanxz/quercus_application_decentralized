#include "../pico_routing.c"
#include "../lib/rfid.c"
#include "../pico_plane.c"

/// @brief checks the payload of a tub, determines its destination and resets the should_check value.
/// @return the destination type of the tub after the check.
uint8_t security_check(bool* should_check) {
    printf("I am checking the tub, %d.\n", get_rfid_data(TUB_ID));
    //Implement these comments once tubs aren't consumable.
    // char data[4];
    // data[0] = 1;
    // RFID_write_data_block(PASSED_SECURITY, data);
    // data[0] = !get_rfid_data(PAYLOAD);
    // RFID_write_data_block(SECURITY, data);
    *should_check = 0;
    this.tub[RFID].passed_security = 1;
    if(get_rfid_data(PAYLOAD)) {
        if(get_rfid_data(PLANE_OR_DROPOFF)){
            this.tub[RFID].destination_id = this.nearest[DROPOFF];
            this.tub[RFID].destination_type = DROPOFF;
            return DROPOFF;
        }
        if(check_plane_arrived(get_rfid_data(PLANE_ID))){
            this.tub[RFID].destination_id = this.id_lookup[this.plane_to_id[PLANE_ID]];
            this.tub[RFID].destination_type = PLANE;
            return PLANE;
        }
        this.tub[RFID].destination_id = this.nearest[STORAGE];
        this.tub[RFID].destination_type = STORAGE;
        return STORAGE;
    }
    this.tub[RFID].destination_id = this.nearest[QUARANTINE];
    this.tub[RFID].destination_type = QUARANTINE;
    return QUARANTINE;
}


void reroute_from_security(){
    char request[REQ_LENGTH];
    request[MSG_SENDER] = this.id;
    request[MSG_TYPE] = this.tub[RFID].destination_type;
    request[REQ_DEST_ID] = this.tub[RFID].destination_id;
    request[REQ_DEST_TYPE] = this.tub[RFID].destination_type;
    Direction to = this.dir_lookup[this.tub[RFID].destination_id];
    // handle_request_movement(request);
    add_task(RFID, to, request);
    add_task(to, OUT, request);
}


export int main(void) {
    init();
    this.should_check = 0;
    while(1){
        if(this.should_check){
            printf("I get to security\n");
            if(RFID_check_tag()){
                security_check(&this.should_check);
                reroute_from_security();
            }
        }
        loop();
        sleep(100);
    }
}