#ifndef PROTIMER_MODEL_H
#define PROTIMER_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum protimer_event {
	EVT_NONE,
	EVT_INC_TIME,
	EVT_DEC_TIME,
	EVT_TIME_TICK,
	EVT_START_PAUSE,
	EVT_ABRT,
};

struct protimer_model_cb {
	void (*on_time)(uint32_t seconds);
	void (*on_status)(const char *msg);
	void (*on_state)(const char *msg);
};

void protimer_model_init(const struct protimer_model_cb *cb);
void protimer_model_stop(void);
void protimer_model_send_event(enum protimer_event evt);
void protimer_model_process(void);

#ifdef __cplusplus
}
#endif

#endif /* PROTIMER_MODEL_H */
