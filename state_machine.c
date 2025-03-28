#include "state_machine.h"
#include <assert.h>
#include <stdlib.h>

state_machine_t *state_machine_create(state_id_t initial_state) {
  state_machine_t *state_machine =
      (state_machine_t *)malloc(sizeof(state_machine_t));
  assert(state_machine != NULL);
  state_machine->current_state = initial_state;
  return state_machine;
}

void state_machine_destroy(state_machine_t *state_machine) {
  assert(state_machine != NULL);
  free(state_machine);
}

void state_machine_event(state_machine_t *state_machine, event_t event) {
  assert(state_machine != NULL);

  state_table_entry_t *current_state = &state_machine->state_table[state_machine->current_state];

  // Call the on exit function if it exists
  if (current_state->state_on_exit) {
    current_state->state_on_exit();
  }

  // Transition to new state
  state_machine->current_state = current_state->state_event_handler(event);

  // Call the on enter function if it exists
  if (current_state->state_on_enter) {
    current_state->state_on_enter();
  }
}

void state_machine_assign_event_handler(state_machine_t *state_machine, state_id_t state, state_machine_event_handler_t event_handler) {
  assert(state_machine != NULL);
  assert(state < STATE_MACHINE_STATE_MAX);
  state_machine->state_table[state].state_event_handler = event_handler;
}

void state_machine_assign_on_enter_handler(state_machine_t *state_machine, state_id_t state, state_machine_on_enter_t on_enter) {
  assert(state_machine != NULL);
  assert(state < STATE_MACHINE_STATE_MAX);
  state_machine->state_table[state].state_on_enter = on_enter;
}

void state_machine_assign_on_exit_handler(state_machine_t *state_machine, state_id_t state, state_machine_on_exit_t on_exit) {
  assert(state_machine != NULL);
  assert(state < STATE_MACHINE_STATE_MAX);
  state_machine->state_table[state].state_on_exit = on_exit;
}
