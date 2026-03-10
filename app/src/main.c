#include "include/main.h"
#include "adc_read.h"
#include "display_manager.h"

#include "menu_screen.h"
#include "protimer_screen.h"
#include "clockalarm_screen.h"

#include <zephyr/bindesc.h>
#include <zephyr/input/input.h>
#include <zephyr/fs/littlefs.h>
#include <zephyr/storage/flash_map.h>
#include <lvgl.h>

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

enum active_screen {
    SCREEN_MENU,
    SCREEN_PROTIMER,
    SCREEN_CLOCKALARM,
};

static enum active_screen current_screen;

static void show_menu(void);

static void on_back(void)
{
    switch (current_screen) {
    case SCREEN_PROTIMER:
        protimer_screen_destroy();
        break;
    case SCREEN_CLOCKALARM:
        clockalarm_screen_destroy();
        break;
    default:
        break;
    }

    lv_obj_clean(lv_scr_act());
    show_menu();
}

static void on_menu_select(enum menu_selection sel)
{
    lv_obj_clean(lv_scr_act());

    switch (sel) {
    case MENU_PROTIMER:
        current_screen = SCREEN_PROTIMER;
        protimer_screen_create(on_back);
        break;
    case MENU_CLOCKALARM:
        current_screen = SCREEN_CLOCKALARM;
        clockalarm_screen_create(on_back);
        break;
    }
}

static void show_menu(void)
{
    current_screen = SCREEN_MENU;
    menu_screen_create(on_menu_select);
}

int main(void)
{
    printk("init version - %s\r\n", BINDESC_GET_STR(app_version_string));

    xfs_init(&lfs_storage_mnt);

    display_init_screens();

    show_menu();

    while (true) {
        switch (current_screen) {
        case SCREEN_PROTIMER:
            protimer_process();
            break;
        case SCREEN_CLOCKALARM:
            clockalarm_process();
            break;
        default:
            break;
        }

        display_update();
        k_msleep(50);
    }

    return 0;
}
