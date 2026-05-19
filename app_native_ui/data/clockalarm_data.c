/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file clockalarm_data.c
 * @brief Clock/Alarm data layer — bridges provider callbacks to LVGL subjects.
 */

#include "data/clockalarm_data.h"
#include "data/nav_data.h"

#include <stdbool.h>
#include <stdio.h>

lv_subject_t subj_ca_time;
lv_subject_t subj_ca_alarm_str;
lv_subject_t subj_ca_alarm_en;
lv_subject_t subj_ca_state;
lv_subject_t subj_ca_status;

static const clockalarm_provider_ops_t *s_ops;

static char s_time_buf[12];
static char s_alarm_buf[12];
static char s_state_buf[32];
static char s_status_buf[64];

static void on_time(uint8_t h, uint8_t m, uint8_t s);
static void on_alarm(uint8_t h, uint8_t m, uint8_t s, bool enabled);
static void on_state(const char *msg);
static void on_status(const char *msg);
static void nav_observer_cb(lv_observer_t *obs, lv_subject_t *subj);

static const struct clockalarm_ui_cbs s_ui_cbs = {
	.on_time   = on_time,
	.on_alarm  = on_alarm,
	.on_state  = on_state,
	.on_status = on_status,
};

static void on_time(uint8_t h, uint8_t m, uint8_t s)
{
	snprintf(s_time_buf, sizeof(s_time_buf), "%02u:%02u:%02u", h, m, s);
	lv_subject_set_pointer(&subj_ca_time, s_time_buf);
}

static void on_alarm(uint8_t h, uint8_t m, uint8_t s, bool enabled)
{
	snprintf(s_alarm_buf, sizeof(s_alarm_buf), "%02u:%02u:%02u", h, m, s);
	lv_subject_set_pointer(&subj_ca_alarm_str, s_alarm_buf);
	lv_subject_set_int(&subj_ca_alarm_en, (int32_t)enabled);
}

static void on_state(const char *msg)
{
	snprintf(s_state_buf, sizeof(s_state_buf), "%s", msg);
	lv_subject_set_pointer(&subj_ca_state, s_state_buf);
}

static void on_status(const char *msg)
{
	snprintf(s_status_buf, sizeof(s_status_buf), "%s", msg);
	lv_subject_set_pointer(&subj_ca_status, s_status_buf);
}

static void nav_observer_cb(lv_observer_t *obs, lv_subject_t *subj)
{
	(void)obs;

	if (lv_subject_get_int(subj) == (int32_t)SCREEN_CLOCKALARM) {
		if (s_ops && s_ops->on_screen_enter) {
			s_ops->on_screen_enter(&s_ui_cbs);
		}
	} else {
		if (s_ops && s_ops->on_screen_exit) {
			s_ops->on_screen_exit();
		}
	}
}

void clockalarm_data_init(void)
{
	lv_subject_init_pointer(&subj_ca_time, "00:00:00");
	lv_subject_init_pointer(&subj_ca_alarm_str, "00:00:00");
	lv_subject_init_int(&subj_ca_alarm_en, 0);
	lv_subject_init_pointer(&subj_ca_state, "");
	lv_subject_init_pointer(&subj_ca_status, "");

	lv_subject_add_observer(&subj_active_screen, nav_observer_cb, NULL);
}

void clockalarm_data_set_provider(const clockalarm_provider_ops_t *ops)
{
	s_ops = ops;
}

void clockalarm_data_send_event(enum clockalarm_ui_evt evt)
{
	if (s_ops && s_ops->send_event) {
		s_ops->send_event((int)evt);
	}
}
