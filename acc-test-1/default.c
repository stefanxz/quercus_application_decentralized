#include "../pico_routing.c"

export int main(void) {
	reset_module();
    init();
    while(1){
        loop();
        sleep(100);
    }
}
