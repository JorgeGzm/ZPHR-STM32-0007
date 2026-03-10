#ifndef PROTIMER_VIEW_H
#define PROTIMER_VIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum protimer_btn_id {
	BTN_INC,
	BTN_DEC,
	BTN_START_PAUSE,
	BTN_ABORT,
	BTN_BACK,
};

typedef void (*protimer_view_btn_cb_t)(enum protimer_btn_id id);

void protimer_view_create(protimer_view_btn_cb_t cb);

void protimer_view_set_time(uint32_t seconds);
void protimer_view_set_status(const char *msg);
void protimer_view_set_state(const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* PROTIMER_VIEW_H */
