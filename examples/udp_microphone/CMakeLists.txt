cmake_minimum_required(VERSION 3.12)

add_executable(udp_microphone main.c)

target_include_directories(udp_microphone PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(udp_microphone PRIVATE pico_analog_microphone pico_cyw43_arch_lwip_threadsafe_background)

target_compile_definitions(udp_microphone PRIVATE
    WIFI_SSID=\"${WIFI_SSID}\"
    WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
    SERVER=\"${SERVER}\"
    PORT=${PORT}
)

pico_add_extra_outputs(udp_microphone)
