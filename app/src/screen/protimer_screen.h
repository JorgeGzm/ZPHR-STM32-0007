#ifndef PROTIMER_SCREEN_H
#define PROTIMER_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Create the protimer screen and init the state machine
 */
void protimer_screen_create(void);

/**
 * @brief Process pending SMF events (call from main loop)
 */
void protimer_process(void);

#ifdef __cplusplus
}
#endif

#endif /* PROTIMER_SCREEN_H */
