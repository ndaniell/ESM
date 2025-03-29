#include "state_machine_viz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DOT_SIZE 8192
#define MAX_TRANSITION_STR 128
#define MAX_STATE_NAME_LEN 32

// Helper function to generate state name
static void get_state_name(char* buffer, size_t buffer_size, state_id_t state_id) {
    snprintf(buffer, buffer_size, "State_%u", state_id);
}

char* state_machine_generate_dot(state_machine_t* state_machine) {
    char* dot = (char*)malloc(MAX_DOT_SIZE);
    if (!dot) return NULL;

    // Initialize DOT graph
    int offset = snprintf(dot, MAX_DOT_SIZE,
        "digraph state_machine {\n"
        "    node [shape=circle];\n"
        "    rankdir=LR;\n\n");

    // Add all states
    char state_name[MAX_STATE_NAME_LEN];
    for (int i = 0; i < STATE_MACHINE_STATE_MAX; i++) {
        get_state_name(state_name, sizeof(state_name), i);
        
        // Check if this state has any transitions or is the current state
        bool has_transitions = false;
        for (int event = 0; event < MAX_EVENTS_PER_STATE; event++) {
            if (state_machine->state_transitions[i][event].init) {
                has_transitions = true;
                break;
            }
        }
        
        // Only add states that are either current state or have transitions
        if (i == state_machine->current_state || has_transitions) {
            offset += snprintf(dot + offset, MAX_DOT_SIZE - offset,
                "    %s [%s%s];\n",
                state_name,
                i == state_machine->current_state ? "style=filled,fillcolor=lightblue" : "",
                state_machine->state_table[i].state_on_enter || state_machine->state_table[i].state_on_exit ? 
                    ",penwidth=2" : "");
        }
    }

    // Add transitions
    for (int state = 0; state < STATE_MACHINE_STATE_MAX; state++) {
        for (int event = 0; event < MAX_EVENTS_PER_STATE; event++) {
            state_machine_transition_t* transition = &state_machine->state_transitions[state][event];
            if (transition->init) {
                char from_state[MAX_STATE_NAME_LEN];
                char to_state[MAX_STATE_NAME_LEN];
                get_state_name(from_state, sizeof(from_state), transition->current_state);
                get_state_name(to_state, sizeof(to_state), transition->next_state);
                
                offset += snprintf(dot + offset, MAX_DOT_SIZE - offset,
                    "    %s -> %s [label=\"event_%d%s\"];\n",
                    from_state,
                    to_state,
                    transition->event_id,
                    transition->on_transition ? "*" : "");
            }
        }
    }

    // Add legend
    offset += snprintf(dot + offset, MAX_DOT_SIZE - offset,
        "\n    // Legend\n"
        "    subgraph cluster_legend {\n"
        "        label=\"Legend\";\n"
        "        node [shape=plaintext];\n"
        "        legend [label=\"Blue fill = Current State\\n"
        "Bold outline = Has Enter/Exit handlers\\n"
        "* on transition = Has transition handler\"];\n"
        "    }\n");

    // Close the graph
    snprintf(dot + offset, MAX_DOT_SIZE - offset, "}\n");
    return dot;
}

int state_machine_save_dot(state_machine_t* state_machine, const char* filename) {
    char* dot = state_machine_generate_dot(state_machine);
    if (!dot) return -1;

    FILE* f = fopen(filename, "w");
    if (!f) {
        free(dot);
        return -1;
    }

    fprintf(f, "%s", dot);
    fclose(f);
    free(dot);
    return 0;
} 