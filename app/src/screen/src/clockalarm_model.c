/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file clockalarm_model.c
 * @brief Clock/Alarm state machine — 4-state SMF with 1-second tick.
 */

#include "clockalarm_model.h"

#include <zephyr/kernel.h>
#include <zephyr/smf.h>

enum ca_state_id {
	CA_STATE_TICKING,
	CA_STATE_CLOCK_SETTING,
	CA_STATE_ALARM_SETTING,
	CA_STATE_ALARM_NOTIFY,
	CA_STATE_COUNT,
};

enum setting_field {
	FIELD_HOUR,
	FIELD_MIN,
	FIELD_SEC,
	FIELD_ONOFF,
};

struct ca_obj {
	struct smf_ctx         ctx;
	enum clockalarm_event  event;
	uint8_t  hour;
	uint8_t  min;
	uint8_t  sec;
	uint8_t  alarm_hour;
	uint8_t  alarm_min;
	uint8_t  alarm_sec;
	bool     alarm_enabled;
	uint8_t  edit_h;
	uint8_t  edit_m;
	uint8_t  edit_s;
	bool     edit_onoff;
	enum setting_field field;
	uint8_t  blink_count;
	bool     blink_on;
};

/* Forward declarations of static helpers. */
static void tick_handler(struct k_timer *timer);
static void notify_time(void);
static void notify_alarm(void);
static void notify_state(const char *msg);
static void notify_status(const char *msg);
static void notify_edit_time(void);
static const char *field_name(enum setting_field f);
static void adjust_field(struct ca_obj *o, int dir);
static void ticking_entry(void *obj);
static enum smf_state_result ticking_run(void *obj);
static void clock_setting_entry(void *obj);
static enum smf_state_result clock_setting_run(void *obj);
static void alarm_setting_entry(void *obj);
static enum smf_state_result alarm_setting_run(void *obj);
static void alarm_notify_entry(void *obj);
static enum smf_state_result alarm_notify_run(void *obj);

static struct ca_obj s_obj;
static const struct clockalarm_model_cb *s_cb;

K_MSGQ_DEFINE(ca_msgq, sizeof(enum clockalarm_event), 16, 4);
K_TIMER_DEFINE(ca_timer, tick_handler, NULL);

static const struct smf_state ca_states[] = {
	[CA_STATE_TICKING]       = SMF_CREATE_STATE(ticking_entry, ticking_run,
						     NULL, NULL, NULL),
	[CA_STATE_CLOCK_SETTING] = SMF_CREATE_STATE(clock_setting_entry,
						     clock_setting_run,
						     NULL, NULL, NULL),
	[CA_STATE_ALARM_SETTING] = SMF_CREATE_STATE(alarm_setting_entry,
						     alarm_setting_run,
						     NULL, NULL, NULL),
	[CA_STATE_ALARM_NOTIFY]  = SMF_CREATE_STATE(alarm_notify_entry,
						     alarm_notify_run,
						     NULL, NULL, NULL),
};

static void tick_handler(struct k_timer *timer)
{
	enum clockalarm_event evt = CA_EVT_TICK;

	k_msgq_put(&ca_msgq, &evt, K_NO_WAIT);
}

static void notify_time(void)
{
	if (s_cb && s_cb->on_time) {
		s_cb->on_time(s_obj.hour, s_obj.min, s_obj.sec);
	}
}

static void notify_alarm(void)
{
	if (s_cb && s_cb->on_alarm) {
		s_cb->on_alarm(s_obj.alarm_hour, s_obj.alarm_min,
			       s_obj.alarm_sec, s_obj.alarm_enabled);
	}
}

static void notify_state(const char *msg)
{
	if (s_cb && s_cb->on_state) {
		s_cb->on_state(msg);
	}
}

static void notify_status(const char *msg)
{
	if (s_cb && s_cb->on_status) {
		s_cb->on_status(msg);
	}
}

static void notify_edit_time(void)
{
	if (s_cb && s_cb->on_time) {
		s_cb->on_time(s_obj.edit_h, s_obj.edit_m, s_obj.edit_s);
	}
}

