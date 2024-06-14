cmake_minimum_required(VERSION 3.12)

set(ENV{PICO_CYW43_DRIVER_PATH} ${CMAKE_CURRENT_LIST_DIR}/lib/cyw43-driver)

include(pico_sdk_import.cmake)

project(Azure-RTOS-on-Raspberry-Pi-Pico-RP2040)

pico_sdk_init()

if (NOT DEFINED ENABLE_USB)
    set(ENABLE_USB 0)
endif()
message(STATUS "ENABLE_USB: ${ENABLE_USB}")

set(NXD_ENABLE_FILE_SERVERS OFF CACHE INTERNAL "Includes a dependency on FileX to support 'server' protocol handlers")
set(NXD_ENABLE_AZURE_IOT OFF CACHE INTERNAL "Enable Azure IoT")

set(THREADX_ARCH "cortex_m0")
set(THREADX_TOOLCHAIN "gnu")

set(TX_USER_FILE "${CMAKE_CURRENT_LIST_DIR}/config/tx_user_config.h")
add_subdirectory(lib/threadx)

set(NX_USER_FILE "${CMAKE_CURRENT_LIST_DIR}/config/nx_user_config.h")
add_subdirectory(lib/netxduo)
add_subdirectory(demo_threadx)
add_subdirectory(demo_netx)
add_subdirectory(app)
