/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file protimer_data.c
 * @brief Pro Timer data layer — bridges provider callbacks to LVGL subjects.
 */

#include "data/protimer_data.h"
#include "data/nav_data.h"

#include <stdio.h>

lv_subject_t subj_pt_time;
lv_subject_t subj_pt_status;
lv_subject_t subj_pt_state;

static const protimer_provider_ops_t *s_ops;

static char s_time_buf[12];
static char s_status_buf[64];
static char s_state_buf[32];

static void on_time(uint32_t seconds);
static void on_status(const char *msg);
static void on_state(const char *msg);
static void nav_observer_cb(lv_observer_t *obs, lv_subject_t *subj);

static const struct protimer_ui_cbs s_ui_cbs = {
	.on_time   = on_time,
	.on_status = on_status,
	.on_state  = on_state,
};

static void on_time(uint32_t seconds)
{
	uint32_t min = seconds / 60;
	uint32_t sec = seconds % 60;

	snprintf(s_time_buf, sizeof(s_time_buf), "%03u:%02u", min, sec);
	lv_subject_set_pointer(&subj_pt_time, s_time_buf);
}

static void on_status(const char *msg)
{
	snprintf(s_status_buf, sizeof(s_status_buf), "%s", msg);
	lv_subject_set_pointer(&subj_pt_status, s_status_buf);
}

static void on_state(const char *msg)
{
	snprintf(s_state_buf, sizeof(s_state_buf), "%s", msg);
	lv_subject_set_pointer(&subj_pt_state, s_state_buf);
}

static void nav_observer_cb(lv_observer_t *obs, lv_subject_t *subj)
{
	(void)obs;

	if (lv_subject_get_int(subj) == (int32_t)SCREEN_PROTIMER) {
		if (s_ops && s_ops->on_screen_enter) {
			s_ops->on_screen_enter(&s_ui_cbs);
		}
	} else {
		if (s_ops && s_ops->on_screen_exit) {
			s_ops->on_screen_exit();
		}
	}
}

void protimer_data_init(void)
{
	lv_subject_init_pointer(&subj_pt_time, "000:00");
	lv_subject_init_pointer(&subj_pt_status, "");
	lv_subject_init_pointer(&subj_pt_state, "");

	lv_subject_add_observer(&subj_active_screen, nav_observer_cb, NULL);
}

void protimer_data_set_provider(const protimer_provider_ops_t *ops)
{
	s_ops = ops;
}

void protimer_data_send_event(enum protimer_ui_evt evt)
{
	if (s_ops && s_ops->send_event) {
		s_ops->send_event((int)evt);
	}
}
