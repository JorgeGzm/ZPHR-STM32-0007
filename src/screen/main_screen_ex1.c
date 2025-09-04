#include <zephyr/kernel.h>

#include "main_screen_ex1.h"
#include <lvgl.h>

void screen_ex1(void)
{
    /* Get the active screen */
    lv_obj_t *screen = lv_obj_create(NULL);
    
    /* Create a label on the screen */
    lv_obj_t *label = lv_label_create(screen);
    
    /* Set the text */
    lv_label_set_text(label, "Hello World!");

    // Load screen
    lv_scr_load(screen);
}