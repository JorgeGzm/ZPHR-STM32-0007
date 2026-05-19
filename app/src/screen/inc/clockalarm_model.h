/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file clockalarm_model.h
 * @brief Clock/Alarm state machine model — events and callback interface.
 */

#ifndef APP_CLOCKALARM_MODEL_H
#define APP_CLOCKALARM_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

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

/**
 * @brief Initialize the Clock/Alarm model with initial time and alarm values.
 *
 * @param cb Pointer to callback struct (must remain valid for model lifetime).
 */
void clockalarm_model_init(const struct clockalarm_model_cb *cb);

/**
 * @brief Stop the tick timer and flush the event queue.
 */
void clockalarm_model_stop(void);

/**
 * @brief Enqueue an event for processing.
 *
 * @param evt Event to enqueue.
 */
void clockalarm_model_send_event(enum clockalarm_event evt);

/**
 * @brief Drain the event queue and run the state machine.
 */
void clockalarm_model_process(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_CLOCKALARM_MODEL_H */
