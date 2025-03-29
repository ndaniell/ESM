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
#include "state_machine_viz.h"

typedef enum {
    TEST_EVENT_ID_RESET = 0,
    TEST_EVENT_ID_RUN,
    TEST_EVENT_ID_ERROR,
    TEST_EVENT_ID_MAX
} test_event_id_t;

static event_t reset_event = {TEST_EVENT_ID_RESET, 0, NULL};
static event_t run_event = {TEST_EVENT_ID_RUN, 0, NULL};
static event_t error_event = {TEST_EVENT_ID_ERROR, 0, NULL};

#define FUZZ_MAX_STATES 10
#define FUZZ_NUM_ITERATIONS 100
#define FUZZ_NUM_EVENTS 1000

void state_init_on_enter_handler(event_t event) {
    printf("State init on enter\n");
}

void state_init_on_exit_handler(event_t event) {
    printf("State init on exit\n");
}

void state_run_on_enter_handler(event_t event) {
    printf("State run on enter\n");
}

void state_run_on_exit_handler(event_t event) {
    printf("State run on exit\n");
}

void state_error_on_enter_handler(event_t event) {
    printf("State error on enter\n");
}

void state_error_on_exit_handler(event_t event) {
    printf("State error on exit\n");
}

typedef enum {
    STATE_MACHINE_STATE_INIT = 0,
    STATE_MACHINE_STATE_RUN,
    STATE_MACHINE_STATE_ERROR
} state_machine_state_t;

int simple_walk_test(void) {
    state_machine_t* state_machine = state_machine_create(STATE_MACHINE_STATE_INIT);

    state_machine_assign_on_enter_handler(state_machine, STATE_MACHINE_STATE_INIT, state_init_on_enter_handler);
    state_machine_assign_on_exit_handler(state_machine, STATE_MACHINE_STATE_INIT, state_init_on_exit_handler);

    state_machine_assign_on_enter_handler(state_machine, STATE_MACHINE_STATE_RUN, state_run_on_enter_handler);
    state_machine_assign_on_exit_handler(state_machine, STATE_MACHINE_STATE_RUN, state_run_on_exit_handler);

    state_machine_assign_on_enter_handler(state_machine, STATE_MACHINE_STATE_ERROR, state_error_on_enter_handler);
    state_machine_assign_on_exit_handler(state_machine, STATE_MACHINE_STATE_ERROR, state_error_on_exit_handler);

    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_INIT, STATE_MACHINE_STATE_RUN, TEST_EVENT_ID_RUN);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_RUN, STATE_MACHINE_STATE_ERROR, TEST_EVENT_ID_ERROR);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_RUN, STATE_MACHINE_STATE_INIT, TEST_EVENT_ID_RESET);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_ERROR, STATE_MACHINE_STATE_RUN, TEST_EVENT_ID_RUN);

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

void call_dot_command(const char* dot_file, const char* png_file) {
    char command[256];
    snprintf(command, sizeof(command), "dot -Tpng %s -o %s", dot_file, png_file);
    system(command);
}

void visualization_test(void) {
    state_machine_t* state_machine = state_machine_create(STATE_MACHINE_STATE_INIT);

    // Add the same transitions as in simple_walk_test
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_INIT, STATE_MACHINE_STATE_RUN, TEST_EVENT_ID_RUN);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_RUN, STATE_MACHINE_STATE_ERROR, TEST_EVENT_ID_ERROR);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_RUN, STATE_MACHINE_STATE_INIT, TEST_EVENT_ID_RESET);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_ERROR, STATE_MACHINE_STATE_RUN, TEST_EVENT_ID_RUN);

    // Generate visualization
    state_machine_save_dot(state_machine, "state_machine.dot");
    printf("State machine visualization saved to 'state_machine.dot'\n");
    printf("To generate PNG, run: dot -Tpng state_machine.dot -o state_machine.png\n");

    state_machine_destroy(state_machine);
}

// Helper function to generate a random event
static event_t generate_random_event(int max_event_id) {
    event_t event = {
        .event_id = rand() % max_event_id,
        .event_data_length = 0,
        .event_data = NULL
    };
    return event;
}

