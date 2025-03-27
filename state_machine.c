#include <stdlib.h>
#include <assert.h>
#include "state_machine.h"

state_machine_t* state_machine_create(void) {
    state_machine_t* state_machine = (state_machine_t*)malloc(sizeof(state_machine_t));
    assert(state_machine != NULL);
    return state_machine;
}

void state_machine_destroy(state_machine_t* state_machine) {
    assert(state_machine != NULL);
    free(state_machine);
}

void state_machine_init(state_machine_t* state_machine) {
    assert(state_machine != NULL);
    state_machine->current_state = STATE_MACHINE_STATE_INIT;
}

void state_machine_event(state_machine_t* state_machine, event_id_t event, void* data) {
    assert(state_machine != NULL);
    state_machine->current_state = state_machine->state_table[state_machine->current_state].state_event_handler(event, data);
}