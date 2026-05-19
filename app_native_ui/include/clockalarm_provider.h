/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file clockalarm_provider.h
 * @brief Clock/Alarm provider interface — decouples UI from app logic.
 */

#ifndef APP_NATIVE_UI_CLOCKALARM_PROVIDER_H
#define APP_NATIVE_UI_CLOCKALARM_PROVIDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/* UI-side event IDs. Values must match enum clockalarm_event in clockalarm_model.h. */
enum clockalarm_ui_evt {
	CA_UI_EVT_NONE = 0,
	CA_UI_EVT_TICK = 1,
	CA_UI_EVT_SET  = 2,
	CA_UI_EVT_OK   = 3,
	CA_UI_EVT_INC  = 4,
	CA_UI_EVT_DEC  = 5,
	CA_UI_EVT_ABRT = 6,
};

/** Callbacks the UI provides to the app so the model can update the display. */
struct clockalarm_ui_cbs {
	void (*on_time)(uint8_t h, uint8_t m, uint8_t s);
	void (*on_alarm)(uint8_t h, uint8_t m, uint8_t s, bool enabled);
	void (*on_state)(const char *msg);
	void (*on_status)(const char *msg);
};

/** Operations the app provides to the UI for model control. */
typedef struct {
	void (*on_screen_enter)(const struct clockalarm_ui_cbs *cbs);
	void (*on_screen_exit)(void);
	void (*send_event)(int evt);
} clockalarm_provider_ops_t;

#ifdef __cplusplus
}
#endif

#endif /* APP_NATIVE_UI_CLOCKALARM_PROVIDER_H */
