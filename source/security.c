#include "../lib/pico_routing.c"
#include "../lib/rfid.c"
#include "../lib/pico_plane.c"

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
    int plane_id = get_rfid_data(PLANE_ID);
    if(get_rfid_data(PAYLOAD)) {
        if(get_rfid_data(PLANE_OR_DROPOFF)){
            this.tub[RFID].destination_id = this.nearest[DROPOFF];
            this.tub[RFID].destination_type = DROPOFF;
            printf("I am rerouting to Drop-Off.\n");
            return DROPOFF;
        }
        if(check_plane_arrived(plane_id)){
            this.tub[RFID].destination_id = this.plane_to_id[plane_id];
            this.tub[RFID].destination_type = PLANE;
            this.tub[RFID].plane_id = plane_id;
            printf("I am rerouting to plane %d on module %d.\n", this.tub[RFID].destination_id, this.tub[RFID].destination_type);
            return PLANE;
        }
        this.tub[RFID].destination_id = this.nearest[STORAGE];
        this.tub[RFID].destination_type = STORAGE;
        printf("I am rerouting to Storage. \n");
        return STORAGE;
    }
    this.tub[RFID].destination_id = this.nearest[QUARANTINE];
    this.tub[RFID].destination_type = QUARANTINE;
    printf("I am rerouting to Quarantine.\n");
    return QUARANTINE;
}


void reroute_from_security(){
    char request[REQ_LENGTH];
    request[MSG_SENDER] = this.id;
    request[MSG_TYPE] = REQUEST_MOVEMENT;
    request[REQ_DEST_ID] = this.tub[RFID].destination_id;
    request[REQ_DEST_TYPE] = this.tub[RFID].destination_type;
    request[REQ_TUB_ID] = this.tub[RFID].id;
    request[REQ_PLANE_ID] = this.tub[RFID].plane_id;
    Direction to = this.dir_lookup[this.tub[RFID].destination_id];

    printf("I am rerouting to dir: %d with final destination %d and destination type %d \n", to, request[REQ_DEST_ID], request[REQ_DEST_TYPE]);
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