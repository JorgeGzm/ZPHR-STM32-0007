#include <lvgl.h>
#include "protimer_view.h"

static lv_obj_t *lbl_time;
static lv_obj_t *lbl_status;
static lv_obj_t *lbl_state;
static lv_obj_t *btn_back;
static lv_obj_t *btn_inc;
static lv_obj_t *btn_dec;
static lv_obj_t *btn_sp;
static lv_obj_t *btn_abrt;

static protimer_view_btn_cb_t s_btn_cb;

static void btn_event_cb(lv_event_t *e)
{
	if (!s_btn_cb) {
		return;
	}

	lv_obj_t *target = lv_event_get_target(e);

	if (target == btn_inc) {
		s_btn_cb(BTN_INC);
	} else if (target == btn_dec) {
		s_btn_cb(BTN_DEC);
	} else if (target == btn_sp) {
		s_btn_cb(BTN_START_PAUSE);
	} else if (target == btn_abrt) {
		s_btn_cb(BTN_ABORT);
	} else if (target == btn_back) {
		s_btn_cb(BTN_BACK);
	}
}

static lv_obj_t *create_btn(lv_obj_t *parent, const char *text,
			     lv_color_t bg_color, int32_t w)
{
	lv_obj_t *btn = lv_button_create(parent);
	lv_obj_set_size(btn, w, 44);
	lv_obj_set_style_bg_color(btn, bg_color, 0);
	lv_obj_set_style_radius(btn, 6, 0);
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

	lv_obj_t *lbl = lv_label_create(btn);
	lv_label_set_text(lbl, text);
	lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
	lv_obj_center(lbl);

	return btn;
}

void protimer_view_create(protimer_view_btn_cb_t cb)
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
	lv_obj_align(lbl_state, LV_ALIGN_TOP_MID, 0, 4);

	/* Time display */
	lbl_time = lv_label_create(scr);
	lv_obj_set_style_text_color(lbl_time, lv_color_white(), 0);
	lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_28, 0);
	lv_obj_set_width(lbl_time, lv_pct(100));
	lv_obj_set_style_text_align(lbl_time, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_time, "000:00");
	lv_obj_align(lbl_time, LV_ALIGN_CENTER, 0, -20);

	/* Status message */
	lbl_status = lv_label_create(scr);
	lv_obj_set_style_text_color(lbl_status, lv_color_hex(0xaaaaaa), 0);
	lv_obj_set_width(lbl_status, lv_pct(100));
	lv_obj_set_style_text_align(lbl_status, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_status, "");
	lv_obj_align(lbl_status, LV_ALIGN_CENTER, 0, 30);

	/* Button row */
	lv_obj_t *btn_row = lv_obj_create(scr);
	lv_obj_set_size(btn_row, 304, 50);
	lv_obj_set_style_bg_opa(btn_row, LV_OPA_TRANSP, 0);
	lv_obj_set_style_border_width(btn_row, 0, 0);
	lv_obj_set_style_pad_all(btn_row, 0, 0);
	lv_obj_set_style_pad_gap(btn_row, 6, 0);
	lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_CENTER,
			      LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_align(btn_row, LV_ALIGN_BOTTOM_MID, 0, -2);

	/* back(44) + inc(56) + dec(56) + play(72) + stop(56) + 4*4gap = 300 */
	btn_back = create_btn(btn_row, LV_SYMBOL_LEFT,
			      lv_palette_main(LV_PALETTE_GREY), 44);
	btn_inc  = create_btn(btn_row, LV_SYMBOL_PLUS,
			      lv_palette_main(LV_PALETTE_GREEN), 56);
	btn_dec  = create_btn(btn_row, LV_SYMBOL_MINUS,
			      lv_palette_main(LV_PALETTE_ORANGE), 56);
	btn_sp   = create_btn(btn_row, LV_SYMBOL_PLAY,
			      lv_palette_main(LV_PALETTE_BLUE), 72);
	btn_abrt = create_btn(btn_row, LV_SYMBOL_STOP,
			      lv_palette_main(LV_PALETTE_RED), 56);
}

void protimer_view_set_time(uint32_t seconds)
{
	uint32_t min = seconds / 60;
	uint32_t sec = seconds % 60;

	lv_label_set_text_fmt(lbl_time, "%03u:%02u", min, sec);
}

void protimer_view_set_status(const char *msg)
{
	lv_label_set_text(lbl_status, msg);
}

void protimer_view_set_state(const char *msg)
{
	lv_label_set_text(lbl_state, msg);
}
