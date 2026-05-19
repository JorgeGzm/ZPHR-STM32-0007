/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file main.c
 * @brief SDL2 simulator entry — stub providers, no Zephyr dependency.
 */

#include "ui.h"
#include "protimer_provider.h"
#include "clockalarm_provider.h"

#include <lvgl.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

/* --- SDL2 display and input glue ----------------------------------------- */

static lv_display_t *s_disp;
static lv_indev_t   *s_indev;

static void sdl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *buf)
{
	/* Provided by lv_drivers/sdl — declare extern or replace with your own. */
	extern void lv_sdl_flush(lv_display_t *disp, const lv_area_t *area,
				 uint8_t *buf);
	lv_sdl_flush(disp, area, buf);
}

static void sdl_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
	extern void lv_sdl_mouse_read(lv_indev_t *indev, lv_indev_data_t *data);
	lv_sdl_mouse_read(indev, data);
}

/* --- Stub providers ------------------------------------------------------- */

static const struct protimer_ui_cbs *s_pt_cbs;
static const struct clockalarm_ui_cbs *s_ca_cbs;

static void pt_on_screen_enter(const struct protimer_ui_cbs *cbs)
{
	s_pt_cbs = cbs;
	printf("[protimer] screen enter\n");
	if (cbs->on_state) {
		cbs->on_state("IDLE");
	}
	if (cbs->on_time) {
		cbs->on_time(0);
	}
	if (cbs->on_status) {
		cbs->on_status("Set time");
	}
}

static void pt_on_screen_exit(void)
{
	printf("[protimer] screen exit\n");
	s_pt_cbs = NULL;
}

static void pt_send_event(int evt)
{
	printf("[protimer] event %d\n", evt);
}

static const protimer_provider_ops_t s_pt_ops = {
	.on_screen_enter = pt_on_screen_enter,
	.on_screen_exit  = pt_on_screen_exit,
	.send_event      = pt_send_event,
};

static void ca_on_screen_enter(const struct clockalarm_ui_cbs *cbs)
{
	s_ca_cbs = cbs;
	printf("[clockalarm] screen enter\n");
	if (cbs->on_state) {
		cbs->on_state("CLOCK");
	}
	if (cbs->on_time) {
		cbs->on_time(10, 10, 0);
	}
	if (cbs->on_alarm) {
		cbs->on_alarm(8, 0, 0, false);
	}
	if (cbs->on_status) {
		cbs->on_status("SET=config  OK=alarm");
	}
}

static void ca_on_screen_exit(void)
{
	printf("[clockalarm] screen exit\n");
	s_ca_cbs = NULL;
}

static void ca_send_event(int evt)
{
	printf("[clockalarm] event %d\n", evt);
}

static const clockalarm_provider_ops_t s_ca_ops = {
	.on_screen_enter = ca_on_screen_enter,
	.on_screen_exit  = ca_on_screen_exit,
	.send_event      = ca_send_event,
};

/* --- Main ----------------------------------------------------------------- */

int main(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	lv_init();

	s_disp = lv_display_create(320, 240);
	lv_display_set_flush_cb(s_disp, sdl_flush_cb);

	s_indev = lv_indev_create();
	lv_indev_set_type(s_indev, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(s_indev, sdl_read_cb);

	ui_init();
	ui_app_register_providers(&s_pt_ops, &s_ca_ops);

	bool running = true;

	while (running) {
		SDL_Event e;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}

		lv_timer_handler();
		SDL_Delay(5);
	}

	SDL_Quit();
	return 0;
}
