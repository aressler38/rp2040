
set(CMAKE_VERBOSE_MAKEFILE ON)

target_link_libraries(comm_test
        pico_stdlib              # for core functionality
        pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything else
        hardware_rtc

)

add_library(morse_code
        morse_code.c
)