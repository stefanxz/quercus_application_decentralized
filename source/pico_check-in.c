#include "quercus_lib_pico.h"
#include "common_init.c"
#include "common_loop.c"
/// @brief The code for a check-in module. Loops infintely, doing the loop() and in().
export int main(void) {
    //initialize the module
    Module module = module_init();

    while(1){
        //check if there is a tub on the RFID reader
        handle_tub_rfid_entry(&module);

        //deal with routing of tubs
        loop(&module);

        //sleep for a bit to add reliability
        sleep(100);
    }
}
