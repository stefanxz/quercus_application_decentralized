#include "../libc_builtin.h"
#include "../quercus_lib_pico.h"

#include "rfid.c"

void setup_plane(int plane_id, bool incoming) {
	led_set_color(0x0000ff);
	char data[4];
	// SET TUB_PLANE
	data[0] = 0x1;
	RFID_write_data_block((int)data, TUB_OR_PLANE);

	// SET PLANE_DROPOFF
	data[0] = -0x1;
	RFID_write_data_block((int)data, PLANE_OR_DROPOFF);

	// SET PLANE_ID
	data[0] = (char)plane_id;
	RFID_write_data_block((int)data, PLANE_ID);

	// SET PAYLOAD
	data[0] = -0x1;
	RFID_write_data_block((int)data, PAYLOAD);

	// SET DEPARTURE_TIME
	data[0] = 0x4;
	RFID_write_data_block((int)data, DEPARTURE_TIME);

	// SET TUB_ID
	data[0] = -0x1;
	RFID_write_data_block((int)data, TUB_ID);

	// SET SECURITY
	data[0] = -0x1;
	RFID_write_data_block((int)data, NEEDS_SECURITY);

	// SET PASSED_SECURITY
	data[0] = -0x1;
	RFID_write_data_block((int)data, PASSED_SECURITY);

	// SET PLANE_ARRIVED
	data[0] = -0x1;
	RFID_write_data_block((int)data, PLANE_ARRIVED);

	// SET DESTINATION
	data[0] = -0x1;
	RFID_write_data_block((int)data, DESTINATION);

	// SET PLANE_DIRECTION
	data[0] = incoming;
	RFID_write_data_block((int)data, PLANE_DIRECTION);
}

void setup_tub(int tub_id, int needs_security, int plane_dropoff, int plane_id, int payload) {
	led_set_color(0x0000ff);
	char data[4];
	// SET TUB_PLANE
	data[0] = 0x0;
	RFID_write_data_block((int)data, TUB_OR_PLANE);

	// SET PLANE_DROPOFF
	data[0] = (char)plane_dropoff;
	RFID_write_data_block((int)data, PLANE_OR_DROPOFF);

	// SET PLANE_ID
	data[0] = (char)plane_id;
	RFID_write_data_block((int)data, PLANE_ID);

	// SET PAYLOAD
	data[0] = (char)payload;
	RFID_write_data_block((int)data, PAYLOAD);

	// SET DEPARTURE_TIME
	data[0] = -0x1;
	RFID_write_data_block((int)data, DEPARTURE_TIME);

	// SET TUB_ID
	data[0] = (char)tub_id;
	RFID_write_data_block((int)data, TUB_ID);

	// SET SECURITY
	data[0] = (char)needs_security;
	RFID_write_data_block((int)data, NEEDS_SECURITY);

	// SET PASSED_SECURITY
	data[0] = 0x0;
	RFID_write_data_block((int)data, PASSED_SECURITY);

	// SET PLANE_ARRIVED
	data[0] = 0x0;
	RFID_write_data_block((int)data, PLANE_ARRIVED);

	// SET DESTINATION
	data[0] = 0x0;
	RFID_write_data_block((int)data, DESTINATION);

	// SET PLANE_DIRECTION
	data[0] = -0x1;
	RFID_write_data_block((int)data, PLANE_DIRECTION);

	// SET TUB ID
	data[0] = (char)tub_id;
	RFID_write_data_block((int)data, TUB_ID);
}

int write() {
	printf("I am le writerr\n");
    int i = 0;
	while (true) {
		// make plane
    if (RFID_check_tag()) {
        led_set_color(0x00ff00);
        switch (i) {
        case 0:
            // Plane 1
            setup_plane(69, 1);
            printf("setup plane 1");
            break;
        case 1:
            // Plane 2
            setup_plane(17, 0);
            printf("setup plane 2");
            break;
        case 2:
            // Tub 1
            setup_tub(1, 0, 0, 17, 1);
            printf("setup tub 1");
            break;
        case 3:
            // Tub 2
            setup_tub(2, 0, 0, 17, 1);
            printf("setup tub 2");
            break;
        case 4:
            // Tub 3
            setup_tub(3, 0, 1, 17, 1);
            printf("setup tub 3");
            break;
        case 5:
            // Tub 4
            setup_tub(4, 1, 1, 17, 1);
            printf("setup tub 4");
            break;
        case 6:
            // Tub 5
            setup_tub(5, 1, 0, 17, 1);
            printf("setup tub 5");
            break;
        case 7:
            // Tub 6
            setup_tub(6, 1, 0, 17, 1);
            printf("setup tub 6");
            break;
        case 8:
            // Tub 7
            setup_tub(7, 1, 1, 17, 1);
            printf("setup tub 7");
            break;
        case 9:
            // Tub 8
            setup_tub(8, 0, 0, 17, 1);
            printf("setup tub 8");
            break;
        case 10:
            // Tub 9
            setup_tub(9, 0, 0, 17, 1);
            printf("setup tub 9");
            break;
        case 11:
            // Tub 10
            setup_tub(10, 1, 0, 17, 0);
            printf("setup tub 10");
            break;
        case 12:
            // Tub 11
            setup_tub(11, 1, 0, 17, 1);
            printf("setup tub 11");
            break;
        case 13:
            // Tub 12
            setup_tub(12, 1, 0, 17, 1);
            printf("setup tub 12");
            break;
        default:
            // Should never reach here
            break;
    }
        i++;
        // set led to smth else
        led_set_color(0x0000ff);
        // wait for 5 seconds
        sleep(3000);
	}
	sleep(100);
	}
	led_set_color(0x0000ff);
}
int read_tub(){
    led_set_color(0x0000ff);
	char data[RFID_LENGTH];
	// SET TUB_PLANE
    get_entrance_rfid_data(data);
    printf("1: %d\n, 2: %d\n,3: %d\n,4: %d\n,5: %d\n,6: %d\n,7: %d\n,8: %d\n,9: %d\n,10: %d\n,11: %d\n", 
    data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10]);
}

export int main(){
    while(true){
        if(RFID_check_tag()){
            led_set_color(0x00ff00);
            setup_tub(10, 1, 0, 17, 0);
            printf("I have finished setting up\n");
            read_tub();
            break;
        }
    }   
}