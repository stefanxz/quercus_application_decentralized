#include "../elementary_functions/pico_routing.c"

/// @brief The code for a default routing module. Loops infintely, doing the loop()
export int main(void) {
    //initialize the module
    init();

    while(1){
        //deal with routing of tubs
        loop();

        //sleep for a bit to add reliability
        sleep(100);
    }
}
