/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file setup.h
 * @brief Application initialization entry point.
 */

#ifndef APP_SETUP_H
#define APP_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize all middleware, UI and run the application event loop.
 *
 * Does not return.
 */
void setup_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_SETUP_H */
