option(WIFI_SSID "WiFi SSID" "default_ssid")
option(WIFI_PASSWORD "WiFi Password" "default_password")

add_executable(comm_test
        comm_test.c
        time_util.c
        wifi.c
        pio.c
        dht11.c
)

# Use the variables as before
target_compile_definitions(comm_test PRIVATE
        WIFI_SSID="${WIFI_SSID}"
        WIFI_PASSWORD="${WIFI_PASSWORD}"
)


pico_generate_pio_header(comm_test ${CMAKE_CURRENT_LIST_DIR}/dht11.pio)


target_link_libraries(comm_test
        pico_stdlib              # for core functionality
        pico_btstack_ble
        pico_btstack_cyw43
        pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything else
        # hardware_adc
        pico_cyw43_arch_lwip_threadsafe_background

        hardware_pio
        hardware_rtc
        hardware_dma
        #pico_lwip
)

# create map/bin/hex file etc.
pico_add_extra_outputs(comm_test)

target_include_directories(comm_test PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
)

