#include "state_machine.h"
#include <assert.h>
#include <stdlib.h>

state_machine_t *state_machine_create(void) {
  state_machine_t *state_machine =
      (state_machine_t *)malloc(sizeof(state_machine_t));
  assert(state_machine != NULL);
  return state_machine;
}

void state_machine_destroy(state_machine_t *state_machine) {
  assert(state_machine != NULL);
  free(state_machine);
}

void state_machine_init(state_machine_t *state_machine) {
  assert(state_machine != NULL);
  state_machine->current_state = STATE_MACHINE_STATE_INIT;
}

void state_machine_event(state_machine_t *state_machine, event_t event) {
  assert(state_machine != NULL);

  // Ignore event if the state guard is false
  if (state_machine->state_table[state_machine->current_state].state_guard) {
    if (!state_machine->state_table[state_machine->current_state].state_guard()) {
      return;
    }
  }

  // Call the on exit function if it exists
  if (state_machine->state_table[state_machine->current_state].state_on_exit) {
    state_machine->state_table[state_machine->current_state].state_on_exit();
  }

  // Transition to new state
  state_machine->current_state =
      state_machine->state_table[state_machine->current_state]
          .state_event_handler(event);

  // Call the on enter function if it exists
  if (state_machine->state_table[state_machine->current_state].state_on_enter) {
    state_machine->state_table[state_machine->current_state].state_on_enter();
  }
}