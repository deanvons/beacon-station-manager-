cmake_minimum_required(VERSION 3.21)
project(beacon_station_manager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Test)
qt_standard_project_setup()   # turns on AUTOMOC (needed for Q_OBJECT, signals and slots)

# ---------------------------------------------------------------------------
# beacon_core: everything that is NOT user interface — data, validation,
# the registry, the simulator.
# Add your new non-UI .h/.cpp files here as you create them.
# ---------------------------------------------------------------------------
add_library(beacon_core STATIC
    src/beacon.h src/beacon.cpp
)
target_include_directories(beacon_core PUBLIC src)
target_link_libraries(beacon_core PUBLIC Qt6::Core)

# ---------------------------------------------------------------------------
# beacon_manager: the application. Add your widget classes here.
# ---------------------------------------------------------------------------
qt_add_executable(beacon_manager
    src/main.cpp
    src/mainwindow.h src/mainwindow.cpp
)
target_link_libraries(beacon_manager PRIVATE beacon_core Qt6::Widgets)
set_target_properties(beacon_manager PROPERTIES WIN32_EXECUTABLE ON)

# ---------------------------------------------------------------------------
# Tests are OPTIONAL in this assignment — see "Adding tests later" in
# assignment.md. Everything needed is already here: uncomment this block, put a
# Qt Test .cpp in tests/, and add one add_beacon_test(...) line per file.
# ---------------------------------------------------------------------------
# enable_testing()
#
# function(add_beacon_test name file)
#     qt_add_executable(${name} ${file})
#     set_target_properties(${name} PROPERTIES WIN32_EXECUTABLE OFF)   # console program
#     target_link_libraries(${name} PRIVATE beacon_core Qt6::Test)
#     add_test(NAME ${name} COMMAND ${name})
#     set_tests_properties(${name} PROPERTIES ENVIRONMENT_MODIFICATION
#         # always print results to the console (on Windows Qt may otherwise hide them)
#         "QT_FORCE_STDERR_LOGGING=set:1;PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>")
# endfunction()
#
# add_beacon_test(tst_beacon tests/tst_beacon.cpp)
