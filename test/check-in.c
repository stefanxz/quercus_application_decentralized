#include "../pico_routing.c"
#include "../pico_plane.c"
#include "../examples.c"

export int main(void) {
    init();

    while(1){
        in();
        loop();
        sleep(100);
    }
}