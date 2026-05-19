/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file clockalarm_data.h
 * @brief Clock/Alarm LVGL subjects and data layer.
 */

#ifndef APP_NATIVE_UI_CLOCKALARM_DATA_H
#define APP_NATIVE_UI_CLOCKALARM_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl.h>
#include "clockalarm_provider.h"

extern lv_subject_t subj_ca_time;
extern lv_subject_t subj_ca_alarm_str;
extern lv_subject_t subj_ca_alarm_en;
extern lv_subject_t subj_ca_state;
extern lv_subject_t subj_ca_status;

void clockalarm_data_init(void);
void clockalarm_data_set_provider(const clockalarm_provider_ops_t *ops);
void clockalarm_data_send_event(enum clockalarm_ui_evt evt);

#ifdef __cplusplus
}
#endif

#endif /* APP_NATIVE_UI_CLOCKALARM_DATA_H */
