#include <lvgl.h>
#include "menu_screen.h"

static menu_screen_cb_t s_cb;

static void btn_event_cb(lv_event_t *e)
{
	if (!s_cb) {
		return;
	}

	enum menu_selection sel = (enum menu_selection)(intptr_t)lv_event_get_user_data(e);
	s_cb(sel);
}

static lv_obj_t *create_menu_btn(lv_obj_t *parent, const char *text,
				  lv_color_t color, enum menu_selection sel)
{
	lv_obj_t *btn = lv_button_create(parent);
	lv_obj_set_size(btn, 200, 60);
	lv_obj_set_style_bg_color(btn, color, 0);
	lv_obj_set_style_radius(btn, 10, 0);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED,
			    (void *)(intptr_t)sel);

	lv_obj_t *lbl = lv_label_create(btn);
	lv_label_set_text(lbl, text);
	lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
	lv_obj_set_style_text_font(lbl, &lv_font_montserrat_14, 0);
	lv_obj_center(lbl);

	return btn;
}

void menu_screen_create(menu_screen_cb_t cb)
{
	s_cb = cb;

	lv_obj_t *scr = lv_scr_act();

	lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);
	lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

	/* Title */
	lv_obj_t *title = lv_label_create(scr);
	lv_obj_set_style_text_color(title, lv_color_hex(0x00d4ff), 0);
	lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
	lv_obj_set_width(title, lv_pct(100));
	lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(title, "Select Application");
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

	/* Button container */
	lv_obj_t *cont = lv_obj_create(scr);
	lv_obj_set_size(cont, 280, 180);
	lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(cont, 0, 0);
	lv_obj_set_style_pad_all(cont, 0, 0);
	lv_obj_set_style_pad_gap(cont, 15, 0);
	lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER,
			      LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_align(cont, LV_ALIGN_CENTER, 0, 10);

	create_menu_btn(cont, LV_SYMBOL_PLAY "  Pro Timer",
			lv_palette_main(LV_PALETTE_BLUE), MENU_PROTIMER);
	create_menu_btn(cont, LV_SYMBOL_AUDIO "  Clock Alarm",
			lv_palette_main(LV_PALETTE_GREEN), MENU_CLOCKALARM);
}
