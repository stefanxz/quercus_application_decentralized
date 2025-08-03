#pragma once

#include <stdbool.h> // For bool type
#include <stdint.h>  // For uint8_t

// Include other necessary headers that define types or constants used here
#include "quercus_lib_pico.h" // For hardware interaction functions
#include "defs.h"             // For module structures, constants like DATA_RFID_LENGTH, etc.
// #include "libc_builtin.h"  // Only if it defines types or macros actually used in prototypes


// --- Hardware Control and Basic Module Operations ---

/**
 * @brief Reset the module to its default state: LED red, belts off, arm neutral.
 */
void reset_module(void);

/**
 * @brief Reads a single byte of DIR_RFID data from the specified data block type.
 * @param type The data block index/type to read from.
 * @return The first byte of the specified DIR_RFID data block, or -1 if no tag is detected.
 */
int get_rfid_data(int type);

/**
 * @brief Reads multiple DIR_RFID data blocks (up to DATA_RFID_LENGTH) and stores the first byte of each into rfid_data.
 * @param rfid_data A pointer to a buffer that will store the read bytes.
 * @return 0 on success, or -1 if no tag is detected.
 */
int get_entrance_rfid_data(char* rfid_data);

/**
 * @brief Writes a tub ID to the DIR_RFID tag.
 * @param tub_id The tub ID to be written.
 * @return 0 on success, or -1 if no tag is detected.
 */
int set_tub_id(int tub_id);

/**
 * @brief Writes a security flag to the DIR_RFID tag.
 * (Note: This function appears to write to DATA_TUB_ID block, which might be a typo
 * if it's meant for a general security flag block.)
 * @param flag The security flag to be written.
 * @return 0 on success, or -1 if no tag is detected.
 */
int set_security_flag(int flag);

/**
 * @brief Sets the 'security passed' flag if the DIR_RFID tag indicates security is needed.
 * @param flag The value to set for the security-passed indicator.
 * @return 0 on success, -1 if no tag is detected, or -2 if the 'DATA_NEEDS_SECURITY' block indicates no security requirement.
 */
int set_security_passed(int flag);

/**
 * @brief Marks that a plane has arrived if the DIR_RFID tag indicates a plane or drop-off scenario.
 * @return 0 on success, -1 if no tag is detected, or -2 if 'DATA_PLANE_OR_DROPOFF' block is not set.
 */
int set_plane_arrived(void);

/**
 * @brief Writes a destination value to the DIR_RFID tag.
 * @param dest The destination code to be written.
 * @return 0 on success, or -1 if no tag is detected.
 */
int set_destination(int dest);

/**
 * @brief Wiggles the arm to potentially free a stuck tub.
 */
void the_wiggler(void);

/**
 * @brief Performs a small wiggle of the arm.
 */
void wiggle(void);

/**
 * @brief Move the tub within the module from one endpoint to another.
 * This function also handles basic timeout and arm wiggling for stuck tubs.
 * @param from The starting endpoint (DIR_LASER_LEFT, DIR_LASER_RIGHT, DIR_RFID).
 * @param to The destination endpoint (DIR_LASER_LEFT, DIR_LASER_RIGHT, DIR_RFID).
 */
void move_within_module(int from, int to);

/**
 * @brief Push a tub out of the module at a specific exit point.
 * @param exit The exit point from which the tub leaves (DIR_LASER_LEFT, DIR_LASER_RIGHT, DIR_OUT).
 *             (Note: The comment implies DIR_RFID, but DIR_RFID is an entry point. Check usage.)
 */
void leave_at(int exit);

/**
 * @brief Take on a tub at a specific entrance point.
 * @param entrance The entrance point where the tub enters (DIR_LASER_LEFT, DIR_LASER_RIGHT, DIR_RFID).
 */
void enter_at(int entrance);


// --- Communication Functions ---

/**
 * @brief Send a request for movement to another module.
 * @param module_id ID of the module to which the request is sent.
 * @param data The request data to be sent (expected to be of length REQ_LENGTH).
 * @return 0 for success, < 0 for failure.
 */
int send_request_movement(int module_id, char* data);

// TODO: delete this function, it is not used anywhere relevant
int send_updated_tub_location(int tub_id, int location_belt);

/**
 * @brief Send a message that a plane has arrived or departed to the Pi module.
 * @param plane_id The ID of the plane.
 * @param arrival_status The status of the plane (true for arrival, false for departure).
 * @return 0 for success, < 0 for failure.
 */
int send_plane_status(int plane_id, bool arrival_status);

// TODO: delete this function, it is not used anywhere relevant
int send_tub_status(char* tub_data);

/**
 * @brief Send a message containing a response to another module's request.
 * @param module_id The ID of the module to which the response is sent.
 * @param value The value to be sent in the response (0 for failure, 1 for success).
 * @return 0 for success, < 0 for failure.
 */
int send_request_response(int module_id, int value);

/**
 * @brief Send a request to receive your path configuration to the Pi module.
 * @return 0 for success, < 0 for failure.
 */
int send_request_path_config(void);
