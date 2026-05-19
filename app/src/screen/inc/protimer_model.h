/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file protimer_model.h
 * @brief Pro Timer state machine model — events and callback interface.
 */

#ifndef APP_PROTIMER_MODEL_H
#define APP_PROTIMER_MODEL_H

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

/**
 * @brief Initialize the Pro Timer model and start the tick timer.
 *
 * @param cb Pointer to callback struct (must remain valid for model lifetime).
 */
void protimer_model_init(const struct protimer_model_cb *cb);

/**
 * @brief Stop the tick timer and flush the event queue.
 */
void protimer_model_stop(void);

/**
 * @brief Enqueue an event for processing.
 *
 * @param evt Event to enqueue.
 */
void protimer_model_send_event(enum protimer_event evt);

/**
 * @brief Drain the event queue and run the state machine.
 */
void protimer_model_process(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_PROTIMER_MODEL_H */
