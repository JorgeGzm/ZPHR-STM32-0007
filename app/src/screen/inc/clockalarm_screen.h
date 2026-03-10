#ifndef CLOCKALARM_SCREEN_H
#define CLOCKALARM_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*clockalarm_back_cb_t)(void);

void clockalarm_screen_create(clockalarm_back_cb_t back_cb);
void clockalarm_screen_destroy(void);
void clockalarm_process(void);

#ifdef __cplusplus
}
#endif

#endif /* CLOCKALARM_SCREEN_H */
