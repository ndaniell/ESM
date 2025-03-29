/**
 * Copyright (c) 2025 Nicholas Daniell
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
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