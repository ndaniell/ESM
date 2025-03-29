cmake_minimum_required(VERSION 3.16)

project(main)

# Enable verbose output
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(EEQ)

# Add compiler flags for gcov coverage
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

add_executable(main tests.c state_machine.c EEQ/event_queue.c state_machine_viz.c)

install(TARGETS main)

enable_testing()
add_test(NAME main COMMAND main)

# Add custom target for lcov coverage reports
add_custom_target(lcov_coverage
    # Create coverage directory
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/lcov_coverage
    # Initialize coverage data
    COMMAND lcov --zerocounters --directory ${CMAKE_BINARY_DIR}/CMakeFiles/main.dir/
    # Run the tests
    COMMAND $<TARGET_FILE:main>             # Third party code
    # Capture the coverage data
    COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR}/CMakeFiles/main.dir/ --output-file lcov_coverage/coverage.info
    COMMAND lcov --remove ${CMAKE_BINARY_DIR}/lcov_coverage/coverage.info 
        --output-file ${CMAKE_BINARY_DIR}/lcov_coverage/filtered_coverage.info
        '*EEQ*'
        '*tests.c'
    # Generate HTML report
    COMMAND genhtml ${CMAKE_BINARY_DIR}/lcov_coverage/filtered_coverage.info --output-directory ${CMAKE_BINARY_DIR}/lcov_coverage/html
    #WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating lcov coverage reports"
)
