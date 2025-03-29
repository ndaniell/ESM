#include "state_machine.h"
#include <assert.h>
#include <stdlib.h>

state_machine_t *state_machine_create(state_id_t initial_state) {
  // Zeroing is required, assumed zeroed initial state
  state_machine_t *state_machine = (state_machine_t *)calloc(1, sizeof(state_machine_t));
  assert(state_machine != NULL);
  state_machine->initial_state = initial_state;
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
    current_state->state_on_exit(event);
  }

  // Check if there is a transition for the event
  state_machine_transition_t *state_transitions = state_machine->state_transitions[state_machine->current_state];
    for (int i = 0; i < MAX_EVENTS_PER_STATE; i++) {
    if (state_transitions[i].init && state_transitions[i].event_id == event.event_id) {
      if (state_transitions[i].on_transition) {
        state_transitions[i].on_transition(event);
      }
      state_machine->current_state = state_transitions[i].next_state;
      break;
    }
  }
  

  // Call the on enter function if it exists
  if (current_state->state_on_enter) {
    current_state->state_on_enter(event);
  }
}

void state_machine_add_transition(state_machine_t *state_machine, state_id_t state_a, state_id_t state_b, event_id_t event_id) {
  assert(state_machine != NULL);
  assert(state_a < STATE_MACHINE_STATE_MAX);
  assert(state_b < STATE_MACHINE_STATE_MAX);
  assert(event_id < MAX_EVENTS_PER_STATE);
  state_machine->state_transitions[state_a][event_id].init = 1;
  state_machine->state_transitions[state_a][event_id].current_state = state_a;
  state_machine->state_transitions[state_a][event_id].next_state = state_b;
  state_machine->state_transitions[state_a][event_id].event_id = event_id;
}

void state_machine_assign_on_enter_handler(state_machine_t *state_machine, state_id_t state, state_machine_event_handler_t on_enter) {
  assert(state_machine != NULL);
  assert(state < STATE_MACHINE_STATE_MAX);
  state_machine->state_table[state].state_on_enter = on_enter;
}

void state_machine_assign_on_exit_handler(state_machine_t *state_machine, state_id_t state, state_machine_event_handler_t on_exit) {
  assert(state_machine != NULL);
  assert(state < STATE_MACHINE_STATE_MAX);
  state_machine->state_table[state].state_on_exit = on_exit;
}
