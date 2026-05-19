/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file screen_menu.c
 * @brief Menu screen — observer-based navigation to app screens.
 */

#include "screens/screen_menu.h"
#include "data/nav_data.h"

#include <lvgl.h>

static void btn_event_cb(lv_event_t *e);
static lv_obj_t *create_menu_btn(lv_obj_t *parent, const char *text,
				  lv_color_t color, enum screen_id target);

static void btn_event_cb(lv_event_t *e)
{
	enum screen_id target = (enum screen_id)(intptr_t)lv_event_get_user_data(e);

	nav_data_go_to(target);
}

static lv_obj_t *create_menu_btn(lv_obj_t *parent, const char *text,
				  lv_color_t color, enum screen_id target)
{
	lv_obj_t *btn = lv_button_create(parent);

	lv_obj_set_size(btn, 200, 60);
	lv_obj_set_style_bg_color(btn, color, 0);
	lv_obj_set_style_radius(btn, 10, 0);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED,
			    (void *)(intptr_t)target);

	lv_obj_t *lbl = lv_label_create(btn);

	lv_label_set_text(lbl, text);
	lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
	lv_obj_center(lbl);

	return btn;
}

void screen_menu_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_obj_create(parent);

	lv_obj_remove_style_all(cont);
	lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
	lv_obj_set_style_bg_color(cont, lv_color_hex(0x1a1a2e), 0);
	lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_bind_flag_if_not_eq(cont, &subj_active_screen,
				   LV_OBJ_FLAG_HIDDEN, (int32_t)SCREEN_MENU);

	lv_obj_t *title = lv_label_create(cont);

	lv_obj_set_style_text_color(title, lv_color_hex(0x00d4ff), 0);
	lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
	lv_obj_set_width(title, lv_pct(100));
	lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(title, "Select Application");
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

	lv_obj_t *btn_col = lv_obj_create(cont);

	lv_obj_remove_style_all(btn_col);
	lv_obj_set_size(btn_col, 280, 180);
	lv_obj_set_style_pad_gap(btn_col, 15, 0);
	lv_obj_set_flex_flow(btn_col, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(btn_col, LV_FLEX_ALIGN_CENTER,
			      LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_align(btn_col, LV_ALIGN_CENTER, 0, 10);

	create_menu_btn(btn_col, LV_SYMBOL_PLAY "  Pro Timer",
			lv_palette_main(LV_PALETTE_BLUE), SCREEN_PROTIMER);
	create_menu_btn(btn_col, LV_SYMBOL_AUDIO "  Clock Alarm",
			lv_palette_main(LV_PALETTE_GREEN), SCREEN_CLOCKALARM);
}
