/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file ui_app.h
 * @brief App-side UI initialization — wires models to the native UI library.
 */

#ifndef APP_UI_APP_H
#define APP_UI_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the UI library and register app-side provider adapters.
 *        Must be called after display_init_screens().
 */
void ui_app_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_UI_APP_H */
