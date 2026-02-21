/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include "adc_read.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <string.h>

#define ADC_RESOLUTION       12
#define ADC_GAIN             ADC_GAIN_1
#define ADC_REFERENCE        ADC_REF_EXTERNAL0
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 10)

#define ADC_MAX_VALUE         (4095)
#define ADC_VREF              (3.300)
#define ADC_ALPHA_FILTER_COEF (0.1)
#define ADC_SAMPLES_COUNT     10

struct adc_measure_filter {
    bool init;
    double voltage;
};

static float adc_voltage_to_soc_lipo(float voltage);
static float adc_voltage_to_soc_pil(float voltage);

static uint16_t adc_buffer[1];

static struct adc_channel_cfg adc_channel_vad_bat = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id = 0,
    .differential = 0,
};

static struct adc_channel_cfg adc_channel_vpil = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id = 1,
    .differential = 0,
};

static struct adc_sequence sequence = {
    .buffer = adc_buffer,
    .buffer_size = sizeof(adc_buffer),
    .resolution = ADC_RESOLUTION,
};

static struct adc_measure_filter g_bbat = {0};
static struct adc_measure_filter g_pilbat = {0};
static const double R1 = 330000.0;
static const double R2 = 1000000.0;
static double voltage_divider_factor = (R1 + R2) / R2;
static const struct device *adc_dev = DEVICE_DT_GET(DT_NODELABEL(adc1));

static float adc_voltage_to_soc_lipo(float voltage)
{
    const float voltages[] = {3.0, 3.2, 3.5, 3.7, 3.85, 4.0, 4.2};
    const float socs[] = {0.0, 10.0, 25.0, 50.0, 75.0, 85.0, 100.0};
    const int size = sizeof(voltages) / sizeof(voltages[0]);

    if (voltage <= voltages[0]) return socs[0];
    if (voltage >= voltages[size - 1]) return socs[size - 1];

    for (int i = 0; i < size - 1; i++) {
        if (voltage >= voltages[i] && voltage <= voltages[i + 1]) {
            float soc = socs[i] + (voltage - voltages[i]) *
                        (socs[i + 1] - socs[i]) / (voltages[i + 1] - voltages[i]);
            return soc;
        }
    }

    return 0.0;
}

static float adc_voltage_to_soc_pil(float voltage)
{
    const float voltages[] = {2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2};
    const float socs[] = {0.0, 10.0, 25.0, 50.0, 75.0, 90.0, 100.0};
    const int size = sizeof(voltages) / sizeof(voltages[0]);

    if (voltage <= voltages[0]) return socs[0];
    if (voltage >= voltages[size - 1]) return socs[size - 1];

    for (int i = 0; i < size - 1; i++) {
        if (voltage >= voltages[i] && voltage <= voltages[i + 1]) {
            float soc = socs[i] + (voltage - voltages[i]) *
                        (socs[i + 1] - socs[i]) / (voltages[i + 1] - voltages[i]);
            return soc;
        }
    }

    return 0.0;
}

static int adc_read_channel_average(uint8_t channel_id, uint32_t *adc_value)
{
    int ret;
    int count_reads = 0;
    uint32_t sum = 0;

    sequence.channels = BIT(channel_id);

    for (int i = 0; i < ADC_SAMPLES_COUNT; i++) {
        ret = adc_read(adc_dev, &sequence);
        if (ret != 0) {
            return ret;
        }

        sum += adc_buffer[0];
        count_reads++;

        k_msleep(5);
    }

    *adc_value = (sum / count_reads);
    return 0;
}

void adc_read_init(void)
{
    if (!device_is_ready(adc_dev)) {
        printk("ADC device not ready\n");
        return;
    }

    memset(&g_bbat, 0, sizeof(struct adc_measure_filter));
    memset(&g_pilbat, 0, sizeof(struct adc_measure_filter));

    adc_channel_setup(adc_dev, &adc_channel_vad_bat);
    adc_channel_setup(adc_dev, &adc_channel_vpil);
}

double adc_read_val_percent_bbat(void)
{
    int ret;
    uint32_t adc_value;
    double val_volt = 0;
    double val_percent = 0;

    ret = adc_read_channel_average(0, &adc_value);
    if (ret) {
        printk("Failed to read BBAT\n");
        return 0;
    }

    val_volt = (((double)adc_value) / ADC_MAX_VALUE) * ADC_VREF * voltage_divider_factor;
    if (!g_bbat.init) {
        g_bbat.init = true;
        g_bbat.voltage = val_volt;
    } else {
        g_bbat.voltage = ADC_ALPHA_FILTER_COEF * val_volt +
                         (1.0 - ADC_ALPHA_FILTER_COEF) * g_bbat.voltage;
    }

    val_percent = adc_voltage_to_soc_lipo(g_bbat.voltage);
    return val_percent;
}

double adc_read_val_percent_pilbat(void)
{
    int ret;
    uint32_t adc_value;
    double val_volt = 0;
    double val_percent = 0;

    ret = adc_read_channel_average(1, &adc_value);
    if (ret) {
        printk("Failed to read PilBAT\n");
        return 0;
    }

    val_volt = (((double)adc_value) / ADC_MAX_VALUE) * ADC_VREF;
    if (!g_pilbat.init) {
        g_pilbat.init = true;
        g_pilbat.voltage = val_volt;
    } else {
        g_pilbat.voltage = ADC_ALPHA_FILTER_COEF * val_volt +
                           (1.0 - ADC_ALPHA_FILTER_COEF) * g_pilbat.voltage;
    }

    val_percent = adc_voltage_to_soc_pil(g_pilbat.voltage);
    return val_percent;
}
