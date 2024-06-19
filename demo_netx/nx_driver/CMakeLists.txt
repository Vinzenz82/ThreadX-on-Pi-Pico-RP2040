project(nx_driver_pico_w)

add_library(${PROJECT_NAME} nx_driver_pico_w.c)

target_link_libraries(${PROJECT_NAME} pico_cyw43_arch_poll 
                                      pico_stdlib
                                      cmsis_core 
                                      azrtos::netxduo
)

target_compile_definitions(pico_cyw43_arch_poll INTERFACE -DCYW43_LWIP=0)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
                              WIFI_SSID=\"${WIFI_SSID}\"
                              WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
)

target_include_directories(${PROJECT_NAME} PUBLIC .)

