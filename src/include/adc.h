#ifndef ADC_H
#define ADC_H

/* C++ detection */
#ifdef __cplusplus
extern "C"
{
#endif

void adc_init(void);
// void adc_read_value(void);
double adc_read_val_percent_bbat(void);
double adc_read_val_percent_pilbat(void);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif