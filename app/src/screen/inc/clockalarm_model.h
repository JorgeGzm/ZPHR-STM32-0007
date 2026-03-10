#ifndef CLOCKALARM_MODEL_H
#define CLOCKALARM_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

enum clockalarm_event {
	CA_EVT_NONE,
	CA_EVT_TICK,
	CA_EVT_SET,
	CA_EVT_OK,
	CA_EVT_INC,
	CA_EVT_DEC,
	CA_EVT_ABRT,
};

struct clockalarm_model_cb {
	void (*on_time)(uint8_t h, uint8_t m, uint8_t s);
	void (*on_alarm)(uint8_t h, uint8_t m, uint8_t s, bool enabled);
	void (*on_state)(const char *msg);
	void (*on_status)(const char *msg);
};

void clockalarm_model_init(const struct clockalarm_model_cb *cb);
void clockalarm_model_stop(void);
void clockalarm_model_send_event(enum clockalarm_event evt);
void clockalarm_model_process(void);

#ifdef __cplusplus
}
#endif

#endif /* CLOCKALARM_MODEL_H */
