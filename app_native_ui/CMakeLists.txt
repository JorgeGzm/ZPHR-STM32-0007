cmake_minimum_required(VERSION 3.20.0)

add_library(lib-native-ui STATIC
    ui.c
    data/nav_data.c
    data/protimer_data.c
    data/clockalarm_data.c
    data/ui_data.c
    screens/screen_menu.c
    screens/screen_protimer.c
    screens/screen_clockalarm.c
)

# Inherit Zephyr/LVGL headers and compile flags.
target_link_libraries(lib-native-ui PRIVATE zephyr_interface)

# PUBLIC: app gets these transitively via target_link_libraries(app PRIVATE lib-native-ui).
target_include_directories(lib-native-ui
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
