#ifndef BUZZER_H
#define BUZZER_H

/* C++ detection */
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

enum buzzer_type
{
    BUZZER_OFF = 0,
    BUZZER_SOUND_1,
    BUZZER_MAX,
};

int buzzer_init(void);
int buzzer_play_sound(enum buzzer_type type);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif