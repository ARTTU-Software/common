cmake_minimum_required(VERSION 3.16)

option(BUILD_COMMON_LIBRARY "Build a static library from common sources" ON)

option(BUILD_COMMON_TESTS "Build and run tests" OFF)

# Function to recursively find all directories containing .h files and collect them
function(collect_header_dirs out_var)
    set(HEADER_DIRS "")
    file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")
    foreach(HEADER ${HEADERS})
        get_filename_component(HEADER_DIR ${HEADER} DIRECTORY)
        list(APPEND HEADER_DIRS ${HEADER_DIR})
    endforeach()
    if(HEADER_DIRS)
        list(REMOVE_DUPLICATES HEADER_DIRS)
        set(${out_var} ${HEADER_DIRS} PARENT_SCOPE)
    endif()
endfunction()

# Create INTERFACE library for headers (always available)
add_library(common INTERFACE)

# Collect header directories automatically
collect_header_dirs(COMMON_INCLUDE_DIRS)
target_include_directories(common INTERFACE ${COMMON_INCLUDE_DIRS})

# Always build the static library from C sources
if(BUILD_COMMON_LIBRARY)
    # Collect all .c files recursively, excluding test/ unless building tests
    if(BUILD_COMMON_TESTS)
        file(GLOB_RECURSE COMMON_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c")
    else()
        file(GLOB_RECURSE COMMON_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c")
        # Exclude test sources
        list(FILTER COMMON_SOURCES EXCLUDE REGEX ".*/test/.*")
    endif()

    add_library(common_static STATIC ${COMMON_SOURCES})
    target_include_directories(common_static PUBLIC ${COMMON_INCLUDE_DIRS})
    # Link to common INTERFACE for consistency
    target_link_libraries(common_static PUBLIC common)
endif()

# If building tests (requires the static library)
if(BUILD_COMMON_TESTS AND BUILD_COMMON_LIBRARY)
    enable_testing()
endif()
