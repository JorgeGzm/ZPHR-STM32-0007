/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file protimer_provider.h
 * @brief Pro Timer provider interface — decouples UI from app logic.
 */

#ifndef APP_NATIVE_UI_PROTIMER_PROVIDER_H
#define APP_NATIVE_UI_PROTIMER_PROVIDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* UI-side event IDs. Values must match enum protimer_event in protimer_model.h. */
enum protimer_ui_evt {
	PT_UI_EVT_NONE        = 0,
	PT_UI_EVT_INC_TIME    = 1,
	PT_UI_EVT_DEC_TIME    = 2,
	PT_UI_EVT_TIME_TICK   = 3,
	PT_UI_EVT_START_PAUSE = 4,
	PT_UI_EVT_ABRT        = 5,
};

/** Callbacks the UI provides to the app so the model can update the display. */
struct protimer_ui_cbs {
	void (*on_time)(uint32_t seconds);
	void (*on_status)(const char *msg);
	void (*on_state)(const char *msg);
};

/** Operations the app provides to the UI for model control. */
typedef struct {
	void (*on_screen_enter)(const struct protimer_ui_cbs *cbs);
	void (*on_screen_exit)(void);
	void (*send_event)(int evt);
} protimer_provider_ops_t;

#ifdef __cplusplus
}
#endif

#endif /* APP_NATIVE_UI_PROTIMER_PROVIDER_H */
