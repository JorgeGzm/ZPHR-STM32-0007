cmake_minimum_required(VERSION 3.20.0)

# Add workspace root so Zephyr finds the custom boards/ directory.
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ZPHR-STM32-0007)

include(${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)

add_subdirectory(../app_native_ui app_native_ui_build)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screen/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fonts
)

target_sources(app PRIVATE
    src/main.c
    src/bsp/bsp.c
    src/setup/setup.c
    src/ui.c
    src/adc_read.c
    src/display_manager.c
    src/screen/src/protimer_model.c
    src/screen/src/clockalarm_model.c
)

target_link_libraries(app PRIVATE lib-native-ui)
