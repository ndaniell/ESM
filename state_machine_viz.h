#ifndef STATE_MACHINE_VIZ_H
#define STATE_MACHINE_VIZ_H

#include "state_machine.h"

#ifdef __cplusplus
extern "C" {
#endif

// Generate DOT format visualization of the state machine
// Returns a dynamically allocated string that must be freed by the caller
char* state_machine_generate_dot(state_machine_t* state_machine);

// Save the state machine visualization to a DOT file
// Returns 0 on success, -1 on failure
int state_machine_save_dot(state_machine_t* state_machine, const char* filename);

#ifdef __cplusplus
}
#endif

#endif /* STATE_MACHINE_VIZ_H */ 