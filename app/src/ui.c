/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file ui.c
 * @brief App-side UI wiring — adapts state machine models to the native UI
 *        provider interfaces and initializes the complete UI stack.
 */

#include "ui_app.h"

#include "ui.h"
#include "protimer_provider.h"
#include "clockalarm_provider.h"

#include "protimer_model.h"
#include "clockalarm_model.h"

/* Forward declarations of static helpers. */
static void pt_on_screen_enter(const struct protimer_ui_cbs *cbs);
static void ca_on_screen_enter(const struct clockalarm_ui_cbs *cbs);

static const protimer_provider_ops_t s_pt_ops = {
	.on_screen_enter = pt_on_screen_enter,
	.on_screen_exit  = protimer_model_stop,
	.send_event      = (void (*)(int))protimer_model_send_event,
};

static const clockalarm_provider_ops_t s_ca_ops = {
	.on_screen_enter = ca_on_screen_enter,
	.on_screen_exit  = clockalarm_model_stop,
	.send_event      = (void (*)(int))clockalarm_model_send_event,
};

static void pt_on_screen_enter(const struct protimer_ui_cbs *cbs)
{
	/* protimer_ui_cbs and protimer_model_cb are layout-identical. */
	protimer_model_init((const struct protimer_model_cb *)cbs);
}

static void ca_on_screen_enter(const struct clockalarm_ui_cbs *cbs)
{
	/* clockalarm_ui_cbs and clockalarm_model_cb are layout-identical. */
	clockalarm_model_init((const struct clockalarm_model_cb *)cbs);
}

void ui_app_init(void)
{
	ui_init();
	ui_app_register_providers(&s_pt_ops, &s_ca_ops);
}
