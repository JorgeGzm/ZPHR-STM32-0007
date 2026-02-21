#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/pwm.h>
#include <lvgl.h>
#include "display_manager.h"
#include <zephyr/dt-bindings/input/input-event-codes.h>

#define BACKLIGHT DT_ALIAS(pwm_led0)

static uint8_t g_backlight_scale = 4;
static const struct pwm_dt_spec pwm_backlight = PWM_DT_SPEC_GET(BACKLIGHT);
static const struct device *display_device = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

/* LVGL Init*/
void display_init_screens(void)
{
    if (!device_is_ready(display_device))
    {
        printk("Display init error!");
    }

    if (!device_is_ready(pwm_backlight.dev))
    {
        printk("Error: PWM device %s is not ready\n",
               pwm_backlight.dev->name);
    }

    lv_init();
    
    display_set_backlight(1);
    printk("LVGL initialized\r\n");

    display_blanking_off(display_device);
}

void display_update(void)
{
    lv_timer_handler();
}

int display_map_button_cb(int btn)
{
    uint32_t key = 0;

    switch (btn)
    {
        case INPUT_KEY_UP: // Backlight
            key = LV_KEY_UP;
            break;
        case INPUT_KEY_LEFT: // Power
            key = LV_KEY_LEFT;
            break;
        case INPUT_KEY_RIGHT:// Tare
            key = LV_KEY_RIGHT;
            break;
        case INPUT_KEY_DOWN: // Function
            key = LV_KEY_DOWN;
            break;
        case INPUT_KEY_ENTER: // Gross/Net
            key = LV_KEY_ENTER;
            break;
        case INPUT_KEY_R:
        default:
            return -EINVAL;
    }

    return 0;
}

int display_set_backlight(uint8_t brightness)
{
    int ret;

    if(brightness > DISPLAY_MAX_BLACKLIGHT_LEVEL)
    {
        return -EINVAL;
    }

    ret = pwm_set_pulse_dt(&pwm_backlight, (brightness * pwm_backlight.period) / DISPLAY_MAX_BLACKLIGHT_LEVEL);
    return ret;
}

void display_inc_blacklight_level(void)
{
    display_set_backlight(g_backlight_scale);

    if (++g_backlight_scale > DISPLAY_MAX_BLACKLIGHT_LEVEL)
    {
        g_backlight_scale = 1;
    }
}