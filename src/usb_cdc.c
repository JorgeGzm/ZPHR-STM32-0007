#include "usb_cdc.h"
#include "display_manager.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/bindesc.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <sample_usbd.h>
#include <math.h>

#define STACKSIZE 2048
#define PRIORITY  7

static int enable_usb_device_next(void);
static void usb_thread(void);

static struct usbd_context *sample_usbd;

static int enable_usb_device_next(void)
{
    int err;

    sample_usbd = sample_usbd_init_device(NULL);
    if (sample_usbd == NULL)
    {
        return -ENODEV;
    }

    err = usbd_enable(sample_usbd);
    if (err)
    {
        return err;
    }

    return 0;
}

static void usb_thread(void)
{
    char msg[200] = {0};
    uint8_t decimal_places = 0;
    double load_local = 0;
    uint32_t dtr = 0U;
    int mac_id = 0;

    k_msleep(2000);

    const struct device *const dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);

    enable_usb_device_next();

    while (true)
    {
        uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
        if (dtr)
        {
            break;
        }
        else
        {
            k_msleep(100);
        }
    }

    while (true)
    {
        mac_id++;
        snprintf(msg, sizeof(msg), "%x=%.0f:%d\r\n", mac_id, load_local, decimal_places);
        for (size_t i = 0; i < strlen(msg); i++)
        {
            uart_poll_out(dev, msg[i]);
        }

        k_msleep(5000);
    }
}

#ifndef CONFIG_HFN_ENABLE_TEST_FLASH_MEM
K_THREAD_DEFINE(usblog1_id, STACKSIZE, usb_thread, NULL, NULL, NULL,
               PRIORITY, 0, 0);
#endif