# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ZPHR-STM32-0007)

# Incluir configurações comuns do subsistema USB do Zephyr
include(${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)

target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screen
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fonts
)

target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adc_read.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screen/basic_widgets.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screen/basic_events.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screen/main_screen.c
)