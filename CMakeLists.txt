cmake_minimum_required(VERSION 3.16)
project(common C)

option(BUILD_COMMON_LIBRARY "Build a static library from common sources" ON)
option(BUILD_COMMON_TESTS   "Build and run tests"                        OFF)

# ---------------------------------------------------------------------------
# Recursively collect all directories containing .h files under a given root
# and add them as INTERFACE include directories on a target.
# Usage: recursively_add_header_dirs(<target> <dir1> [dir2 ...])
# ---------------------------------------------------------------------------
function(recursively_add_header_dirs target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "recursively_add_header_dirs: target '${target}' does not exist")
    endif()

    set(HEADER_DIRS "")
    foreach(HEADER_ROOT IN LISTS ARGN)
        if(NOT IS_ABSOLUTE "${HEADER_ROOT}")
            set(HEADER_ROOT "${CMAKE_CURRENT_LIST_DIR}/${HEADER_ROOT}")
        endif()

        if(EXISTS "${HEADER_ROOT}")
            file(GLOB_RECURSE _HEADERS "${HEADER_ROOT}/*.h")
            foreach(_HEADER IN LISTS _HEADERS)
                get_filename_component(_DIR "${_HEADER}" DIRECTORY)
                list(APPEND HEADER_DIRS "${_DIR}")
            endforeach()
        else()
            message(WARNING "recursively_add_header_dirs: path '${HEADER_ROOT}' does not exist")
        endif()
    endforeach()

    if(HEADER_DIRS)
        list(REMOVE_DUPLICATES HEADER_DIRS)
        target_include_directories(${target} INTERFACE ${HEADER_DIRS})
    endif()
endfunction()

# ---------------------------------------------------------------------------
# Collect project header dirs
# NOTE: if you add new .h files, re-run CMake to pick them up (GLOB caveat)
# ---------------------------------------------------------------------------
file(GLOB_RECURSE _ALL_HEADERS "${CMAKE_CURRENT_LIST_DIR}/*.h")
set(COMMON_INCLUDE_DIRS "")
foreach(_H IN LISTS _ALL_HEADERS)
    get_filename_component(_D "${_H}" DIRECTORY)
    list(APPEND COMMON_INCLUDE_DIRS "${_D}")
endforeach()

# Optionally include Ceedling vendor headers if present
foreach(_vendor_dir
    "${CMAKE_CURRENT_LIST_DIR}/build/vendor/unity/src"
    "${CMAKE_CURRENT_LIST_DIR}/build/vendor/cmock/src"
)
    if(EXISTS "${_vendor_dir}")
        list(APPEND COMMON_INCLUDE_DIRS "${_vendor_dir}")
    endif()
endforeach()

list(REMOVE_DUPLICATES COMMON_INCLUDE_DIRS)

# ---------------------------------------------------------------------------
# INTERFACE library — always available, carries all include dirs
# ---------------------------------------------------------------------------
add_library(common INTERFACE)
target_include_directories(common INTERFACE ${COMMON_INCLUDE_DIRS})

# ---------------------------------------------------------------------------
# Static library
# NOTE: if you add new .c files, re-run CMake to pick them up (GLOB caveat)
# ---------------------------------------------------------------------------
if(BUILD_COMMON_LIBRARY)
    file(GLOB_RECURSE COMMON_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.c")
    list(FILTER COMMON_SOURCES EXCLUDE REGEX ".*/test/.*")

    add_library(common_static STATIC ${COMMON_SOURCES})
    target_include_directories(common_static PUBLIC ${COMMON_INCLUDE_DIRS})
    target_link_libraries(common_static PUBLIC common)
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(BUILD_COMMON_TESTS)
    enable_testing()
endif()