// #include "../quercus_lib_pi.h"

// #include "../libc_builtin.h"

// export int main(void) {
// 	print("boo from triple updated WASM\n");
// 	subscribe_to_event(EVENT_LASER_LEFT_DETECT | EVENT_MESSAGE_RECEIVED | EVENT_ENCODER_LONG_STOPPED |
// 					   EVENT_LASER_RIGHT_DETECT);
// 	print("event test!");
// 	while (1) {
// 		print("Hello from triple updated WASM\n");
// 		// volatile int c = a / b;

// 		enum EventType e;
// 		while ((e = next_event())) {
// 			if (e == EVENT_LASER_LEFT_DETECT) {
// 				print("laser!\n");
// 			} else if (e == EVENT_MESSAGE_RECEIVED) {
// 				int addr;
// 				int len = next_message_address(&addr);
// 				printf("network event: %.*s\n", len, (char*)addr);
// 			} else if (e == EVENT_MESSAGE_ALLOC_FAILED) {
// 				print("alloc failed\n");
// 			} else if (e == EVENT_NONE) {
// 				// skip
// 			} else {
// 				printf("unknown! - %d\n", e);
// 			}
// 		}
// 		sleep(1000);
// 	}
// }
