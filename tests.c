/**
 * Copyright (c) 2023 Nicholas Daniell
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "state_machine.h"


state_id_t state_init_event_handler(event_t event) {
    printf("State init\n");
    return STATE_MACHINE_STATE_RUN;
}

state_id_t state_run_event_handler(event_t event) {
    printf("State run\n");
    return STATE_MACHINE_STATE_ERROR;
}

state_id_t state_error_event_handler(event_t event) {
    printf("State error\n");
    return STATE_MACHINE_STATE_INIT;
}

int simple_test(void) {
    state_machine_t* state_machine = state_machine_create();
    state_machine->state_table[STATE_MACHINE_STATE_INIT].state_event_handler = state_init_event_handler;
    state_machine->state_table[STATE_MACHINE_STATE_RUN].state_event_handler = state_run_event_handler;
    state_machine->state_table[STATE_MACHINE_STATE_ERROR].state_event_handler = state_error_event_handler;
    state_machine_event(state_machine, (event_t){0, 0, NULL});
    state_machine_event(state_machine, (event_t){0, 0, NULL});
    state_machine_event(state_machine, (event_t){0, 0, NULL});
    state_machine_event(state_machine, (event_t){0, 0, NULL});

    state_machine_init(state_machine);
    state_machine_destroy(state_machine);
    return 0;
}


int main(void) {
    simple_test();
    return 0;
}