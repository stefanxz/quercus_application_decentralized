#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../functions/movement.c"
#include "../functions/rfid.c"
#include "../functions/network.c"
#include "../functions/graph.c"
// #include "../functions/algorithm.c"
#include "../pico_in/main.c"

#include <stdbool.h>

#define MAX_NUMBER_OF_TUBS 2
#define MAX_NUMBER_OF_PLANES 100
#define MAX_NUMBER_OF_MODULES 8

int get_message_type(char* msg){
	return msg[1];
}

export int main(void) {
	// Module module;
	// Module* module_ptr = &module;
	// enum EventType e;
	// char * msg;
	// subscribe_to_event(EVENT_MESSAGE_RECEIVED | EVENT_LASER_LEFT_DETECT | EVENT_LASER_RIGHT_DETECT);
	led_set_color(LED_RED);
	while(true) {
		// if(){
		// 	if(RFID_read_data_block(0, 1))
		// } 
		//{
		if(RFID_check_tag()){
			printf("RFID detected\n");
		}
		// in(e, module_ptr);
		sleep(50) ;
		//}
	}
	return 0;
}
