/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ADC_READ_H
#define ADC_READ_H

#ifdef __cplusplus
extern "C" {
#endif

void adc_read_init(void);
double adc_read_val_percent_bbat(void);
double adc_read_val_percent_pilbat(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_READ_H */
