cmake_minimum_required(VERSION 3.20.0)

# Encontrar o Zephyr e configurar o projeto
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(indicador-zigbee)

# Incluir configurações comuns do subsistema USB do Zephyr
include(${ZEPHYR_BASE}/samples/subsys/usb/common/common.cmake)

# Diretórios de cabeçalhos para todo o projeto
zephyr_include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screen
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fonts
)

# Adicionar arquivos-fonte ao aplicativo
target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_cdc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/buzzer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_manager.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screen/main_screen_ex1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/screen/main_screen.c
)

set(LVGL_DIR ${ZEPHYR_BASE}/modules/lib/gui/lvgl)