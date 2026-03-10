#include "protimer_screen.h"
#include "protimer_view.h"
#include "protimer_model.h"

static protimer_back_cb_t s_back_cb;

static void on_btn_pressed(enum protimer_btn_id id)
{
	if (id == BTN_BACK) {
		if (s_back_cb) {
			s_back_cb();
		}
		return;
	}

	static const enum protimer_event map[] = {
		[BTN_INC]         = EVT_INC_TIME,
		[BTN_DEC]         = EVT_DEC_TIME,
		[BTN_START_PAUSE] = EVT_START_PAUSE,
		[BTN_ABORT]       = EVT_ABRT,
	};

	protimer_model_send_event(map[id]);
}

static const struct protimer_model_cb model_cb = {
	.on_time   = protimer_view_set_time,
	.on_status = protimer_view_set_status,
	.on_state  = protimer_view_set_state,
};

void protimer_screen_create(protimer_back_cb_t back_cb)
{
	s_back_cb = back_cb;
	protimer_view_create(on_btn_pressed);
	protimer_model_init(&model_cb);
}

void protimer_screen_destroy(void)
{
	protimer_model_stop();
}

void protimer_process(void)
{
	protimer_model_process();
}
