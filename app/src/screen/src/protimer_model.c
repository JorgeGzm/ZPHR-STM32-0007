/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file protimer_model.c
 * @brief Pro Timer state machine — 5-state SMF with 100 ms tick.
 */

#include "protimer_model.h"

#include <zephyr/kernel.h>
#include <zephyr/smf.h>

enum protimer_state_id {
	STATE_IDLE,
	STATE_TIME_SET,
	STATE_COUNTDOWN,
	STATE_PAUSE,
	STATE_STAT,
	STATE_COUNT,
};

struct protimer_obj {
	struct smf_ctx       ctx;
	enum protimer_event  event;
	uint32_t             curr_time;
	uint32_t             elapsed_time;
	uint32_t             pro_time;
	uint8_t              tick_count;
};

/* Forward declarations of static helpers. */
static void tick_timer_handler(struct k_timer *timer);
static void notify_time(uint32_t seconds);
static void notify_status(const char *msg);
static void notify_state(const char *msg);
static void idle_entry(void *obj);
static enum smf_state_result idle_run(void *obj);
static void time_set_entry(void *obj);
static enum smf_state_result time_set_run(void *obj);
static void countdown_entry(void *obj);
static enum smf_state_result countdown_run(void *obj);
static void countdown_exit(void *obj);
static void pause_entry(void *obj);
static enum smf_state_result pause_run(void *obj);
static void stat_entry(void *obj);
static enum smf_state_result stat_run(void *obj);

static struct protimer_obj s_obj;
static const struct protimer_model_cb *s_cb;

K_MSGQ_DEFINE(protimer_msgq, sizeof(enum protimer_event), 16, 4);
K_TIMER_DEFINE(tick_timer, tick_timer_handler, NULL);

static const struct smf_state protimer_states[] = {
	[STATE_IDLE]      = SMF_CREATE_STATE(idle_entry, idle_run, NULL, NULL, NULL),
	[STATE_TIME_SET]  = SMF_CREATE_STATE(time_set_entry, time_set_run, NULL, NULL, NULL),
	[STATE_COUNTDOWN] = SMF_CREATE_STATE(countdown_entry, countdown_run,
					     countdown_exit, NULL, NULL),
	[STATE_PAUSE]     = SMF_CREATE_STATE(pause_entry, pause_run, NULL, NULL, NULL),
	[STATE_STAT]      = SMF_CREATE_STATE(stat_entry, stat_run, NULL, NULL, NULL),
};

static void tick_timer_handler(struct k_timer *timer)
{
	enum protimer_event evt = EVT_TIME_TICK;

	k_msgq_put(&protimer_msgq, &evt, K_NO_WAIT);
}

static void notify_time(uint32_t seconds)
{
	if (s_cb && s_cb->on_time) {
		s_cb->on_time(seconds);
	}
}

static void notify_status(const char *msg)
{
	if (s_cb && s_cb->on_status) {
		s_cb->on_status(msg);
	}
}

static void notify_state(const char *msg)
{
	if (s_cb && s_cb->on_state) {
		s_cb->on_state(msg);
	}
}

/* IDLE */
static void idle_entry(void *obj)
{
	struct protimer_obj *o = obj;

	o->curr_time    = 0;
	o->elapsed_time = 0;
	o->tick_count   = 0;

	notify_state("IDLE");
	notify_time(0);
	notify_status("Set time");
}

static enum smf_state_result idle_run(void *obj)
{
	struct protimer_obj *o = obj;

