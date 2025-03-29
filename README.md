[![ci](https://github.com/ndaniell/ESM/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/ndaniell/ESM/actions/workflows/ci.yml)
# ESM (Embedded State Machine)

A lightweight, efficient state machine implementation designed for embedded systems. ESM provides a flexible framework for implementing finite state machines with support for guards, transition handlers, and state entry/exit actions.

## Features

- Minimal memory footprint with configurable limits
- No dynamic memory allocation after initialization
- Support for state entry/exit handlers
- Transition guards for conditional state changes
- Transition handlers for action execution
- C99 compatible
- Thread-safe state transitions
- No external dependencies

## Configuration

The state machine can be configured through the following defines in `state_machine.h`:

```c
#define MAX_EVENTS_PER_STATE 20    // Maximum number of events per state
#define STATE_MACHINE_STATE_MAX 10  // Maximum number of states
```

## API Reference

### Types

```c
typedef uint32_t state_id_t;                                  // State identifier type
typedef void (*state_machine_event_handler_t)(event_t event); // Handler function type
typedef int (*state_machine_guard_t)(event_t event);         // Guard function type
```

### Core Functions

#### Creation/Destruction
```c
state_machine_t* state_machine_create(state_id_t initial_state);
void state_machine_destroy(state_machine_t* state_machine);
```

#### Event Processing
```c
void state_machine_event(state_machine_t* state_machine, event_t event);
```

#### Transition Management
```c
// Add transition without guard
void state_machine_add_transition(
    state_machine_t* state_machine,
    state_id_t state_a,
    state_id_t state_b,
    event_id_t event_id
);

// Add transition with guard condition
void state_machine_add_transition_with_guard(
    state_machine_t* state_machine,
    state_id_t state_a,
    state_id_t state_b,
    event_id_t event_id,
    state_machine_guard_t guard
);
```

#### State Handler Management
```c
void state_machine_assign_on_enter_handler(
    state_machine_t* state_machine,
    state_id_t state,
    state_machine_event_handler_t on_enter
);

void state_machine_assign_on_exit_handler(
    state_machine_t* state_machine,
    state_id_t state,
    state_machine_event_handler_t on_exit
);
```

## Usage Example

```c
// Define states
enum {
    STATE_IDLE = 0,
    STATE_RUNNING,
    STATE_ERROR
};

// Define events
enum {
    EVENT_START = 0,
    EVENT_STOP,
    EVENT_ERROR
};

// Guard condition example
int check_can_start(event_t event) {
    return system_ready() && battery_ok();
}

// State handlers
void on_enter_running(event_t event) {
    initialize_peripherals();
}

void on_exit_running(event_t event) {
    cleanup_peripherals();
}

// Create and configure state machine
state_machine_t* sm = state_machine_create(STATE_IDLE);

// Add transitions
state_machine_add_transition_with_guard(sm, STATE_IDLE, STATE_RUNNING, EVENT_START, check_can_start);
state_machine_add_transition(sm, STATE_RUNNING, STATE_IDLE, EVENT_STOP);
state_machine_add_transition(sm, STATE_RUNNING, STATE_ERROR, EVENT_ERROR);

// Add state handlers
state_machine_assign_on_enter_handler(sm, STATE_RUNNING, on_enter_running);
state_machine_assign_on_exit_handler(sm, STATE_RUNNING, on_exit_running);
```

## Performance

The state machine is designed for embedded systems with:
- O(1) event processing time
- Deterministic memory usage
- No dynamic memory allocation during operation
- Minimal stack usage

## Limitations

- Fixed maximum number of states and events per state
- No hierarchical state support
- No direct support for parallel states
- No built-in event queue (can be integrated with external queue)

## Building and Testing

The library includes a comprehensive test suite that verifies:
- Basic state transitions
- Guard conditions
- State handlers
- Performance characteristics
- Memory usage
- Error conditions

To build and run tests:
```bash
make test
```

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please follow these steps:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## Version History

- v1.0.0 - Initial release
  - Basic state machine functionality
  - State entry/exit handlers
  - Guard conditions
  - Transition handlers
