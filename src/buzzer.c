#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/sys/printk.h>
#include "buzzer.h"

#define STACKSIZE 2048
#define PRIORITY 7
#define BUZZER DT_ALIAS(pwm_led1)

struct buzzer_work_data {
    struct k_work work;
    enum buzzer_type type;
};

static struct buzzer_work_data buzzer_work;
static const struct pwm_dt_spec pwm_buzzer = PWM_DT_SPEC_GET(BUZZER);

static void buzzer_work_handler(struct k_work *work)
{
    struct buzzer_work_data *work_data = CONTAINER_OF(work, struct buzzer_work_data, work);
    int ret = -EINVAL;

    switch (work_data->type)
    {
    case BUZZER_OFF:
        ret = pwm_set_pulse_dt(&pwm_buzzer, 0);
        break;

    case BUZZER_SOUND_1:
        ret = pwm_set_pulse_dt(&pwm_buzzer, (pwm_buzzer.period) / 2);
        k_msleep(100);
        ret |= pwm_set_pulse_dt(&pwm_buzzer, 0);
        break;

    default:
        ret = -EINVAL;
        break;
    }

    if (ret != 0) {
        printk("Buzzer PWM error: %d\n", ret);
    }
}

int buzzer_init(void)
{
    int ret;

    ret = device_is_ready(pwm_buzzer.dev);
    if (!ret)
    {
        printk("Error: PWM device %s is not ready\n", pwm_buzzer.dev->name);
        return -ENODEV;
    }

    k_work_init(&buzzer_work.work, buzzer_work_handler);

    ret = buzzer_play_sound(BUZZER_OFF);
    return ret;
}

int buzzer_play_sound(enum buzzer_type type)
{
    buzzer_work.type = type;
    return k_work_submit(&buzzer_work.work);
}