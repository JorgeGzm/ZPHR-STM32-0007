#include <zephyr/kernel.h>
#include <lvgl.h>
#include "basic_events.h"
#include "display_manager.h"

void my_event_cb_ex1(lv_event_t *event)
{
	static int counter = 0;
	lv_event_code_t code = lv_event_get_code(event);
	
	switch (code)
	{
	case LV_EVENT_PRESSED:
		printk("code LV_EVENT_PRESSED\n");
		break;
	case LV_EVENT_RELEASED:
		printk("code LV_EVENT_RELEASED\n");
		break;
	case LV_EVENT_CLICKED:
		printk("code LV_EVENT_CLICKED\n");
		lv_obj_t *obj = lv_event_get_target_obj(event);
		counter = (counter + 20) % 40;
		lv_obj_set_x(obj, lv_obj_get_x(obj)+counter);
		break;
	case LV_EVENT_LONG_PRESSED:
		printk("code LV_EVENT_LONG_PRESSED\n");
		break;
	case LV_EVENT_LONG_PRESSED_REPEAT:
		printk("code LV_EVENT_LONG_PRESSED\n");
		break;
	default:
		printk("code: %d\n", code);
		break;
	}
}

/* 
 *Basic event 
 */
void basic_widgets_events_ex1(void)
{
	lv_log("log lvgl enabled\n");
	lv_obj_set_style_bg_color(lv_screen_active(), lv_color_black(), 0);
	lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

	lv_obj_t *obj = lv_obj_create(lv_screen_active());
	lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
	lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
	lv_obj_add_event_cb(obj, my_event_cb_ex1, LV_EVENT_ALL, NULL);
}

//#########################################################################
//#########################################################################

void button_clicked_cb(lv_event_t *event)
{
	lv_event_code_t code = lv_event_get_code(event);
	if(code == LV_EVENT_CLICKED)
	{
		lv_obj_t  *led = (lv_obj_t*)lv_event_get_user_data(event);
		lv_led_toggle(led);
		printk("Clicked!\n");
	}
	if(code == LV_EVENT_VALUE_CHANGED)
	{
		lv_obj_t  *btn2 = (lv_obj_t*)lv_event_get_target(event);
		lv_state_t state = lv_obj_get_state(btn2);
		
		if (state == (LV_STATE_CHECKED | LV_STATE_FOCUSED))
        {
            printk("CHECKED!\n");
        }
        else
        {
            printk("UNCHECKED!\n");
        }
	}
}

/* 
 * Basic button event 
 */
void basic_widgets_events_ex2(void)
{
	lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), 0);
	lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

    lv_obj_t* led = lv_led_create(lv_screen_active());
    lv_led_off(led);
    lv_obj_center(led);

    lv_obj_t* btn = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn, button_clicked_cb, LV_EVENT_ALL, led);

    lv_obj_t* btn_lbl = lv_label_create(btn);
    lv_label_set_text(btn_lbl, "click me");

    lv_obj_t* btn2 = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn2, button_clicked_cb, LV_EVENT_ALL, NULL);
	lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_y(btn2, 50);

    lv_obj_t* btn_lbl2 = lv_label_create(btn2);
    lv_label_set_text(btn_lbl2, "click me");
}

//#########################################################################
//#########################################################################

void on_checked(lv_event_t* e)
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED)
    {
        lv_obj_t* chk = lv_event_get_target_obj(e);
        lv_state_t state = lv_obj_get_state(chk);
        if (state & LV_STATE_CHECKED)
        {
            printk("checked\n");
        }
        else
        {
            printk("unchecked\n");
        }
    }
}

/* 
 * Basic checkbokx event 
 */
void basic_widgets_events_ex3(void)
{
	lv_obj_set_style_bg_color(lv_screen_active(), lv_color_white(), 0);
	lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

	lv_obj_t* chk = lv_checkbox_create(lv_screen_active());
    lv_checkbox_set_text(chk, "check me");
    lv_obj_add_state(chk, LV_STATE_CHECKED);
    lv_obj_add_event_cb(chk, on_checked, LV_EVENT_ALL, NULL);
}