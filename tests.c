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

#define PERF_NUM_ITERATIONS 1000000
#define PERF_NUM_TRANSITIONS 5
#define PERF_WARMUP_ITERATIONS 1000

#define DEBUG_STATE_MACHINE 0

typedef struct {
    double avg_event_processing_us;
    double min_event_processing_us;
    double max_event_processing_us;
    uint64_t total_state_changes;
    uint64_t total_events_processed;
} perf_stats_t;

void state_init_on_enter_handler(event_t event) {
    #if DEBUG_STATE_MACHINE
        printf("State init on enter\n");
    #endif
}

void state_init_on_exit_handler(event_t event) {
    #if DEBUG_STATE_MACHINE
        printf("State init on exit\n");
    #endif
}

void state_run_on_enter_handler(event_t event) {
    #if DEBUG_STATE_MACHINE
        printf("State run on enter\n");
    #endif
}

void state_run_on_exit_handler(event_t event) {
    #if DEBUG_STATE_MACHINE
        printf("State run on exit\n");
    #endif
}

void state_error_on_enter_handler(event_t event) {
    #if DEBUG_STATE_MACHINE
        printf("State error on enter\n");
    #endif
}

void state_error_on_exit_handler(event_t event) {
    #if DEBUG_STATE_MACHINE
        printf("State error on exit\n");
    #endif
}

typedef enum {
    STATE_MACHINE_STATE_INIT = 0,
    STATE_MACHINE_STATE_RUN,
    STATE_MACHINE_STATE_ERROR
} state_machine_state_t;

// Add these test-specific definitions after the existing test_event_id_t enum
static event_t data_event = {TEST_EVENT_ID_RUN, sizeof(int), NULL};
static int test_data = 42;

// Add these guard condition functions after the existing state handler functions
int guard_check_data_exists(event_t event) {
    return event.event_data != NULL && event.event_data_length > 0;
}

int guard_check_data_value(event_t event) {
    if (!event.event_data || event.event_data_length != sizeof(int)) {
        return 0;
    }
    return *(int*)event.event_data == 42;
}

void on_transition_handler(event_t event) {
    #if DEBUG_STATE_MACHINE
        printf("State transition\n");
    #endif
}

int simple_walk_test(void) {
    state_machine_t* state_machine = state_machine_create(STATE_MACHINE_STATE_INIT);

    state_machine_assign_on_enter_handler(state_machine, STATE_MACHINE_STATE_INIT, state_init_on_enter_handler);
    state_machine_assign_on_exit_handler(state_machine, STATE_MACHINE_STATE_INIT, state_init_on_exit_handler);

    state_machine_assign_on_enter_handler(state_machine, STATE_MACHINE_STATE_RUN, state_run_on_enter_handler);
    state_machine_assign_on_exit_handler(state_machine, STATE_MACHINE_STATE_RUN, state_run_on_exit_handler);

    state_machine_assign_on_enter_handler(state_machine, STATE_MACHINE_STATE_ERROR, state_error_on_enter_handler);
    state_machine_assign_on_exit_handler(state_machine, STATE_MACHINE_STATE_ERROR, state_error_on_exit_handler);

    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_INIT, STATE_MACHINE_STATE_RUN, TEST_EVENT_ID_RUN, NULL);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_RUN, STATE_MACHINE_STATE_ERROR, TEST_EVENT_ID_ERROR, NULL);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_RUN, STATE_MACHINE_STATE_INIT, TEST_EVENT_ID_RESET, NULL);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_ERROR, STATE_MACHINE_STATE_RUN, TEST_EVENT_ID_RUN, NULL);

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

void cleanup_test_files(const char* pattern) {
    char command[256];
#ifdef _WIN32
    snprintf(command, sizeof(command), "del %s 2>nul", pattern);
#else
    snprintf(command, sizeof(command), "rm -f %s 2>/dev/null", pattern);
#endif
    system(command);
}