static const char *field_name(enum setting_field f)
{
	switch (f) {
	case FIELD_HOUR:  return "Adjust HOUR (+/-)";
	case FIELD_MIN:   return "Adjust MINUTE (+/-)";
	case FIELD_SEC:   return "Adjust SECOND (+/-)";
	case FIELD_ONOFF: return "Toggle ON/OFF (+/-)";
	default:          return "";
	}
}

static void adjust_field(struct ca_obj *o, int dir)
{
	switch (o->field) {
	case FIELD_HOUR:
		o->edit_h = (o->edit_h + 24 + dir) % 24;
		break;
	case FIELD_MIN:
		o->edit_m = (o->edit_m + 60 + dir) % 60;
		break;
	case FIELD_SEC:
		o->edit_s = (o->edit_s + 60 + dir) % 60;
		break;
	case FIELD_ONOFF:
		o->edit_onoff = !o->edit_onoff;
		break;
	}
}

/* TICKING */
static void ticking_entry(void *obj)
{
	struct ca_obj *o = obj;

	notify_state("CLOCK");
	notify_time();
	notify_alarm();
	notify_status("SET=config  OK=alarm");

	k_timer_start(&ca_timer, K_SECONDS(1), K_SECONDS(1));

	(void)o;
}

static enum smf_state_result ticking_run(void *obj)
{
	struct ca_obj *o = obj;

