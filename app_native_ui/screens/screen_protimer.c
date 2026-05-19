/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file screen_protimer.c
 * @brief Pro Timer screen — widgets bound to subjects via observers.
 */

#include "screens/screen_protimer.h"
#include "data/protimer_data.h"
#include "data/nav_data.h"

#include <lvgl.h>

static void btn_event_cb(lv_event_t *e);
static void ptr_obs_cb(lv_observer_t *obs, lv_subject_t *subj);
static lv_obj_t *create_btn(lv_obj_t *parent, const char *text,
			     lv_color_t bg_color, int32_t w);

static void btn_event_cb(lv_event_t *e)
{
	enum protimer_ui_evt evt =
		(enum protimer_ui_evt)(intptr_t)lv_event_get_user_data(e);

	if (evt == (enum protimer_ui_evt)-1) {
		nav_data_go_back();
		return;
	}

	protimer_data_send_event(evt);
}

static void ptr_obs_cb(lv_observer_t *obs, lv_subject_t *subj)
{
	lv_obj_t *lbl = (lv_obj_t *)lv_observer_get_target(obs);

	lv_label_set_text(lbl, (const char *)lv_subject_get_pointer(subj));
}

static lv_obj_t *create_btn(lv_obj_t *parent, const char *text,
			     lv_color_t bg_color, int32_t w)
{
	lv_obj_t *btn = lv_button_create(parent);

	lv_obj_set_size(btn, w, 44);
	lv_obj_set_style_bg_color(btn, bg_color, 0);
	lv_obj_set_style_radius(btn, 6, 0);

	lv_obj_t *lbl = lv_label_create(btn);

	lv_label_set_text(lbl, text);
	lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
	lv_obj_center(lbl);

	return btn;
}

void screen_protimer_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_obj_create(parent);

	lv_obj_remove_style_all(cont);
	lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
	lv_obj_set_style_bg_color(cont, lv_color_hex(0x1a1a2e), 0);
	lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
	lv_obj_set_style_pad_all(cont, 8, 0);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_bind_flag_if_not_eq(cont, &subj_active_screen,
				   LV_OBJ_FLAG_HIDDEN, (int32_t)SCREEN_PROTIMER);

	lv_obj_t *lbl_state = lv_label_create(cont);

	lv_obj_set_style_text_color(lbl_state, lv_color_hex(0x00d4ff), 0);
	lv_obj_set_width(lbl_state, lv_pct(100));
	lv_obj_set_style_text_align(lbl_state, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_state, "");
	lv_obj_align(lbl_state, LV_ALIGN_TOP_MID, 0, 4);
	lv_subject_add_observer_obj(&subj_pt_state, ptr_obs_cb, lbl_state, NULL);

	lv_obj_t *lbl_time = lv_label_create(cont);

	lv_obj_set_style_text_color(lbl_time, lv_color_white(), 0);
	lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_28, 0);
	lv_obj_set_width(lbl_time, lv_pct(100));
	lv_obj_set_style_text_align(lbl_time, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_time, "000:00");
	lv_obj_align(lbl_time, LV_ALIGN_CENTER, 0, -20);
	lv_subject_add_observer_obj(&subj_pt_time, ptr_obs_cb, lbl_time, NULL);

	lv_obj_t *lbl_status = lv_label_create(cont);

	lv_obj_set_style_text_color(lbl_status, lv_color_hex(0xaaaaaa), 0);
	lv_obj_set_width(lbl_status, lv_pct(100));
	lv_obj_set_style_text_align(lbl_status, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_status, "");
	lv_obj_align(lbl_status, LV_ALIGN_CENTER, 0, 30);
	lv_subject_add_observer_obj(&subj_pt_status, ptr_obs_cb, lbl_status, NULL);

	lv_obj_t *btn_row = lv_obj_create(cont);

	lv_obj_remove_style_all(btn_row);
	lv_obj_set_size(btn_row, 304, 50);
	lv_obj_set_style_pad_gap(btn_row, 6, 0);
	lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_CENTER,
			      LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_align(btn_row, LV_ALIGN_BOTTOM_MID, 0, -2);

	lv_obj_t *btn;

	btn = create_btn(btn_row, LV_SYMBOL_LEFT,
			 lv_palette_main(LV_PALETTE_GREY), 44);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED,
			    (void *)(intptr_t)-1);

	btn = create_btn(btn_row, LV_SYMBOL_PLUS,
			 lv_palette_main(LV_PALETTE_GREEN), 56);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED,
			    (void *)(intptr_t)PT_UI_EVT_INC_TIME);

	btn = create_btn(btn_row, LV_SYMBOL_MINUS,
			 lv_palette_main(LV_PALETTE_ORANGE), 56);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED,
			    (void *)(intptr_t)PT_UI_EVT_DEC_TIME);

	btn = create_btn(btn_row, LV_SYMBOL_PLAY,
			 lv_palette_main(LV_PALETTE_BLUE), 72);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED,
			    (void *)(intptr_t)PT_UI_EVT_START_PAUSE);

	btn = create_btn(btn_row, LV_SYMBOL_STOP,
			 lv_palette_main(LV_PALETTE_RED), 56);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED,
			    (void *)(intptr_t)PT_UI_EVT_ABRT);
}