void visualization_test(void) {
    printf("\nVisualization Test:\n");
    printf("==================\n\n");

    state_machine_t* state_machine = state_machine_create(STATE_MACHINE_STATE_INIT);

    // Add transitions with different configurations to test visualization
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_INIT, STATE_MACHINE_STATE_RUN, TEST_EVENT_ID_RUN, NULL);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_RUN, STATE_MACHINE_STATE_ERROR, TEST_EVENT_ID_ERROR, NULL);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_RUN, STATE_MACHINE_STATE_INIT, TEST_EVENT_ID_RESET, NULL);
    state_machine_add_transition(state_machine, STATE_MACHINE_STATE_ERROR, STATE_MACHINE_STATE_RUN, TEST_EVENT_ID_RUN, NULL);

    // Generate visualization
    state_machine_save_dot(state_machine, "state_machine.dot");
    printf("[PASS] Generated state machine visualization\n");

    state_machine_destroy(state_machine);

    // Cleanup generated files
    printf("Cleaning up test files...\n");
    cleanup_test_files("state_machine_*.dot");
    cleanup_test_files("state_machine_*.png");
    printf("[DONE] Cleaned up visualization files\n");
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
            
            state_machine_add_transition(state_machine, from_state, to_state, event_id, NULL);
        }

        #if DEBUG_STATE_MACHINE
        // Generate visualization of initial state
            char dot_filename[64];
            char png_filename[64];
            snprintf(dot_filename, sizeof(dot_filename), "fuzz_test_%d_initial.dot", iteration);
            snprintf(png_filename, sizeof(png_filename), "fuzz_test_%d_initial.png", iteration);
            state_machine_save_dot(state_machine, dot_filename);
            call_dot_command(dot_filename, png_filename);
        #endif

        // Send random events
        #if DEBUG_STATE_MACHINE
            printf("Fuzz test iteration %d: Testing state machine with %d states\n", 
                   iteration, num_states);
        #endif
        
        for (int i = 0; i < FUZZ_NUM_EVENTS; i++) {
            event_t random_event = generate_random_event(MAX_EVENTS_PER_STATE);
            state_id_t previous_state = state_machine->current_state;
            
            state_machine_event(state_machine, random_event);
            
            // Verify that the current state is within bounds
            assert(state_machine->current_state < num_states);
            
            #if DEBUG_STATE_MACHINE
                if (previous_state != state_machine->current_state) {
                    printf("  Event %d triggered transition: %d -> %d\n", 
                           random_event.event_id, previous_state, state_machine->current_state);
                }
            #endif
        }

        // Generate visualization of final state
        #if DEBUG_STATE_MACHINE
            snprintf(dot_filename, sizeof(dot_filename), "fuzz_test_%d_final.dot", iteration);
            snprintf(png_filename, sizeof(png_filename), "fuzz_test_%d_final.png", iteration);
            state_machine_save_dot(state_machine, dot_filename);
            call_dot_command(dot_filename, png_filename);
        #endif

        state_machine_destroy(state_machine);
        #if DEBUG_STATE_MACHINE
            printf("Fuzz test iteration %d completed successfully\n\n", iteration);
        #endif
    }

    // Add cleanup at the end of the function
    cleanup_test_files("fuzz_test_*.dot");
    cleanup_test_files("fuzz_test_*.png");
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

static double get_time_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1e6 + (double)ts.tv_nsec / 1e3;
}

static perf_stats_t run_performance_test(state_machine_t* state_machine, event_t* events, int num_events) {
    perf_stats_t stats = {0};
    double min_time = 1e9;  // Initialize to a large value
    double max_time = 0;
    double total_time = 0;
    
    // Warm up to avoid cold cache effects
    for (int i = 0; i < PERF_WARMUP_ITERATIONS; i++) {
        state_machine_event(state_machine, events[i % num_events]);
    }
    
    // Main performance test
    for (int i = 0; i < PERF_NUM_ITERATIONS; i++) {
        state_id_t prev_state = state_machine->current_state;
        event_t current_event = events[i % num_events];
        
        double start_time = get_time_us();
        state_machine_event(state_machine, current_event);
        double end_time = get_time_us();
        
        double elapsed = end_time - start_time;
        total_time += elapsed;
        min_time = (elapsed < min_time) ? elapsed : min_time;
        max_time = (elapsed > max_time) ? elapsed : max_time;
        
        stats.total_events_processed++;
        if (prev_state != state_machine->current_state) {
            stats.total_state_changes++;
        }
    }
    
    stats.avg_event_processing_us = total_time / PERF_NUM_ITERATIONS;
    stats.min_event_processing_us = min_time;
    stats.max_event_processing_us = max_time;
    
    return stats;
}

