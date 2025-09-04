#include <zephyr/kernel.h>

#include "main_screen_ex1.h"
#include <lvgl.h>

void screen_ex1(void)
{
    #if 0

    // Using current screen
    lv_obj_t *splash_screen = lv_scr_act();
	
	/* Set background to black */
	lv_obj_set_style_bg_color(splash_screen, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(splash_screen, LV_OPA_COVER, 0);

	/* Create title label */
	lv_obj_t *title_label = lv_label_create(splash_screen);
	lv_label_set_text(title_label, "Hello World!");
	lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
	lv_obj_align(title_label, LV_ALIGN_CENTER, 0, -10);

#else 

    // Creating new screen
    lv_obj_t *splash_screen = lv_obj_create(NULL);
	
	/* Set background to black */
	lv_obj_set_style_bg_color(splash_screen, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(splash_screen, LV_OPA_COVER, 0);

	/* Create title label */
	lv_obj_t *title_label = lv_label_create(splash_screen);
	lv_label_set_text(title_label, "GZM Systems!");
	lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
	lv_obj_align(title_label, LV_ALIGN_CENTER, 0, -10);

    lv_scr_load(splash_screen);
#endif
    
}