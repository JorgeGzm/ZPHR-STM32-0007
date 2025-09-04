#ifndef MAIN_H
#define MAIN_H

/* C++ detection */
#ifdef __cplusplus
extern "C"
{
#endif

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void print_screen_clear(void);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif