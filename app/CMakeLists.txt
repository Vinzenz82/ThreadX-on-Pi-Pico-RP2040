project(app)

set(SOURCE_FILES_APP 
        ../main.c
        ../../port/tx_initialize_low_level.S
        app_main.c
        ../middleware/nanoMODBUS/nanomodbus.c
)

add_executable(${PROJECT_NAME} 
        ${SOURCE_FILES_APP}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ../middleware/nanoMODBUS/
)

pico_enable_stdio_usb(${PROJECT_NAME} ${ENABLE_USB})

pico_add_extra_outputs(${PROJECT_NAME})

if (PICO_CYW43_SUPPORTED) # set by PICO_BOARD=pico_w
    target_link_libraries(${PROJECT_NAME} pico_cyw43_arch_none )
endif()
target_link_libraries(${PROJECT_NAME} pico_stdlib cmsis_core azrtos::threadx)

