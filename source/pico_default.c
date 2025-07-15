#include "quercus_lib_pico.h"
#include "common_init.c"
#include "common_loop.c"

/// @brief The code for a default routing module. Loops infintely, doing the loop()
export int main(void) {
    //initialize the module
    Module module = module_init();

    while(1){
        //deal with routing of tubs
        loop(&module);

        //sleep for a bit to add reliability
        sleep(100);
    }
}
