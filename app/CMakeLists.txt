project(app)

set(SOURCE_FILES_APP 
        ../main.c
        ../port/tx_initialize_low_level.S
        app_main.c
        hw_config.c
        ../middleware/nanoMODBUS/nanomodbus.c
)

add_subdirectory(../lib/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src build)

add_executable(${PROJECT_NAME} 
        ${SOURCE_FILES_APP}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ../middleware/nanoMODBUS/
)

pico_enable_stdio_usb(${PROJECT_NAME} ${ENABLE_USB})

######test
pico_enable_stdio_uart(${PROJECT_NAME} 1) 
target_compile_options(${PROJECT_NAME} PUBLIC 
  -Wstack-usage=2048 
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
  PARAM_ASSERTIONS_ENABLE_ALL=1 
  PICO_USE_STACK_GUARDS=1
  PICO_MALLOC_PANIC=1
)
######test end

pico_add_extra_outputs(${PROJECT_NAME})


if (PICO_CYW43_SUPPORTED) # set by PICO_BOARD=pico_w
    target_link_libraries(${PROJECT_NAME} pico_cyw43_arch_none )
endif()
target_link_libraries(${PROJECT_NAME} pico_stdlib cmsis_core azrtos::threadx FatFs_SPI)

