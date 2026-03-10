#include <lvgl.h>
#include "clockalarm_view.h"

static lv_obj_t *lbl_time;
static lv_obj_t *lbl_alarm;
static lv_obj_t *lbl_state;
static lv_obj_t *lbl_status;
static lv_obj_t *btn_set;
static lv_obj_t *btn_ok;
static lv_obj_t *btn_inc;
static lv_obj_t *btn_dec;
static lv_obj_t *btn_back;

static clockalarm_view_btn_cb_t s_btn_cb;

static void btn_event_cb(lv_event_t *e)
{
	if (!s_btn_cb) {
		return;
	}

	lv_obj_t *target = lv_event_get_target(e);

	if (target == btn_set) {
		s_btn_cb(CA_BTN_SET);
	} else if (target == btn_ok) {
		s_btn_cb(CA_BTN_OK);
	} else if (target == btn_inc) {
		s_btn_cb(CA_BTN_INC);
	} else if (target == btn_dec) {
		s_btn_cb(CA_BTN_DEC);
	} else if (target == btn_back) {
		s_btn_cb(CA_BTN_BACK);
	}
}

static lv_obj_t *create_btn(lv_obj_t *parent, const char *text,
			     lv_color_t bg_color, int32_t w)
{
	lv_obj_t *btn = lv_button_create(parent);
	lv_obj_set_size(btn, w, 40);
	lv_obj_set_style_bg_color(btn, bg_color, 0);
	lv_obj_set_style_radius(btn, 6, 0);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

	lv_obj_t *lbl = lv_label_create(btn);
	lv_label_set_text(lbl, text);
	lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
	lv_obj_center(lbl);

	return btn;
}

void clockalarm_view_create(clockalarm_view_btn_cb_t cb)
{
	s_btn_cb = cb;

	lv_obj_t *scr = lv_scr_act();

	lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);
	lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
	lv_obj_set_style_pad_all(scr, 8, 0);

	/* State label (top) */
	lbl_state = lv_label_create(scr);
	lv_obj_set_style_text_color(lbl_state, lv_color_hex(0x00d4ff), 0);
	lv_obj_set_width(lbl_state, lv_pct(100));
	lv_obj_set_style_text_align(lbl_state, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_state, "");
	lv_obj_align(lbl_state, LV_ALIGN_TOP_MID, 0, 2);

	/* Current time (large) */
	lbl_time = lv_label_create(scr);
	lv_obj_set_style_text_color(lbl_time, lv_color_white(), 0);
	lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_28, 0);
	lv_obj_set_width(lbl_time, lv_pct(100));
	lv_obj_set_style_text_align(lbl_time, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_time, "00:00:00");
	lv_obj_align(lbl_time, LV_ALIGN_CENTER, 0, -25);

	/* Alarm time (smaller, below main time) */
	lbl_alarm = lv_label_create(scr);
	lv_obj_set_style_text_color(lbl_alarm, lv_color_hex(0x888888), 0);
	lv_obj_set_style_text_font(lbl_alarm, &lv_font_montserrat_14, 0);
	lv_obj_set_width(lbl_alarm, lv_pct(100));
	lv_obj_set_style_text_align(lbl_alarm, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_alarm, LV_SYMBOL_BELL " 08:00:00 [OFF]");
	lv_obj_align(lbl_alarm, LV_ALIGN_CENTER, 0, 15);

	/* Status message */
	lbl_status = lv_label_create(scr);
	lv_obj_set_style_text_color(lbl_status, lv_color_hex(0xaaaaaa), 0);
	lv_obj_set_width(lbl_status, lv_pct(100));
	lv_obj_set_style_text_align(lbl_status, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_status, "");
	lv_obj_align(lbl_status, LV_ALIGN_CENTER, 0, 38);

	/* Button row */
	lv_obj_t *btn_row = lv_obj_create(scr);
	lv_obj_set_size(btn_row, 304, 46);
	lv_obj_set_style_bg_opa(btn_row, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(btn_row, 0, 0);
	lv_obj_set_style_pad_all(btn_row, 0, 0);
	lv_obj_set_style_pad_gap(btn_row, 4, 0);
	lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_CENTER,
			      LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_align(btn_row, LV_ALIGN_BOTTOM_MID, 0, -2);

	/* 5 buttons: back(44) + set(56) + ok(56) + inc(56) + dec(56) + 4*4gap = 284 */
	btn_back = create_btn(btn_row, LV_SYMBOL_LEFT,
			      lv_palette_main(LV_PALETTE_GREY), 44);
	btn_set  = create_btn(btn_row, LV_SYMBOL_SETTINGS,
			      lv_palette_main(LV_PALETTE_PURPLE), 56);
	btn_ok   = create_btn(btn_row, LV_SYMBOL_OK,
			      lv_palette_main(LV_PALETTE_BLUE), 56);
	btn_inc  = create_btn(btn_row, LV_SYMBOL_PLUS,
			      lv_palette_main(LV_PALETTE_GREEN), 56);
	btn_dec  = create_btn(btn_row, LV_SYMBOL_MINUS,
			      lv_palette_main(LV_PALETTE_ORANGE), 56);
}

void clockalarm_view_set_time(uint8_t h, uint8_t m, uint8_t s)
{
	lv_label_set_text_fmt(lbl_time, "%02u:%02u:%02u", h, m, s);
}

void clockalarm_view_set_alarm(uint8_t h, uint8_t m, uint8_t s, bool enabled)
{
	lv_label_set_text_fmt(lbl_alarm, LV_SYMBOL_BELL " %02u:%02u:%02u [%s]",
			      h, m, s, enabled ? "ON" : "OFF");

	lv_obj_set_style_text_color(lbl_alarm,
				    enabled ? lv_color_hex(0x00ff88)
					    : lv_color_hex(0x888888), 0);
}

void clockalarm_view_set_state(const char *msg)
{
	lv_label_set_text(lbl_state, msg);
}

void clockalarm_view_set_status(const char *msg)
{
	lv_label_set_text(lbl_status, msg);
}
