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
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdint.h>

#include "event_queue.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef uint32_t state_id_t;
typedef state_id_t (*state_machine_event_handler_t)(event_t);
typedef state_id_t (*state_machine_guard_t)();
typedef state_id_t (*state_machine_on_enter_t)();
typedef state_id_t (*state_machine_on_exit_t)();

typedef enum {
  STATE_MACHINE_STATE_INIT,
  STATE_MACHINE_STATE_RUN,
  STATE_MACHINE_STATE_ERROR,
  STATE_MACHINE_STATE_MAX
} state_machine_state_t;

typedef struct {
  state_id_t state;
  state_machine_event_handler_t state_event_handler;
  state_machine_guard_t state_guard;
  state_machine_on_enter_t state_on_enter;
  state_machine_on_exit_t state_on_exit;
} state_table_entry_t;

typedef struct {
  state_table_entry_t state_table[STATE_MACHINE_STATE_MAX];
  state_id_t current_state;
} state_machine_t;

state_machine_t *state_machine_create(void);
void state_machine_destroy(state_machine_t *state_machine);
void state_machine_init(state_machine_t *state_machine);
void state_machine_event(state_machine_t *state_machine, event_t event);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* STATE_MACHINE_H */
