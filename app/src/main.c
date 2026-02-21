#include "include/main.h"
#include "adc_read.h"
#include "display_manager.h"

#include "screen/basic_widgets.h"
#include "screen/basic_events.h"
#include "screen/main_screen.h"

#include <zephyr/bindesc.h>
#include <zephyr/input/input.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>

#include "xfs.h"

#define STACKSIZE 2048
#define PRIORITY  7

FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(lfs_data);

static struct fs_mount_t lfs_storage_mnt = {
    .type = FS_LITTLEFS,
    .fs_data = &lfs_data,
    .storage_dev = (void *)FIXED_PARTITION_ID(storage_partition),
    .mnt_point = "/lfs1",
};

static void adc_thread(void);

static void adc_thread(void)
{
    double val_percent;
    adc_read_init();

    while (1) {
        k_msleep(1000);

        val_percent = adc_read_val_percent_bbat();
        val_percent = adc_read_val_percent_pilbat();
    }
}

int main(void)
{
    printk("init version - %s\r\n", BINDESC_GET_STR(app_version_string));

    xfs_init(&lfs_storage_mnt);

    display_init_screens();

    #if 0
    /* basic screen demo */
    main_screen_create();
    main_screen_show();
    #endif

    #if 1
    // basic_widgets_ex1();
    basic_widgets_events_ex2();

    #endif

    while (true)
    {
        display_update();
        k_msleep(50);
    }

    return 0;
}

K_THREAD_DEFINE(adc3_id, STACKSIZE, adc_thread, NULL, NULL, NULL,
                PRIORITY, 0, 0);
