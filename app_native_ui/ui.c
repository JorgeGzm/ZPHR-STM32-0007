/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file ui.c
 * @brief Native UI initialization and provider registration.
 */

#include "ui.h"
#include "data/ui_data.h"
#include "data/protimer_data.h"
#include "data/clockalarm_data.h"
#include "screens/screen_menu.h"
#include "screens/screen_protimer.h"
#include "screens/screen_clockalarm.h"

#include <lvgl.h>

void ui_init(void)
{
	lv_obj_t *scr = lv_scr_act();

	ui_data_init();

	screen_menu_create(scr);
	screen_protimer_create(scr);
	screen_clockalarm_create(scr);
}

void ui_app_register_providers(const protimer_provider_ops_t *pt_ops,
			       const clockalarm_provider_ops_t *ca_ops)
{
	protimer_data_set_provider(pt_ops);
	clockalarm_data_set_provider(ca_ops);
}
