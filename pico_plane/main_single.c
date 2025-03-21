#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"

#include "../elementary_functions/movement_functions.c"
#include "../elementary_functions/rfid_functions.c"
#include "../algo_functions.c"

#include <stdbool.h>
#include <stdio.h>

#define MAX_NUMBER_OF_TUBS 2
#define MAX_NUMBER_OF_PLANES 100
#define MAX_NUMBER_OF_MODULES 8

int get_message_type(char* msg){
	return msg[1];
}

int main(void) {
	Module module;
	Module* module_ptr = &module;
	enum EventType e;
	char * msg;
	subscribe_to_event(EVENT_MESSAGE_RECEIVED | EVENT_RFID_DETECT | EVENT_LASER_LEFT_DETECT | EVENT_LASER_RIGHT_DETECT);
	while(true) {
		e = next_event();
		//if RFID detected plane
		//{
		in(e, module_ptr);
		sleep(10);
		//}
	}
	return 0;
}
