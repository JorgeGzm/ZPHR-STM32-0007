/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file setup.c
 * @brief Application initialization and main event loop.
 */

#include "setup/setup.h"

#include <zephyr/bindesc.h>
#include <zephyr/fs/fs.h>
#include <zephyr/kernel.h>

#include <gzm/fs_mgr.h>

#include "adc_read.h"
#include "bsp/bsp.h"
#include "display_manager.h"
#include "ui_app.h"

#include "protimer_model.h"
#include "clockalarm_model.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(setup, CONFIG_LOG_DEFAULT_LEVEL);

/* Forward declarations of static helpers. */
static void setup_sanity_init(void);
static int  setup_init_middleware(void);
static int  setup_init_database(void);
static int  setup_init_tasks(void);
static int  setup_fatal_error(int error);

static void setup_sanity_init(void)
{
	LOG_INF("ZPHR-STM32-0007 v%s", BINDESC_GET_STR(app_version_string));
	LOG_INF("Kernel: %s", BINDESC_GET_STR(kernel_version_string));
	LOG_INF("Build:  %s", BINDESC_GET_STR(build_date_time_string));
}

static int setup_init_middleware(void)
{
	int ret;

	ret = bsp_init();
	if (ret) {
		LOG_ERR("bsp_init: %d", ret);
		return ret;
	}

	ret = fs_mgr_init(bsp_get_lfs_mount());
	if (ret) {
		LOG_WRN("fs_mgr_init: %d (non-fatal, continuing without fs)", ret);
	}

	adc_read_init();
	display_init_screens();

	return 0;
}

static int setup_init_database(void)
{
	struct fs_dirent entry;
	int ret;

	ret = fs_stat("/lfs1", &entry);
	if (ret) {
		LOG_WRN("db: filesystem not accessible (%d), skipping", ret);
		return 0;
	}

	LOG_DBG("db: filesystem ready, type=%u", entry.type);

	return 0;
}

static int setup_init_tasks(void)
{
	ui_app_init();

	return 0;
}

static int setup_fatal_error(int error)
{
	LOG_ERR("fatal init error: %d — system halted", error);
	k_panic();
	return error;
}

void setup_init(void)
{
	int ret;

	setup_sanity_init();

	ret = setup_init_middleware();
	if (ret) {
		setup_fatal_error(ret);
	}

	ret = setup_init_database();
	if (ret) {
		setup_fatal_error(ret);
	}

	ret = setup_init_tasks();
	if (ret) {
		setup_fatal_error(ret);
	}

	while (true) {
		protimer_model_process();
		clockalarm_model_process();
		display_update();
		k_msleep(50);
	}
}
