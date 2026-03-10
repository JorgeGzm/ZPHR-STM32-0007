#ifndef CLOCKALARM_VIEW_H
#define CLOCKALARM_VIEW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

enum clockalarm_btn_id {
	CA_BTN_SET,
	CA_BTN_OK,
	CA_BTN_INC,
	CA_BTN_DEC,
	CA_BTN_BACK,
};

typedef void (*clockalarm_view_btn_cb_t)(enum clockalarm_btn_id id);

void clockalarm_view_create(clockalarm_view_btn_cb_t cb);
void clockalarm_view_set_time(uint8_t h, uint8_t m, uint8_t s);
void clockalarm_view_set_alarm(uint8_t h, uint8_t m, uint8_t s, bool enabled);
void clockalarm_view_set_state(const char *msg);
void clockalarm_view_set_status(const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* CLOCKALARM_VIEW_H */
