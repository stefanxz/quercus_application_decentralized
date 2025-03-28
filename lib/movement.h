#pragma once

#include "../libc_builtin.h"
#include "../quercus_lib_pico.h"

const int ARM_LEFT = 60;
const int ARM_RIGHT = 105;
const int ARM_NEUTRAL = 0;

const int LED_RED = 0xff0000;
const int LED_GREEN = 0x00ff00;
const int LED_BLUE = 0x0000ff;

const int BELT_OFF = 0;
const int BELT_LEFT_SLOW = 20;
const int BELT_RIGHT_SLOW = -20;
const int BELT_DOWN_SLOW = 20;
const int BELT_UP_SLOW = -20;

// Stops the belts, resets the arm and turn the LED to red.
void reset_module();

// Moves Tub within a Hardware Module from the position of start to dest,
// pass along the tubId for the location to be updated.
void move_within_module(int tub_id, int start, int dest);

// Moves Tub within a Hardware Module from the position of exit_point to the Hardware Module with moduleId,
// let requestData send the tub data.
int move_to_neighbour(int module_id, int exit_point, char* tub_data);