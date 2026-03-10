#include "clockalarm_screen.h"
#include "clockalarm_view.h"
#include "clockalarm_model.h"

static clockalarm_back_cb_t s_back_cb;

static void on_btn_pressed(enum clockalarm_btn_id id)
{
	if (id == CA_BTN_BACK) {
		if (s_back_cb) {
			s_back_cb();
		}
		return;
	}

	static const enum clockalarm_event map[] = {
		[CA_BTN_SET] = CA_EVT_SET,
		[CA_BTN_OK]  = CA_EVT_OK,
		[CA_BTN_INC] = CA_EVT_INC,
		[CA_BTN_DEC] = CA_EVT_DEC,
	};

	clockalarm_model_send_event(map[id]);
}

static const struct clockalarm_model_cb model_cb = {
	.on_time   = clockalarm_view_set_time,
	.on_alarm  = clockalarm_view_set_alarm,
	.on_state  = clockalarm_view_set_state,
	.on_status = clockalarm_view_set_status,
};

void clockalarm_screen_create(clockalarm_back_cb_t back_cb)
{
	s_back_cb = back_cb;
	clockalarm_view_create(on_btn_pressed);
	clockalarm_model_init(&model_cb);
}

void clockalarm_screen_destroy(void)
{
	clockalarm_model_stop();
}

void clockalarm_process(void)
{
	clockalarm_model_process();
}
