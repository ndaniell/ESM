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


typedef enum {
    TEST_EVENT_ID_RESET = 0,
    TEST_EVENT_ID_RUN,
    TEST_EVENT_ID_ERROR,
    TEST_EVENT_ID_MAX
} test_event_id_t;

static event_t reset_event = {TEST_EVENT_ID_RESET, 0, NULL};
static event_t run_event = {TEST_EVENT_ID_RUN, 0, NULL};
static event_t error_event = {TEST_EVENT_ID_ERROR, 0, NULL};


state_id_t state_init_event_handler(event_t event) {
    printf("State init\n");
    switch (event.event_id) {
        case TEST_EVENT_ID_RUN:
            return STATE_MACHINE_STATE_RUN;
        case TEST_EVENT_ID_ERROR:
            return STATE_MACHINE_STATE_ERROR;
        default:
            return STATE_MACHINE_STATE_INIT;
    }
}

state_id_t state_run_event_handler(event_t event) {
    printf("State run\n");
    switch (event.event_id) {
        case TEST_EVENT_ID_RUN:
            return STATE_MACHINE_STATE_RUN;
        case TEST_EVENT_ID_ERROR:
            return STATE_MACHINE_STATE_ERROR;
        default:
            return STATE_MACHINE_STATE_INIT;
    }
}

state_id_t state_error_event_handler(event_t event) {
    printf("State error\n");
    switch (event.event_id) {
        case TEST_EVENT_ID_RUN:
            return STATE_MACHINE_STATE_RUN;
        case TEST_EVENT_ID_ERROR:
            return STATE_MACHINE_STATE_ERROR;
        default:
            return STATE_MACHINE_STATE_INIT;
    }
}

void state_init_on_enter_handler() {
    printf("State init on enter\n");
}

void state_init_on_exit_handler() {
    printf("State init on exit\n");
}

void state_run_on_enter_handler(void) {
    printf("State run on enter\n");
}

void state_run_on_exit_handler(void) {
    printf("State run on exit\n");
}

void state_error_on_enter_handler() {
    printf("State error on enter\n");
}

void state_error_on_exit_handler() {
    printf("State error on exit\n");
}

int simple_walk_test(void) {
    state_machine_t* state_machine = state_machine_create(STATE_MACHINE_STATE_INIT);

    state_machine_assign_event_handler(state_machine, STATE_MACHINE_STATE_INIT, state_init_event_handler);
    state_machine_assign_on_enter_handler(state_machine, STATE_MACHINE_STATE_INIT, state_init_on_enter_handler);
    state_machine_assign_on_exit_handler(state_machine, STATE_MACHINE_STATE_INIT, state_init_on_exit_handler);

    state_machine_assign_event_handler(state_machine, STATE_MACHINE_STATE_RUN, state_run_event_handler);
    state_machine_assign_on_enter_handler(state_machine, STATE_MACHINE_STATE_RUN, state_run_on_enter_handler);
    state_machine_assign_on_exit_handler(state_machine, STATE_MACHINE_STATE_RUN, state_run_on_exit_handler);

    state_machine_assign_event_handler(state_machine, STATE_MACHINE_STATE_ERROR, state_error_event_handler);
    state_machine_assign_on_enter_handler(state_machine, STATE_MACHINE_STATE_ERROR, state_error_on_enter_handler);
    state_machine_assign_on_exit_handler(state_machine, STATE_MACHINE_STATE_ERROR, state_error_on_exit_handler);

    assert(state_machine->current_state == STATE_MACHINE_STATE_INIT);

    state_machine_event(state_machine, run_event);
    assert(state_machine->current_state == STATE_MACHINE_STATE_RUN);

    state_machine_event(state_machine, error_event);
    assert(state_machine->current_state == STATE_MACHINE_STATE_ERROR);

    state_machine_event(state_machine, run_event);
    assert(state_machine->current_state == STATE_MACHINE_STATE_RUN);

    state_machine_event(state_machine, reset_event);
    assert(state_machine->current_state == STATE_MACHINE_STATE_INIT);

    state_machine_destroy(state_machine);
    return 0;
}

int main(void) {
    simple_walk_test();
    return 0;
}