#include <zephyr/kernel.h>
#include <zephyr/smf.h>
#include <lvgl.h>

#include "protimer_screen.h"
#include "display_manager.h"

/* -------------------------------------------------------------------------- */
/*  Events                                                                    */
/* -------------------------------------------------------------------------- */
enum protimer_event {
	EVT_NONE,
	EVT_INC_TIME,
	EVT_DEC_TIME,
	EVT_TIME_TICK,
	EVT_START_PAUSE,
	EVT_ABRT,
};

/* -------------------------------------------------------------------------- */
/*  State list                                                                */
/* -------------------------------------------------------------------------- */
enum protimer_state_id {
	STATE_IDLE,
	STATE_TIME_SET,
	STATE_COUNTDOWN,
	STATE_PAUSE,
	STATE_STAT,
	STATE_COUNT,
};

/* Forward declarations */
static const struct smf_state protimer_states[];

/* -------------------------------------------------------------------------- */
/*  State machine object                                                      */
/* -------------------------------------------------------------------------- */
struct protimer_obj {
	struct smf_ctx ctx;          /* must be first */
	enum protimer_event event;
	uint32_t curr_time;          /* seconds */
	uint32_t elapsed_time;       /* seconds */
	uint32_t pro_time;           /* accumulated productive seconds */
	uint8_t  tick_count;         /* sub-tick counter (0-9 = 1 second) */
};

static struct protimer_obj s_obj;

/* -------------------------------------------------------------------------- */
/*  Message queue for events                                                  */
/* -------------------------------------------------------------------------- */
K_MSGQ_DEFINE(protimer_msgq, sizeof(enum protimer_event), 16, 4);

/* -------------------------------------------------------------------------- */
/*  Timer tick (100 ms)                                                       */
/* -------------------------------------------------------------------------- */
static void tick_timer_handler(struct k_timer *timer)
{
	enum protimer_event evt = EVT_TIME_TICK;
	k_msgq_put(&protimer_msgq, &evt, K_NO_WAIT);
}

K_TIMER_DEFINE(tick_timer, tick_timer_handler, NULL);

/* -------------------------------------------------------------------------- */
/*  LVGL widgets                                                              */
/* -------------------------------------------------------------------------- */
static lv_obj_t *lbl_time;
static lv_obj_t *lbl_status;
static lv_obj_t *lbl_state;
static lv_obj_t *btn_inc;
static lv_obj_t *btn_dec;
static lv_obj_t *btn_sp;
static lv_obj_t *btn_abrt;

/* -------------------------------------------------------------------------- */
/*  LVGL helpers                                                              */
/* -------------------------------------------------------------------------- */
static void display_time(uint32_t seconds)
{
	uint32_t min = seconds / 60;
	uint32_t sec = seconds % 60;

	lv_label_set_text_fmt(lbl_time, "%03u:%02u", min, sec);
}

static void set_status(const char *msg)
{
	lv_label_set_text(lbl_status, msg);
}

static void set_state_label(const char *msg)
{
	lv_label_set_text(lbl_state, msg);
}

/* -------------------------------------------------------------------------- */
/*  LVGL button callbacks                                                     */
/* -------------------------------------------------------------------------- */
static void btn_event_cb(lv_event_t *e)
{
	lv_obj_t *target = lv_event_get_target(e);
	enum protimer_event evt;

	if (target == btn_inc) {
		evt = EVT_INC_TIME;
	} else if (target == btn_dec) {
		evt = EVT_DEC_TIME;
	} else if (target == btn_sp) {
		evt = EVT_START_PAUSE;
	} else if (target == btn_abrt) {
		evt = EVT_ABRT;
	} else {
		return;
	}

	k_msgq_put(&protimer_msgq, &evt, K_NO_WAIT);
}