void performance_test(void) {
    printf("\nState Machine Performance Test:\n");
    printf("==============================\n\n");

    // Create a simple circular state machine for testing
    state_machine_t* state_machine = state_machine_create(0);
    
    // Create a circular pattern of states
    for (int i = 0; i < PERF_NUM_TRANSITIONS; i++) {
        state_machine_add_transition(state_machine, i, (i + 1) % PERF_NUM_TRANSITIONS, i, NULL);
    }
    
    // Create test events
    event_t test_events[PERF_NUM_TRANSITIONS];
    for (int i = 0; i < PERF_NUM_TRANSITIONS; i++) {
        test_events[i].event_id = i;
        test_events[i].event_data_length = 0;
        test_events[i].event_data = NULL;
    }
    
    // Run performance test
    printf("Running performance test with:\n");
    printf("  - %d iterations\n", PERF_NUM_ITERATIONS);
    printf("  - %d states in circular transition\n", PERF_NUM_TRANSITIONS);
    printf("  - %d warmup iterations\n\n", PERF_WARMUP_ITERATIONS);
    
    perf_stats_t stats = run_performance_test(state_machine, test_events, PERF_NUM_TRANSITIONS);
    
    // Print results
    printf("Results:\n");
    printf("  Average event processing time: %.3f us\n", stats.avg_event_processing_us);
    printf("  Minimum event processing time: %.3f us\n", stats.min_event_processing_us);
    printf("  Maximum event processing time: %.3f us\n", stats.max_event_processing_us);
    printf("  Events per second: %.2f\n", 1e6 / stats.avg_event_processing_us);
    printf("  Total state changes: %lu\n", stats.total_state_changes);
    printf("  State change ratio: %.2f%%\n", 
           (double)stats.total_state_changes / stats.total_events_processed * 100);
    
    // Run additional test with handlers to measure overhead
    printf("\nRunning test with handlers:\n");
    for (int i = 0; i < PERF_NUM_TRANSITIONS; i++) {
        state_machine_assign_on_enter_handler(state_machine, i, state_init_on_enter_handler);
        state_machine_assign_on_exit_handler(state_machine, i, state_init_on_exit_handler);
    }
    
    perf_stats_t stats_with_handlers = run_performance_test(state_machine, test_events, PERF_NUM_TRANSITIONS);
    
    printf("  Average event processing time: %.3f us\n", stats_with_handlers.avg_event_processing_us);
    printf("  Handler overhead: %.3f us\n", 
           stats_with_handlers.avg_event_processing_us - stats.avg_event_processing_us);
    
    state_machine_destroy(state_machine);
}

// Add this test function before main()
int guard_condition_test(void) {
    printf("\nGuard Condition Test:\n");
    printf("===================\n\n");

    state_machine_t* state_machine = state_machine_create(STATE_MACHINE_STATE_INIT);

    // Add transitions with guards
    state_machine_add_transition_with_guard(
        state_machine,
        STATE_MACHINE_STATE_INIT,
        STATE_MACHINE_STATE_RUN,
        TEST_EVENT_ID_RUN,
        on_transition_handler,
        guard_check_data_exists
    );

    state_machine_add_transition_with_guard(
        state_machine,
        STATE_MACHINE_STATE_RUN,
        STATE_MACHINE_STATE_ERROR,
        TEST_EVENT_ID_RUN,
        NULL,
        guard_check_data_value
    );

    // Test 1: Transition should fail (no data)
    printf("Test 1: Attempting transition with no data\n");
    state_machine_event(state_machine, run_event);
    assert(state_machine->current_state == STATE_MACHINE_STATE_INIT);
    printf("Guard prevented transition without data\n");

    // Test 2: Transition should succeed (with data)
    printf("Test 2: Attempting transition with valid data\n");
    data_event.event_data = &test_data;
    state_machine_event(state_machine, data_event);
    assert(state_machine->current_state == STATE_MACHINE_STATE_RUN);
    printf("Guard allowed transition with data\n");

    // Test 3: Test value-specific guard
    printf("Test 3: Testing value-specific guard\n");
    test_data = 41;  // Wrong value
    state_machine_event(state_machine, data_event);
    assert(state_machine->current_state == STATE_MACHINE_STATE_RUN);
    printf("Guard prevented transition with incorrect value\n");

    test_data = 42;  // Correct value
    state_machine_event(state_machine, data_event);
    assert(state_machine->current_state == STATE_MACHINE_STATE_ERROR);
    printf("Guard allowed transition with correct value\n");

    state_machine_destroy(state_machine);
    printf("\nGuard condition test completed successfully\n\n");
    return 0;
}

// Modify main() to include the new test
int main(void) {
    print_structure_statistics();
    simple_walk_test();
    visualization_test();
    performance_test();
    guard_condition_test();
    fuzz_test();
    return 0;
}