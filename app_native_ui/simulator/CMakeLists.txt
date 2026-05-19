cmake_minimum_required(VERSION 3.20)
project(app_native_ui_sim C)

set(CMAKE_C_STANDARD 11)

# Path to LVGL source — override with -DLVGL_DIR=<path> if needed.
if(NOT DEFINED LVGL_DIR)
    set(LVGL_DIR "$ENV{ZEPHYR_BASE}/../modules/lib/gui/lvgl"
        CACHE PATH "Path to LVGL source tree")
endif()

# lv_conf.h in this directory.
set(LV_CONF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h"
    CACHE STRING "Path to lv_conf.h")

add_compile_definitions(
    LV_CONF_INCLUDE_SIMPLE
    LV_CONF_PATH="${LV_CONF_PATH}"
)

add_subdirectory(${LVGL_DIR} ${CMAKE_BINARY_DIR}/lvgl EXCLUDE_FROM_ALL)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

set(UI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(sim
    main.c
    ${UI_ROOT}/ui.c
    ${UI_ROOT}/data/nav_data.c
    ${UI_ROOT}/data/protimer_data.c
    ${UI_ROOT}/data/clockalarm_data.c
    ${UI_ROOT}/data/ui_data.c
    ${UI_ROOT}/screens/screen_menu.c
    ${UI_ROOT}/screens/screen_protimer.c
    ${UI_ROOT}/screens/screen_clockalarm.c
)

target_include_directories(sim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}      # lv_conf.h
    ${UI_ROOT}                       # data/*, screens/*
    ${UI_ROOT}/include               # ui.h, *_provider.h
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(sim PRIVATE lvgl ${SDL2_LIBRARIES})