/* -------------------------------------------------------------------------- */
/*  State handlers - IDLE                                                     */
/* -------------------------------------------------------------------------- */
static void idle_entry(void *obj)
{
	struct protimer_obj *o = obj;

	o->curr_time = 0;
	o->elapsed_time = 0;
	o->tick_count = 0;

	set_state_label("IDLE");
	display_time(0);
	set_status("Set time");
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

/* -------------------------------------------------------------------------- */
/*  State handlers - TIME_SET                                                 */
/* -------------------------------------------------------------------------- */
static void time_set_entry(void *obj)
{
	struct protimer_obj *o = obj;

	set_state_label("TIME SET");
	display_time(o->curr_time);
	set_status("Adjust time");
}

static enum smf_state_result time_set_run(void *obj)
{
	struct protimer_obj *o = obj;

	switch (o->event) {
	case EVT_INC_TIME:
		o->curr_time += 60;
		display_time(o->curr_time);
		break;
	case EVT_DEC_TIME:
		if (o->curr_time >= 60) {
			o->curr_time -= 60;
			display_time(o->curr_time);
		}
		break;
	case EVT_START_PAUSE:
		if (o->curr_time >= 60) {
			smf_set_state(SMF_CTX(o),
				      &protimer_states[STATE_COUNTDOWN]);
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

/* -------------------------------------------------------------------------- */
/*  State handlers - COUNTDOWN                                                */
/* -------------------------------------------------------------------------- */
static void countdown_entry(void *obj)
{
	struct protimer_obj *o = obj;

	o->tick_count = 0;
	set_state_label("COUNTDOWN");
	display_time(o->curr_time);
	set_status("Running...");
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
				display_time(o->curr_time);
			}
			if (o->curr_time == 0) {
				smf_set_state(SMF_CTX(o),
					      &protimer_states[STATE_IDLE]);
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

	o->pro_time += o->elapsed_time;
	o->elapsed_time = 0;
}

/* -------------------------------------------------------------------------- */
/*  State handlers - PAUSE                                                    */
/* -------------------------------------------------------------------------- */
static void pause_entry(void *obj)
{
	struct protimer_obj *o = obj;

	set_state_label("PAUSED");
	display_time(o->curr_time);
	set_status("Paused");
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

/* -------------------------------------------------------------------------- */
/*  State handlers - STAT                                                     */
/* -------------------------------------------------------------------------- */
static void stat_entry(void *obj)
{
	struct protimer_obj *o = obj;

	o->tick_count = 0;
	set_state_label("STATISTICS");
	display_time(o->pro_time);
	set_status("Productive time");
}

static enum smf_state_result stat_run(void *obj)
{
	struct protimer_obj *o = obj;

	switch (o->event) {
	case EVT_TIME_TICK:
		o->tick_count++;
		if (o->tick_count >= 30) {
			smf_set_state(SMF_CTX(o),
				      &protimer_states[STATE_IDLE]);
		}
		break;
	default:
		break;
	}

	return SMF_EVENT_HANDLED;
}

/* -------------------------------------------------------------------------- */
/*  State table                                                               */
/* -------------------------------------------------------------------------- */
static const struct smf_state protimer_states[] = {
	[STATE_IDLE]      = SMF_CREATE_STATE(idle_entry, idle_run, NULL,
					     NULL, NULL),
	[STATE_TIME_SET]  = SMF_CREATE_STATE(time_set_entry, time_set_run, NULL,
					     NULL, NULL),
	[STATE_COUNTDOWN] = SMF_CREATE_STATE(countdown_entry, countdown_run,
					     countdown_exit, NULL, NULL),
	[STATE_PAUSE]     = SMF_CREATE_STATE(pause_entry, pause_run, NULL,
					     NULL, NULL),
	[STATE_STAT]      = SMF_CREATE_STATE(stat_entry, stat_run, NULL,
					     NULL, NULL),
};

/* -------------------------------------------------------------------------- */
/*  Helper to create a styled button                                          */
/* -------------------------------------------------------------------------- */
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

/* -------------------------------------------------------------------------- */
/*  Public: create UI and init state machine                                  */
/* -------------------------------------------------------------------------- */
void protimer_screen_create(void)
{
	lv_obj_t *scr = lv_scr_act();

	/* background */
	lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);
	lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
	lv_obj_set_style_pad_all(scr, 8, 0);

	/* State label (top) */
	lbl_state = lv_label_create(scr);
	lv_obj_set_style_text_color(lbl_state, lv_color_hex(0x00d4ff), 0);
	lv_obj_set_width(lbl_state, lv_pct(100));
	lv_obj_set_style_text_align(lbl_state, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_state, "IDLE");
	lv_obj_align(lbl_state, LV_ALIGN_TOP_MID, 0, 4);

	/* Time display (center, large font) */
	lbl_time = lv_label_create(scr);
	lv_obj_set_style_text_color(lbl_time, lv_color_white(), 0);
	lv_obj_set_style_text_font(lbl_time, &lv_font_montserrat_48, 0);
	lv_obj_set_width(lbl_time, lv_pct(100));
	lv_obj_set_style_text_align(lbl_time, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_time, "000:00");
	lv_obj_align(lbl_time, LV_ALIGN_CENTER, 0, -20);

	/* Status message */
	lbl_status = lv_label_create(scr);
	lv_obj_set_style_text_color(lbl_status, lv_color_hex(0xaaaaaa), 0);
	lv_obj_set_width(lbl_status, lv_pct(100));
	lv_obj_set_style_text_align(lbl_status, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(lbl_status, "Set time");
	lv_obj_align(lbl_status, LV_ALIGN_CENTER, 0, 30);

	/* Button row container */
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

	/* Buttons (total: 62+62+90+62 + 3*6 = 294px, fits in 304px) */
	btn_inc  = create_btn(btn_row, LV_SYMBOL_PLUS,
			      lv_palette_main(LV_PALETTE_GREEN), 62);
	btn_dec  = create_btn(btn_row, LV_SYMBOL_MINUS,
			      lv_palette_main(LV_PALETTE_ORANGE), 62);
	btn_sp   = create_btn(btn_row, LV_SYMBOL_PLAY,
			      lv_palette_main(LV_PALETTE_BLUE), 90);
	btn_abrt = create_btn(btn_row, LV_SYMBOL_STOP,
			      lv_palette_main(LV_PALETTE_RED), 62);

	/* Init SMF and start tick timer */
	smf_set_initial(SMF_CTX(&s_obj), &protimer_states[STATE_IDLE]);
	k_timer_start(&tick_timer, K_MSEC(100), K_MSEC(100));
}

/* -------------------------------------------------------------------------- */
/*  Public: process pending events (call from main loop)                      */
/* -------------------------------------------------------------------------- */
void protimer_process(void)
{
	enum protimer_event evt;

	while (k_msgq_get(&protimer_msgq, &evt, K_NO_WAIT) == 0) {
		s_obj.event = evt;
		smf_run_state(SMF_CTX(&s_obj));
	}
}
