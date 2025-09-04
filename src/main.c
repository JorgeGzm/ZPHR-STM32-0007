#include "include/main.h"
#include "include/adc.h"
#include "main_screen.h"
#include "buzzer.h"
#include "display_manager.h"

#include "screen/main_screen_ex1.h"
#include "screen/main_screen.h"

#include <zephyr/bindesc.h>
#include <zephyr/input/input.h>

#define STACKSIZE 2048
#define PRIORITY  7

static void button_input_callback(struct input_event *evt, void *user_data);
static void adc_thread(void);

INPUT_CALLBACK_DEFINE(NULL, button_input_callback, 0);

static void button_input_callback(struct input_event *evt, void *user_data)
{
    if (evt->value == 1) // Button pressed
    {
        #if defined(CONFIG_HFN_DEBUG_DEBOUNCE)
        printk("input event: dev=%-16s type=%2x code=%3d value=%d\n",
        evt->dev ? evt->dev->name : "NULL", evt->type, evt->code, evt->value);
        #endif

        switch (evt->code) {
        case INPUT_KEY_DOWN: // Function button
            break;
        case INPUT_KEY_UP: // Backlight button
            break;
        case INPUT_KEY_LEFT: // Tare button
            break;
        case INPUT_KEY_R: // Tare button long press
            break;
        case INPUT_KEY_ENTER: // On/Off button
            break;
        case INPUT_KEY_RIGHT: // Gross/Net button
            break;
        case INPUT_KEY_RIGHTSHIFT: // Gross/Net button long press
            break;
        default:
            printk("Unknown button code: %d\n", evt->code);
            break;
        }
    }
}

static void adc_thread(void)
{
    double val_percent;
    adc_init();
    
    while (1) {
        k_msleep(1000);
        
        val_percent = adc_read_val_percent_bbat();
        val_percent = adc_read_val_percent_pilbat();
    }
}

int main(void)
{
    buzzer_init();
    printk("init version - %s\r\n", BINDESC_GET_STR(app_version_string));

    display_init_screens();
    
    #if 0
    /* basic screen demo */
    main_screen_create();
    main_screen_show();
    #endif

    screen_ex1();
    while (true)
    {
        display_update();
        k_msleep(50);
    }

    return 0;
}

K_THREAD_DEFINE(adc3_id, STACKSIZE, adc_thread, NULL, NULL, NULL,
                PRIORITY, 0, 0);
