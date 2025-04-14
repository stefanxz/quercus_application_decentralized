#include "../lib/pico_routing.c"
#include "../lib/rfid.c"
#include "../lib/pico_plane.c"

/// @brief checks the payload of a tub, determines its destination and resets the should_check value.
/// @return the destination type of the tub after the check.
uint8_t security_check(bool* should_check) {
    printf("I am checking the tub, %d.\n", get_rfid_data(TUB_ID));

    // Consume the should_check token
    *should_check = 0;

    // Set that the tub has passed security
    this.tub[RFID].passed_security = 1;
    // Get the plane id of the tub
    int plane_id = get_rfid_data(PLANE_ID);

    // If the payload is "safe", so not 0
    if(get_rfid_data(PAYLOAD)) {
        // If the tub is going to drop-off, set the destination to the nearest drop-off
        if(get_rfid_data(PLANE_OR_DROPOFF)){
            this.tub[RFID].destination_id = this.nearest[DROPOFF];
            this.tub[RFID].destination_type = DROPOFF;
            printf("I am rerouting to Drop-Off.\n");
            return DROPOFF;
        }

        // If the plane of the tub has arrived, set the destination to the plane
        if(check_plane_arrived(plane_id)){
            this.tub[RFID].destination_id = this.plane_to_id[plane_id];
            this.tub[RFID].destination_type = PLANE;
            this.tub[RFID].plane_id = plane_id;
            printf("I am rerouting to plane %d on module %d.\n", this.tub[RFID].destination_id, this.tub[RFID].destination_type);
            return PLANE;
        }

        // If the plane of the tub has not arrived, set the destination to the nearest storage
        this.tub[RFID].destination_id = this.nearest[STORAGE];
        this.tub[RFID].destination_type = STORAGE;
        printf("I am rerouting to Storage. \n");
        return STORAGE;
    }
    // If the payload is "unsafe", so 0, rerout the tub to quarantine
    this.tub[RFID].destination_id = this.nearest[QUARANTINE];
    this.tub[RFID].destination_type = QUARANTINE;
    printf("I am rerouting to Quarantine.\n");
    return QUARANTINE;
}

/// @brief Reroutes the tub from the security module to its destination after passing the security check.
void reroute_from_security(){
    // Create a request to send to the next module
    char request[REQ_LENGTH];
    request[MSG_SENDER] = this.id;
    request[MSG_TYPE] = REQUEST_MOVEMENT;
    request[REQ_DEST_ID] = this.tub[RFID].destination_id;
    request[REQ_DEST_TYPE] = this.tub[RFID].destination_type;
    request[REQ_TUB_ID] = this.tub[RFID].id;
    request[REQ_PLANE_ID] = this.tub[RFID].plane_id;
    
    // Determine the direction within the module, leading to the destination
    Direction to = this.dir_lookup[this.tub[RFID].destination_id];

    // Reroute the tub to the direction towards the destination
    printf("I am rerouting to dir: %d with final destination %d and destination type %d \n", to, request[REQ_DEST_ID], request[REQ_DEST_TYPE]);
    add_task(RFID, to, request);
    add_task(to, OUT, request);
}


export int main(void) {
    // Initialize the module
    init();
    // Initialize the should_check variable to 0
    this.should_check = 0;

    // Loop infinitely, checking if a tub should go to a security check and routing it accordingly
    while(1){
        // If a security check should be done to the tub
        if(this.should_check){
            printf("I get to security\n");
            // If there is a tub on the RFID reader
            if(RFID_check_tag()){
                // Do a security check on the tub and reroute it to its destination
                security_check(&this.should_check);
                reroute_from_security();
            }
        }
        // Deal with general routing of tubs
        loop();

        // Sleep for a bit to add reliability
        sleep(100);
    }
}