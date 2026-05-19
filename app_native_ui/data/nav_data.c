/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file nav_data.c
 * @brief Navigation data implementation.
 */

#include "data/nav_data.h"

lv_subject_t subj_active_screen;

static enum screen_id s_prev_screen = SCREEN_MENU;

void nav_data_init(void)
{
	lv_subject_init_int(&subj_active_screen, (int32_t)SCREEN_MENU);
}

void nav_data_go_to(enum screen_id screen)
{
	s_prev_screen = (enum screen_id)lv_subject_get_int(&subj_active_screen);
	lv_subject_set_int(&subj_active_screen, (int32_t)screen);
}

void nav_data_go_back(void)
{
	lv_subject_set_int(&subj_active_screen, (int32_t)s_prev_screen);
	s_prev_screen = SCREEN_MENU;
}
