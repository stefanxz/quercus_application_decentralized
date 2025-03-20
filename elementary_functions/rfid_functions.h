#include "../quercus_lib_pico.h"
#include "../libc_builtin.h"
#include <stdbool.h>

const int BLOCK_SIZE = 4;

// Checks whether a Tub needs to go through security.
// Returns 1 if Tub needs to pass security, if Tub does not it returns 0.
int get_security_flag();

// Checks whether a Tub needs to got to a plane or to dropoff.
// Returns 1 if Tub needs to got to a plane, and returns 0 if Tub must go to a dropoff.
int get_plane_dropoff_flag();

// Checks the id of the plane that the Tub is assigned to.
// Permitted id values range from 0 to 255.
int get_plane_id();

// Im not sure yet.
int get_payload();

// Checks whether a Tub has already passed through security.
// Returns 1 if Tub still needs to pass security, if Tub has already passed security it returns 0.
int has_security_been_passed();

// Checks whether the Plane the Tub is assigned to has already arrived.
// Returns 1 if the assigned plane has arrived, if the assigned plane has not yet arrived it returns 0.
int has_plane_arrived();

// Checks the id of the Hardware Module that the Tub has currently set as its destination.
// Permitted id values range from 0 to 255.
int get_destination();

// Updates whether a Tub has already passed through security.
int set_security_passed();

// Updates whether the Plane the Tub is assigned to has already arrived.
int set_plane_arrived();

// Updates the id of the Hardware Module that the Tub has currently set as its destination.
int set_destination(int dest);
