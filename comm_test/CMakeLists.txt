cmake_minimum_required(VERSION 3.25)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(comm_test C CXX ASM)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 26)


if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif ()

set(PICO_NO_HARDWARE 0)

pico_sdk_init()
message(STATUS "Pico SDK version: ${PICO_SDK_VERSION_STRING}")


add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
)
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif ()


# Add hello world example
add_subdirectory(comm_test)
