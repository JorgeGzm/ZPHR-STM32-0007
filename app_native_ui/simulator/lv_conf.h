/*
 * Copyright (c) 2026 GZM Embarcados
 *
 * LVGL configuration for the SDL2 simulator build.
 * Set LV_CONF_INCLUDE_SIMPLE=1 in CMake so LVGL finds this file.
 */

#if 1 /* Set to 0 to disable this file. */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* Color settings — match the target hardware (16-bit, byte-swapped). */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1

/* Memory */
#define LV_MEM_SIZE (256U * 1024U)

/* HAL tick */
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE <SDL2/SDL.h>
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (SDL_GetTicks())

/* Logging */
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 1

/* Assert */
#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1

/* Observer (required for subject/bind API) */
#define LV_USE_OBSERVER 1

/* Widgets */
#define LV_USE_LABEL 1
#define LV_USE_BUTTON 1
#define LV_USE_BAR 1

/* Fonts */
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_DEFAULT &lv_font_montserrat_24

/* SDL2 display driver */
#define LV_USE_SDL 1
#define LV_SDL_INCLUDE_PATH <SDL2/SDL.h>

/* Display resolution (matches target: 320x240) */
#define LV_HOR_RES_MAX 320
#define LV_VER_RES_MAX 240

#endif /* LV_CONF_H */
#endif /* Enable/Disable */
