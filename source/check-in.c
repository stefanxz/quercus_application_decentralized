#include "../elementary_functions/pico_plane.c"

/// @brief The code for a check-in module. Loops infintely, doing the loop() and in(). 
export int main(void) {
    //initialize the module
    init();

    while(1){
        //check if there is a tub on the RFID reader
        in();
        
        //deal with routing of tubs
        loop();

        //sleep for a bit to add reliability
        sleep(100);
    }
}