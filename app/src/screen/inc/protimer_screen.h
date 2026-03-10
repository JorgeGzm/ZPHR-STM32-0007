#ifndef PROTIMER_SCREEN_H
#define PROTIMER_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*protimer_back_cb_t)(void);

void protimer_screen_create(protimer_back_cb_t back_cb);
void protimer_screen_destroy(void);
void protimer_process(void);

#ifdef __cplusplus
}
#endif

#endif /* PROTIMER_SCREEN_H */
