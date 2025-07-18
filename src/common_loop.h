#pragma once

#include <stdbool.h>
#include <stdint.h> // For uint8_t

// Include other necessary headers that define types or constants used here
#include "defs.h"            // For Module, Task, Q_NULL, etc.
#include "common_hardware.h" // For functions like leave_at, enter_at, move_within_module, etc.
// #include "libc_builtin.h" // Only if it defines types or macros actually used in prototypes


// --- Utility Functions ---

/**
 * @brief Checks if the module is storing a tub.
 * @param module The module to check for stored tubs.
 * @return The ID of the index in the tub array that is currently storing a tub, or Q_NULL if no tub is stored.
 */
int8_t is_storing(const Module module);


// --- Task Management ---

/**
 * @brief Adds a task to the task ring buffer.
 * A task represents a specific movement or action for a tub within the module.
 * @param module Pointer to the module to which the task is added.
 * @param from The endpoint of the module from which the tub moves (e.g., RFID, LASER_LEFT).
 * @param to The endpoint of the module to which the tub moves (e.g., LASER_RIGHT, OUT).
 * @param request The related request message (uint8_t array) that led to the addition of this task.
 */
void add_task(Module* module, Direction from, Direction to, uint8_t* request);

/**
 * @brief Handles the initial routing and movement of tubs that are currently stored within the module.
 * This is typically triggered when the module is no longer "busy" or a plane arrives.
 * @param module Pointer to the module whose stored tubs are to be handled.
 */
void handle_storage(Module* module);

/**
 * @brief Handles incoming request movement messages, determining the tub's path.
 * This function reroutes tubs if their plane has arrived and they were previously destined for storage.
 * It also queues internal movements and sends the tub off to the next module if applicable.
 * @param module Pointer to the module receiving the request.
 * @param msg The incoming movement request message (uint8_t array).
 * @return 0 on success, or -1 if the request is invalid.
 */
int handle_request_movement(Module* module, uint8_t* msg);

/**
 * @brief Re-routes a tub that is currently stored within the module to a new destination.
 * This is typically used when a plane arrives and a stored tub can now be routed directly.
 * @param module Pointer to the module containing the stored tub.
 * @param from The current location/storage position of the tub within the module.
 * @param destination_id The unique identifier of the new destination (e.g., the plane's module ID).
 * @param dest_type The type of the new destination (e.g., PLANE).
 */
void reroute_stored_tub(Module* module, Direction from, uint8_t destination_id, DestinationType dest_type);


// --- Plane Status Handling ---

/**
 * @brief Handles the status of a plane (arrival or departure) received from the Pi module.
 * Updates the module's internal plane schedule and may trigger rerouting of stored tubs.
 * @param module Pointer to the module handling the plane status.
 * @param msg The incoming plane status message (uint8_t array).
 * @return 2 if it noted the plane arrival but is already busy and will not handle the arrival;
 *         1 if it can already reroute its tub towards the landed plane;
 *         0 if the plane was leaving;
 *         -1 if an error occurred with the plane schedule.
 */
int handle_plane_status(Module* module, uint8_t* msg);


// --- Message Handling ---

/**
 * @brief Waits for a message of a specific type and handles it accordingly.
 * It processes messages from the mailbox and dispatches them to appropriate handlers.
 * @param module Pointer to the current module.
 * @param msg_ptr Pointer to a pointer where the address of the received message will be stored.
 * @param expected The expected message type to wait for.
 * @param persistent If true, keeps waiting for messages until the expected one is received; otherwise, it checks once.
 * @return Returns a response code based on the message type received and its handling (e.g., 0 for success, -1 for error).
 */
int await_message(Module* module, uint8_t** msg_ptr, int expected, bool persistent);

/**
 * @brief Clears the data for a given tub structure, effectively making it "empty".
 * @param tub Pointer to the Tub structure to clear.
 */
void clear_tub_data(Tub* tub);

/**
 * @brief Waits for a confirmation response from another module after sending a request.
 * It attempts to receive a `REQUEST_RESPONSE` message.
 * @param module Pointer to the current module waiting for the response.
 * @return True if a positive response is received, false otherwise (e.g., timeout).
 */
bool await_response(Module* module);

/**
 * @brief Sends a request to a neighboring module to allow a tub to leave.
 * This function handles retries and waiting for a response, ensuring a successful handoff.
 * @param module Pointer to the current module.
 * @param next_id The ID of the module to which the tub intends to leave.
 * @param request The request message for the tub's movement.
 * @return True if the request is successfully accepted by the next module, false otherwise.
 */
bool request_to_leave(Module* module, int next_id, char* request);

/**
 * @brief Saves relevant tub data from a request message into a Tub structure.
 * This is used to initialize or update a tub's details based on an incoming movement request.
 * @param tub Pointer to the Tub structure where data will be saved.
 * @param req The request message (char array) containing the tub's details.
 */
void save_tub_from_request(Tub* tub, char req[REQ_LENGTH]);

/**
 * @brief Executes a single task from the module's task queue.
 * This function orchestrates the physical movements and communication required for a task (tub movement, entry, exit).
 * @param module Pointer to the module that will execute the task.
 * @return True if the task was successfully initiated/completed, false if it encountered an issue (e.g., failed to get response).
 */
bool do_task(Module* module);

/**
 * @brief The main operational loop for the module.
 * It continuously checks for new messages and processes existing tasks.
 * @param module Pointer to the module instance running the loop.
 */
void loop(Module* module);
