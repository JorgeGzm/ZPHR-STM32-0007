/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file adc_read.c
 * @brief Battery voltage ADC reading with exponential filtering and SoC conversion.
 */

#include "adc_read.h"

#include <string.h>

#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>

#include "bsp/bsp.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(adc_read, CONFIG_LOG_DEFAULT_LEVEL);

#define ADC_RESOLUTION       12
#define ADC_GAIN             ADC_GAIN_1
#define ADC_REFERENCE        ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME_DEFAULT

#define ADC_MAX_VALUE         (4095)
#define ADC_VREF              (3.300)
#define ADC_ALPHA_FILTER_COEF (0.1)
#define ADC_SAMPLES_COUNT     10

/* Voltage divider: R1=330kΩ, R2=1MΩ on the BBAT channel. */
#define ADC_R1 330000.0
#define ADC_R2 1000000.0

struct adc_measure_filter {
	bool   init;
	double voltage;
};

/* Forward declarations of static helpers. */
static float adc_voltage_to_soc_lipo(float voltage);
static float adc_voltage_to_soc_pil(float voltage);
static int   adc_read_channel_average(uint8_t channel_id, uint32_t *adc_value);

static uint16_t adc_buffer[1];

static struct adc_channel_cfg adc_channel_vad_bat = {
	.gain             = ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = 0,
	.differential     = 0,
};

static struct adc_channel_cfg adc_channel_vpil = {
	.gain             = ADC_GAIN,
	.reference        = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id       = 1,
	.differential     = 0,
};

static struct adc_sequence sequence = {
	.buffer      = adc_buffer,
	.buffer_size = sizeof(adc_buffer),
	.resolution  = ADC_RESOLUTION,
};

static struct adc_measure_filter g_bbat;
static struct adc_measure_filter g_pilbat;

static const double voltage_divider_factor = (ADC_R1 + ADC_R2) / ADC_R2;

static float adc_voltage_to_soc_lipo(float voltage)
{
	const float voltages[] = {3.0f, 3.2f, 3.5f, 3.7f, 3.85f, 4.0f, 4.2f};
	const float socs[]     = {0.0f, 10.0f, 25.0f, 50.0f, 75.0f, 85.0f, 100.0f};
	const int   size       = sizeof(voltages) / sizeof(voltages[0]);
	float       soc        = 0.0f;

	if (voltage <= voltages[0]) {
		return socs[0];
	}
	if (voltage >= voltages[size - 1]) {
		return socs[size - 1];
	}

	for (int i = 0; i < size - 1; i++) {
		if (voltage >= voltages[i] && voltage <= voltages[i + 1]) {
			soc = socs[i] + (voltage - voltages[i]) *
				(socs[i + 1] - socs[i]) / (voltages[i + 1] - voltages[i]);
			return soc;
		}
	}

	return 0.0f;
}

static float adc_voltage_to_soc_pil(float voltage)
{
	const float voltages[] = {2.0f, 2.2f, 2.4f, 2.6f, 2.8f, 3.0f, 3.2f};
	const float socs[]     = {0.0f, 10.0f, 25.0f, 50.0f, 75.0f, 90.0f, 100.0f};
	const int   size       = sizeof(voltages) / sizeof(voltages[0]);
	float       soc        = 0.0f;

	if (voltage <= voltages[0]) {
		return socs[0];
	}
	if (voltage >= voltages[size - 1]) {
		return socs[size - 1];
	}

	for (int i = 0; i < size - 1; i++) {
		if (voltage >= voltages[i] && voltage <= voltages[i + 1]) {
			soc = socs[i] + (voltage - voltages[i]) *
				(socs[i + 1] - socs[i]) / (voltages[i + 1] - voltages[i]);
			return soc;
		}
	}

	return 0.0f;
}

static int adc_read_channel_average(uint8_t channel_id, uint32_t *adc_value)
{
	int      ret         = 0;
	int      count_reads = 0;
	uint32_t sum         = 0;

	sequence.channels = BIT(channel_id);

	for (int i = 0; i < ADC_SAMPLES_COUNT; i++) {
		ret = adc_read(bsp_get_adc(), &sequence);
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
	memset(&g_bbat, 0, sizeof(struct adc_measure_filter));
	memset(&g_pilbat, 0, sizeof(struct adc_measure_filter));

	adc_channel_setup(bsp_get_adc(), &adc_channel_vad_bat);
	adc_channel_setup(bsp_get_adc(), &adc_channel_vpil);
}

double adc_read_val_percent_bbat(void)
{
	int      ret       = 0;
	uint32_t adc_value = 0;
	double   val_volt  = 0.0;

	ret = adc_read_channel_average(0, &adc_value);
	if (ret) {
		LOG_ERR("failed to read BBAT: %d", ret);
		return 0.0;
	}

	val_volt = (((double)adc_value) / ADC_MAX_VALUE) * ADC_VREF * voltage_divider_factor;

	if (!g_bbat.init) {
		g_bbat.init    = true;
		g_bbat.voltage = val_volt;
	} else {
		g_bbat.voltage = ADC_ALPHA_FILTER_COEF * val_volt +
				 (1.0 - ADC_ALPHA_FILTER_COEF) * g_bbat.voltage;
	}

	return adc_voltage_to_soc_lipo(g_bbat.voltage);
}

double adc_read_val_percent_pilbat(void)
{
	int      ret       = 0;
	uint32_t adc_value = 0;
	double   val_volt  = 0.0;

	ret = adc_read_channel_average(1, &adc_value);
	if (ret) {
		LOG_ERR("failed to read PilBAT: %d", ret);
		return 0.0;
	}

	val_volt = (((double)adc_value) / ADC_MAX_VALUE) * ADC_VREF;

	if (!g_pilbat.init) {
		g_pilbat.init    = true;
		g_pilbat.voltage = val_volt;
	} else {
		g_pilbat.voltage = ADC_ALPHA_FILTER_COEF * val_volt +
				   (1.0 - ADC_ALPHA_FILTER_COEF) * g_pilbat.voltage;
	}

	return adc_voltage_to_soc_pil(g_pilbat.voltage);
}
