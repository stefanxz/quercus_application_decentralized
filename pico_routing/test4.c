#include "main.c"
#include "test_constants.h"

Module this;
State state;
Request current_request;

export int main(void) {
    init(&T4);
    loop();
}