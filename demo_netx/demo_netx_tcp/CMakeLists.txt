project(demo_netx_tcp)

set(SOURCE_FILES_APP 
        ../../main.c
        ../../port/tx_initialize_low_level.S
        demo_netx_tcp.c
        ../../middleware/nanoMODBUS/nanomodbus.c
)

add_executable(${PROJECT_NAME} 
        ${SOURCE_FILES_APP}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ../../middleware/nanoMODBUS/
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        NX_ENABLE_DHCP
        )

pico_enable_stdio_usb(${PROJECT_NAME} ${ENABLE_USB})
pico_add_extra_outputs(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} nx_driver_pico_w)
