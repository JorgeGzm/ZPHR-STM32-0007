/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file protimer_data.h
 * @brief Pro Timer LVGL subjects and data layer.
 */

#ifndef APP_NATIVE_UI_PROTIMER_DATA_H
#define APP_NATIVE_UI_PROTIMER_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>
#include "protimer_provider.h"

extern lv_subject_t subj_pt_time;
extern lv_subject_t subj_pt_status;
extern lv_subject_t subj_pt_state;

void protimer_data_init(void);
void protimer_data_set_provider(const protimer_provider_ops_t *ops);
void protimer_data_send_event(enum protimer_ui_evt evt);

#ifdef __cplusplus
}
#endif

#endif /* APP_NATIVE_UI_PROTIMER_DATA_H */
