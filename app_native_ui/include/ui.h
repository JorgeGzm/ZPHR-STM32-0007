/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file ui.h
 * @brief Native UI public API — init and provider registration.
 */

#ifndef APP_NATIVE_UI_UI_H
#define APP_NATIVE_UI_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "protimer_provider.h"
#include "clockalarm_provider.h"

/**
 * @brief Initialize subjects, create all screen containers and bind observers.
 *        Must be called after lv_init().
 */
void ui_init(void);

/**
 * @brief Register app-side provider operations with the data layer.
 *        Must be called after ui_init().
 *
 * @param pt_ops  Pro Timer provider ops (non-NULL).
 * @param ca_ops  Clock/Alarm provider ops (non-NULL).
 */
void ui_app_register_providers(const protimer_provider_ops_t *pt_ops,
			       const clockalarm_provider_ops_t *ca_ops);

#ifdef __cplusplus
}
#endif

#endif /* APP_NATIVE_UI_UI_H */
