#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <stdint.h>

#define DISPLAY_MAX_BLACKLIGHT_LEVEL 5

// Function prototypes
void display_init_screens(void);
void display_update(void);
int display_map_button_cb(int btn);
int display_set_backlight(uint8_t brightness);
void display_inc_blacklight_level(void);

#endif // DISPLAY_MANAGER_H