	switch (o->event) {
	case CA_EVT_TICK:
		o->sec++;
		if (o->sec >= 60) {
			o->sec = 0;
			o->min++;
			if (o->min >= 60) {
				o->min = 0;
				o->hour++;
				if (o->hour >= 24) {
					o->hour = 0;
				}
			}
		}
		notify_time();

		if (o->alarm_enabled &&
		    o->hour == o->alarm_hour &&
		    o->min  == o->alarm_min  &&
		    o->sec  == o->alarm_sec) {
			smf_set_state(SMF_CTX(o), &ca_states[CA_STATE_ALARM_NOTIFY]);
		}
		break;
	case CA_EVT_SET:
		smf_set_state(SMF_CTX(o), &ca_states[CA_STATE_CLOCK_SETTING]);
		break;
	case CA_EVT_OK:
		smf_set_state(SMF_CTX(o), &ca_states[CA_STATE_ALARM_SETTING]);
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

/* CLOCK_SETTING */
static void clock_setting_entry(void *obj)
{
	struct ca_obj *o = obj;

	k_timer_stop(&ca_timer);

	o->edit_h = o->hour;
	o->edit_m = o->min;
	o->edit_s = o->sec;
	o->field  = FIELD_HOUR;

	notify_state("SETTING CLOCK");
	notify_edit_time();
	notify_status(field_name(o->field));
}

static enum smf_state_result clock_setting_run(void *obj)
{
	struct ca_obj *o = obj;

	switch (o->event) {
	case CA_EVT_INC:
		adjust_field(o, 1);
		notify_edit_time();
		break;
	case CA_EVT_DEC:
		adjust_field(o, -1);
		notify_edit_time();
		break;
	case CA_EVT_OK:
		if (o->field == FIELD_HOUR) {
			o->field = FIELD_MIN;
		} else if (o->field == FIELD_MIN) {
			o->field = FIELD_SEC;
		} else {
			o->hour = o->edit_h;
			o->min  = o->edit_m;
			o->sec  = o->edit_s;
			smf_set_state(SMF_CTX(o), &ca_states[CA_STATE_TICKING]);
			return SMF_EVENT_HANDLED;
		}
		notify_status(field_name(o->field));
		break;
	case CA_EVT_ABRT:
		smf_set_state(SMF_CTX(o), &ca_states[CA_STATE_TICKING]);
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

/* ALARM_SETTING */
static void alarm_setting_entry(void *obj)
{
	struct ca_obj *o = obj;

	k_timer_stop(&ca_timer);

	o->edit_h     = o->alarm_hour;
	o->edit_m     = o->alarm_min;
	o->edit_s     = o->alarm_sec;
	o->edit_onoff = o->alarm_enabled;
	o->field      = FIELD_HOUR;

	notify_state("SETTING ALARM");
	notify_edit_time();
	notify_status(field_name(o->field));
}

static enum smf_state_result alarm_setting_run(void *obj)
{
	struct ca_obj *o = obj;

	switch (o->event) {
	case CA_EVT_INC:
		adjust_field(o, 1);
		if (o->field == FIELD_ONOFF) {
			notify_status(o->edit_onoff ? "Alarm: ON" : "Alarm: OFF");
		} else {
			notify_edit_time();
		}
		break;
	case CA_EVT_DEC:
		adjust_field(o, -1);
		if (o->field == FIELD_ONOFF) {
			notify_status(o->edit_onoff ? "Alarm: ON" : "Alarm: OFF");
		} else {
			notify_edit_time();
		}
		break;
	case CA_EVT_OK:
		if (o->field == FIELD_HOUR) {
			o->field = FIELD_MIN;
		} else if (o->field == FIELD_MIN) {
			o->field = FIELD_SEC;
		} else if (o->field == FIELD_SEC) {
			o->field = FIELD_ONOFF;
		} else {
			o->alarm_hour    = o->edit_h;
			o->alarm_min     = o->edit_m;
			o->alarm_sec     = o->edit_s;
			o->alarm_enabled = o->edit_onoff;
			smf_set_state(SMF_CTX(o), &ca_states[CA_STATE_TICKING]);
			return SMF_EVENT_HANDLED;
		}
		notify_status(field_name(o->field));
		if (o->field == FIELD_ONOFF) {
			notify_status(o->edit_onoff ? "Alarm: ON" : "Alarm: OFF");
		}
		break;
	case CA_EVT_ABRT:
		smf_set_state(SMF_CTX(o), &ca_states[CA_STATE_TICKING]);
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

/* ALARM_NOTIFY */
static void alarm_notify_entry(void *obj)
{
	struct ca_obj *o = obj;

	o->blink_count = 0;
	o->blink_on    = true;

	notify_state("!! ALARM !!");
	notify_status("Press any button to dismiss");

	/* 500 ms blink period. */
	k_timer_start(&ca_timer, K_MSEC(500), K_MSEC(500));
}

static enum smf_state_result alarm_notify_run(void *obj)
{
	struct ca_obj *o = obj;

	switch (o->event) {
	case CA_EVT_TICK:
		o->blink_on = !o->blink_on;
		notify_state(o->blink_on ? "!! ALARM !!" : "");
		o->blink_count++;
		if (o->blink_count >= 60) {
			/* Auto-dismiss after 30 seconds. */
			smf_set_state(SMF_CTX(o), &ca_states[CA_STATE_TICKING]);
		}
		break;
	case CA_EVT_SET:
	case CA_EVT_OK:
	case CA_EVT_INC:
	case CA_EVT_DEC:
	case CA_EVT_ABRT:
		smf_set_state(SMF_CTX(o), &ca_states[CA_STATE_TICKING]);
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

void clockalarm_model_init(const struct clockalarm_model_cb *cb)
{
	s_cb = cb;

	s_obj.hour = 10;
	s_obj.min  = 10;
	s_obj.sec  = 10;

	s_obj.alarm_hour    = 8;
	s_obj.alarm_min     = 0;
	s_obj.alarm_sec     = 0;
	s_obj.alarm_enabled = false;

	smf_set_initial(SMF_CTX(&s_obj), &ca_states[CA_STATE_TICKING]);
}

void clockalarm_model_stop(void)
{
	k_timer_stop(&ca_timer);
	k_msgq_purge(&ca_msgq);
}

void clockalarm_model_send_event(enum clockalarm_event evt)
{
	k_msgq_put(&ca_msgq, &evt, K_NO_WAIT);
}

void clockalarm_model_process(void)
{
	enum clockalarm_event evt;

	while (k_msgq_get(&ca_msgq, &evt, K_NO_WAIT) == 0) {
		s_obj.event = evt;
		smf_run_state(SMF_CTX(&s_obj));
	}
}
