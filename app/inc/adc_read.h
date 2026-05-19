/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file adc_read.h
 * @brief Battery voltage ADC reading with exponential filtering and SoC conversion.
 */

#ifndef APP_ADC_READ_H
#define APP_ADC_READ_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize ADC channels for battery voltage measurement.
 */
void adc_read_init(void);

/**
 * @brief Read backup battery state of charge.
 *
 * @return SoC percentage (0.0 – 100.0), or 0.0 on read error.
 */
double adc_read_val_percent_bbat(void);

/**
 * @brief Read pillow battery state of charge.
 *
 * @return SoC percentage (0.0 – 100.0), or 0.0 on read error.
 */
double adc_read_val_percent_pilbat(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_ADC_READ_H */
