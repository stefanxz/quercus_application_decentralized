#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "./rfid.h"

void setup_plane(int plane_id, bool incoming){
    led_set_color(0x0000ff);
    char data[4];
    //SET TUB_PLANE
    data[0] = 0x1;
    RFID_write_data_block((int)data, TUB_OR_PLANE);

    //SET PLANE_DROPOFF
    data[0] = -0x1;
    RFID_write_data_block((int)data, PLANE_OR_DROPOFF);

    //SET PLANE_ID
    data[0] = (char) plane_id;
    RFID_write_data_block((int)data, PLANE_ID);

    //SET PAYLOAD
    data[0] = -0x1;
    RFID_write_data_block((int)data, PAYLOAD);

    //SET DEPARTURE_TIME
    data[0] = 0x4;
    RFID_write_data_block((int)data, DEPARTURE_TIME);

    //SET TUB_ID
    data[0] = -0x1;
    RFID_write_data_block((int)data, TUB_ID);

    //SET SECURITY
    data[0] = -0x1;
    RFID_write_data_block((int)data, NEEDS_SECURITY);

    //SET PASSED_SECURITY
    data[0] = -0x1;
    RFID_write_data_block((int)data, PASSED_SECURITY);

    //SET PLANE_ARRIVED
    data[0] = -0x1;
    RFID_write_data_block((int)data, PLANE_ARRIVED);

    //SET DESTINATION
    data[0] = -0x1;
    RFID_write_data_block((int)data, DESTINATION);

    //SET PLANE_DIRECTION
    data[0] = incoming;
    RFID_write_data_block((int)data, PLANE_DIRECTION);
}

void setup_tub(bool plane_dropoff, int plane_id, 
        bool payload, int tub_id, bool sec_bit){
    led_set_color(0x0000ff);
    char data[4];
    //SET TUB_PLANE
    data[0] = 0x0;
    RFID_write_data_block((int)data, TUB_OR_PLANE);

    //SET PLANE_DROPOFF
    data[0] = (char) plane_dropoff;
    RFID_write_data_block((int)data, PLANE_OR_DROPOFF);

    //SET PLANE_ID
    data[0] = (char) plane_id;
    RFID_write_data_block((int)data, PLANE_ID);

    //SET PAYLOAD
    data[0] = (char) payload;
    RFID_write_data_block((int)data, PAYLOAD);

    //SET DEPARTURE_TIME
    data[0] = -0x1;
    RFID_write_data_block((int)data, DEPARTURE_TIME);

    //SET TUB_ID
    data[0] = (char) tub_id;
    RFID_write_data_block((int)data, TUB_ID);

    //SET SECURITY
    data[0] = (char) sec_bit;
    RFID_write_data_block((int)data, NEEDS_SECURITY);

    //SET PASSED_SECURITY
    data[0] = 0x0;
    RFID_write_data_block((int)data, PASSED_SECURITY);

    //SET PLANE_ARRIVED
    data[0] = 0x0;
    RFID_write_data_block((int)data, PLANE_ARRIVED);

    //SET DESTINATION
    data[0] = 0x0;
    RFID_write_data_block((int)data, DESTINATION);

    //SET PLANE_DIRECTION
    data[0] = -0x1;
    RFID_write_data_block((int)data, PLANE_DIRECTION);
}

int write(){
    printf("I am le writerr\n");
    while(true){
        //make plane
        if(RFID_check_tag()){
            led_set_color(0x00ff00);
            // setup_plane(3, 1);
            setup_tub(1, -1, 1, 2, 1);
            return 0;
        }
        sleep(100);
    }
    led_set_color(0x0000ff);
}