void fuzz_test(void) {
    srand(time(NULL));  // Initialize random seed

    for (int iteration = 0; iteration < FUZZ_NUM_ITERATIONS; iteration++) {
        // Create a state machine with random number of states
        int num_states = (rand() % (FUZZ_MAX_STATES - 2)) + 2;  // At least 2 states
        state_machine_t* state_machine = state_machine_create(0);  // Start at state 0

        // Add random transitions
        int num_transitions = rand() % (num_states * MAX_EVENTS_PER_STATE);
        for (int i = 0; i < num_transitions; i++) {
            state_id_t from_state = rand() % num_states;
            state_id_t to_state = rand() % num_states;
            event_id_t event_id = rand() % MAX_EVENTS_PER_STATE;
            
            state_machine_add_transition(state_machine, from_state, to_state, event_id);
        }

        // Generate visualization of initial state
        char dot_filename[64];
        char png_filename[64];
        snprintf(dot_filename, sizeof(dot_filename), "fuzz_test_%d_initial.dot", iteration);
        snprintf(png_filename, sizeof(png_filename), "fuzz_test_%d_initial.png", iteration);
        state_machine_save_dot(state_machine, dot_filename);
        call_dot_command(dot_filename, png_filename);

        // Send random events
        printf("Fuzz test iteration %d: Testing state machine with %d states\n", 
               iteration, num_states);
        
        for (int i = 0; i < FUZZ_NUM_EVENTS; i++) {
            event_t random_event = generate_random_event(MAX_EVENTS_PER_STATE);
            state_id_t previous_state = state_machine->current_state;
            
            state_machine_event(state_machine, random_event);
            
            // Verify that the current state is within bounds
            assert(state_machine->current_state < num_states);
            
            if (previous_state != state_machine->current_state) {
                printf("  Event %d triggered transition: %d -> %d\n", 
                       random_event.event_id, previous_state, state_machine->current_state);
            }
        }

        // Generate visualization of final state
        snprintf(dot_filename, sizeof(dot_filename), "fuzz_test_%d_final.dot", iteration);
        snprintf(png_filename, sizeof(png_filename), "fuzz_test_%d_final.png", iteration);
        state_machine_save_dot(state_machine, dot_filename);
        call_dot_command(dot_filename, png_filename);

        state_machine_destroy(state_machine);
        printf("Fuzz test iteration %d completed successfully\n\n", iteration);
    }
}

void print_structure_statistics(void) {
    printf("\nState Machine Structure Statistics:\n");
    printf("===================================\n\n");

    // Basic types
    printf("Basic Types:\n");
    printf("  state_id_t size:     %zu bytes\n", sizeof(state_id_t));
    printf("  event_id_t size:     %zu bytes\n", sizeof(event_id_t));
    printf("  event_handler_t size: %zu bytes\n", sizeof(state_machine_event_handler_t));
    printf("\n");

    // Event structure
    printf("Event Structure:\n");
    printf("  event_t total size:   %zu bytes\n", sizeof(event_t));
    printf("    - event_id:         %zu bytes\n", sizeof(event_id_t));
    printf("    - event_data_length:%zu bytes\n", sizeof(size_t));
    printf("    - event_data:       %zu bytes\n", sizeof(void*));
    printf("\n");

    // State table entry
    printf("State Table Entry:\n");
    printf("  state_table_entry_t total size: %zu bytes\n", sizeof(state_table_entry_t));
    printf("    - state:           %zu bytes\n", sizeof(state_id_t));
    printf("    - state_on_enter:  %zu bytes\n", sizeof(state_machine_event_handler_t));
    printf("    - state_on_exit:   %zu bytes\n", sizeof(state_machine_event_handler_t));
    printf("\n");

    // Transition structure
    printf("Transition Structure:\n");
    printf("  state_machine_transition_t total size: %zu bytes\n", sizeof(state_machine_transition_t));
    printf("    - init:            %zu bytes\n", sizeof(int));
    printf("    - current_state:   %zu bytes\n", sizeof(state_id_t));
    printf("    - next_state:      %zu bytes\n", sizeof(state_id_t));
    printf("    - event_id:        %zu bytes\n", sizeof(event_id_t));
    printf("    - on_transition:   %zu bytes\n", sizeof(state_machine_event_handler_t));
    printf("\n");

    // Main state machine structure
    printf("State Machine Structure:\n");
    printf("  state_machine_t total size: %zu bytes\n", sizeof(state_machine_t));
    printf("    - state_table:     %zu bytes\n", sizeof(state_table_entry_t[STATE_MACHINE_STATE_MAX]));
    printf("    - initial_state:   %zu bytes\n", sizeof(state_id_t));
    printf("    - current_state:   %zu bytes\n", sizeof(state_id_t));
    printf("    - state_transitions: %zu bytes\n", 
           sizeof(state_machine_transition_t[STATE_MACHINE_STATE_MAX][MAX_EVENTS_PER_STATE]));
    printf("\n");

    // Configuration constants
    printf("Configuration Constants:\n");
    printf("  MAX_EVENTS_PER_STATE:      %d\n", MAX_EVENTS_PER_STATE);
    printf("  STATE_MACHINE_STATE_MAX:   %d\n", STATE_MACHINE_STATE_MAX);
    printf("\n");

    // Total memory usage example
    printf("Memory Usage Example:\n");
    printf("  Single state machine instance: %zu bytes\n", sizeof(state_machine_t));
    printf("  Theoretical minimum alignment waste: %zu bytes\n", 
           sizeof(state_machine_t) - (
               sizeof(state_table_entry_t[STATE_MACHINE_STATE_MAX]) +
               sizeof(state_id_t) * 2 +
               sizeof(state_machine_transition_t[STATE_MACHINE_STATE_MAX][MAX_EVENTS_PER_STATE])
           ));
    printf("\n");
}

int main(void) {
    print_structure_statistics();
    simple_walk_test();
    visualization_test();
    //fuzz_test();
    return 0;
}