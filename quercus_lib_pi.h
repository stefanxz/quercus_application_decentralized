#define export __attribute__((visibility("default"))) __attribute__((used))

typedef enum EventType {
	EVENT_NONE = 0,
	EVENT_LASER_LEFT_DETECT = 1,
	EVENT_LASER_RIGHT_DETECT = 2,
	EVENT_ENCODER_LONG_STOPPED = 4,
	EVENT_ENCODER_SHORT_STOPPED = 8,
	EVENT_CURRENT_THRESHOLD_REACHED = 16,
	EVENT_POWER_THRESHOLD_REACHED = 32,
	EVENT_MESSAGE_RECEIVED = 64, // special event, needs buffer for data
	EVENT_MESSAGE_ALLOC_FAILED = 128,
} EventType;

extern void print(const char* str);
extern void sleep(int ms);

void subscribe_to_event(int event_type);

void unsubscribe_from_event(int event_type);

enum EventType next_event();

/*
 * Call this function after receiving an EVENT_MESSAGE_RECEIVED event from next_event()
 *
 *
 * The address of the buffer containing the content of the message will be written to
 * address_ptr. The length of the message is returned by this function.
 * If no message is pending, returns 0;
 * If the network event had resulted in a failed allocation, and there was no
 * memory left in the WAMR heap, then returns -1.
 * Other errors are other negative numbers.
 */
// int next_message_address(int* address_ptr);
