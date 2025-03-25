#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../functions/rfid.h"

void setup_plane(int plane_id, bool incoming){
    led_set_color(0x0000ff);
    char data[4];
    //SET TUB_PLANE
    data[0] = 0x1;
    RFID_write_data_block((int)data, 0);

    //SET PLANE_DROPOFF
    data[0] = -0x1;
    RFID_write_data_block((int)data, 1);

    //SET PLANE_ID
    data[0] = (char) plane_id;
    RFID_write_data_block((int)data, 2);

    //SET PAYLOAD
    data[0] = -0x1;
    RFID_write_data_block((int)data, 3);

    //SET DEPARTURE_TIME
    data[0] = 0x4;
    RFID_write_data_block((int)data, 4);

    //SET TUB_ID
    data[0] = -0x1;
    RFID_write_data_block((int)data, 5);

    //SET SECURITY
    data[0] = -0x1;
    RFID_write_data_block((int)data, 6);

    //SET PASSED_SECURITY
    data[0] = -0x1;
    RFID_write_data_block((int)data, 7);

    //SET PLANE_ARRIVED
    data[0] = -0x1;
    RFID_write_data_block((int)data, 8);

    //SET DESTINATION
    data[0] = -0x1;
    RFID_write_data_block((int)data, 9);

    //SET PLANE_DIRECTION
    data[0] = incoming;
    RFID_write_data_block((int)data, 10);
}

void setup_tub(bool plane_dropoff, int plane_id, 
        bool payload, int tub_id, bool sec_bit){
    led_set_color(0x0000ff);
    char data[4];
    //SET TUB_PLANE
    data[0] = 0x0;
    RFID_write_data_block((int)data, 0);

    //SET PLANE_DROPOFF
    data[0] = (char) plane_dropoff;
    RFID_write_data_block((int)data, 1);

    //SET PLANE_ID
    data[0] = (char) plane_id;
    RFID_write_data_block((int)data, 2);

    //SET PAYLOAD
    data[0] = (char) payload;
    RFID_write_data_block((int)data, 3);

    //SET DEPARTURE_TIME
    data[0] = -0x1;
    RFID_write_data_block((int)data, 4);

    //SET TUB_ID
    data[0] = (char) tub_id;
    RFID_write_data_block((int)data, 5);

    //SET SECURITY
    data[0] = (char) sec_bit;
    RFID_write_data_block((int)data, 6);

    //SET PASSED_SECURITY
    data[0] = 0x0;
    RFID_write_data_block((int)data, 7);

    //SET PLANE_ARRIVED
    data[0] = -0x1;
    RFID_write_data_block((int)data, 8);

    //SET DESTINATION
    data[0] = -0x1;
    RFID_write_data_block((int)data, 9);

    //SET PLANE_DIRECTION
    data[0] = -0x1;
    RFID_write_data_block((int)data, 10);
}

export int main(){
    printf("I am le writerr\n");
    while(true){
        //make plane
        if(RFID_check_tag()){
            led_set_color(0x00ff00);
            // setup_plane(4, 0);
            setup_tub(0, 4, 0, 1, 1);
        }
        sleep(100);
    }
}