	switch (o->event) {
	case EVT_INC_TIME:
		o->curr_time += 60;
		smf_set_state(SMF_CTX(o), &protimer_states[STATE_TIME_SET]);
		break;
	case EVT_START_PAUSE:
		smf_set_state(SMF_CTX(o), &protimer_states[STATE_STAT]);
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

/* TIME_SET */
static void time_set_entry(void *obj)
{
	struct protimer_obj *o = obj;

	notify_state("TIME SET");
	notify_time(o->curr_time);
	notify_status("Adjust time");
}

static enum smf_state_result time_set_run(void *obj)
{
	struct protimer_obj *o = obj;

	switch (o->event) {
	case EVT_INC_TIME:
		o->curr_time += 60;
		notify_time(o->curr_time);
		break;
	case EVT_DEC_TIME:
		if (o->curr_time >= 60) {
			o->curr_time -= 60;
			notify_time(o->curr_time);
		}
		break;
	case EVT_START_PAUSE:
		if (o->curr_time >= 60) {
			smf_set_state(SMF_CTX(o), &protimer_states[STATE_COUNTDOWN]);
		}
		break;
	case EVT_ABRT:
		smf_set_state(SMF_CTX(o), &protimer_states[STATE_IDLE]);
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

/* COUNTDOWN */
static void countdown_entry(void *obj)
{
	struct protimer_obj *o = obj;

	o->tick_count = 0;
	notify_state("COUNTDOWN");
	notify_time(o->curr_time);
	notify_status("Running...");
}

static enum smf_state_result countdown_run(void *obj)
{
	struct protimer_obj *o = obj;

	switch (o->event) {
	case EVT_TIME_TICK:
		o->tick_count++;
		if (o->tick_count >= 10) {
			o->tick_count = 0;
			if (o->curr_time > 0) {
				o->curr_time--;
				o->elapsed_time++;
				notify_time(o->curr_time);
			}
			if (o->curr_time == 0) {
				smf_set_state(SMF_CTX(o), &protimer_states[STATE_IDLE]);
			}
		}
		break;
	case EVT_START_PAUSE:
		smf_set_state(SMF_CTX(o), &protimer_states[STATE_PAUSE]);
		break;
	case EVT_ABRT:
		smf_set_state(SMF_CTX(o), &protimer_states[STATE_IDLE]);
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

static void countdown_exit(void *obj)
{
	struct protimer_obj *o = obj;

	o->pro_time    += o->elapsed_time;
	o->elapsed_time = 0;
}

/* PAUSE */
static void pause_entry(void *obj)
{
	struct protimer_obj *o = obj;

	notify_state("PAUSED");
	notify_time(o->curr_time);
	notify_status("Paused");
}

static enum smf_state_result pause_run(void *obj)
{
	struct protimer_obj *o = obj;

	switch (o->event) {
	case EVT_INC_TIME:
		o->curr_time += 60;
		smf_set_state(SMF_CTX(o), &protimer_states[STATE_TIME_SET]);
		break;
	case EVT_DEC_TIME:
		if (o->curr_time >= 60) {
			o->curr_time -= 60;
		}
		smf_set_state(SMF_CTX(o), &protimer_states[STATE_TIME_SET]);
		break;
	case EVT_START_PAUSE:
		smf_set_state(SMF_CTX(o), &protimer_states[STATE_COUNTDOWN]);
		break;
	case EVT_ABRT:
		smf_set_state(SMF_CTX(o), &protimer_states[STATE_IDLE]);
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

/* STAT */
static void stat_entry(void *obj)
{
	struct protimer_obj *o = obj;

	o->tick_count = 0;
	notify_state("STATISTICS");
	notify_time(o->pro_time);
	notify_status("Productive time");
}

static enum smf_state_result stat_run(void *obj)
{
	struct protimer_obj *o = obj;

	switch (o->event) {
	case EVT_TIME_TICK:
		o->tick_count++;
		if (o->tick_count >= 30) {
			smf_set_state(SMF_CTX(o), &protimer_states[STATE_IDLE]);
		}
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

void protimer_model_init(const struct protimer_model_cb *cb)
{
	s_cb = cb;
	smf_set_initial(SMF_CTX(&s_obj), &protimer_states[STATE_IDLE]);
	k_timer_start(&tick_timer, K_MSEC(100), K_MSEC(100));
}

void protimer_model_stop(void)
{
	k_timer_stop(&tick_timer);
	k_msgq_purge(&protimer_msgq);
}

void protimer_model_send_event(enum protimer_event evt)
{
	k_msgq_put(&protimer_msgq, &evt, K_NO_WAIT);
}

void protimer_model_process(void)
{
	enum protimer_event evt;

	while (k_msgq_get(&protimer_msgq, &evt, K_NO_WAIT) == 0) {
		s_obj.event = evt;
		smf_run_state(SMF_CTX(&s_obj));
	}
}
