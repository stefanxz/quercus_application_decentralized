#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"
#include <stdbool.h>

const int ARM_LEFT = 60;
const int ARM_RIGHT = 105;
const int ARM_NEUTRAL = 0;

const int LED_GREEN = 0x00ff00;
const int LED_BLUE = 0x0000ff;

const int BELT_OFF = 0;
const int BELT_LEFT_SLOW = 20;
const int BELT_RIGHT_SLOW = -20;
const int BELT_DOWN_SLOW = 20;
const int BELT_UP_SLOW = -20;

// Moves Tub within a Hardware Module from the position of the right laser sensor to the position of the RFID scanner.
void laser_right_to_rfid();

// Moves Tub within a Hardware Module from the position of the left laser sensor to the position of the RFID scanner.
void laser_left_to_rfid();

// Moves Tub within a Hardware Module from the position of the right laser sensor to the position of the left laser sensor.
void laser_right_to_laser_left();

// Moves Tub within a Hardware Module from the position of the left laser sensor to the position of the right laser sensor.
void laser_left_to_laser_right();

// Moves Tub within a Hardware Module from the position of the RFID scanner to the position of the right laser sensor.
void rfid_to_laser_right();

// Moves Tub within a Hardware Module from the position of the RFID scanner to the position of the left laser sensor.
void rfid_to_laser_left();

// Swaps 2 Tubs in a Hardare Module such that the Tub at the position of the right laser sensor ends up at the position of 
// the RFID scanner and the Tub at the position of the RFID scanner ends up at the position of the right laser sensor.
void swap_laser_right_and_rfid();

// Swaps 2 Tubs in a Hardare Module such that the Tub at the position of the left laser sensor ends up at the position of 
// the RFID scanner and the Tub at the position of the RFID scanner ends up at the position of the left laser sensor.
void swap_laser_left_and_rfid();

