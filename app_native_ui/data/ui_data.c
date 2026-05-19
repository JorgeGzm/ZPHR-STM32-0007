/*
 * Copyright (c) 2026 GZM Embarcados
 */

/**
 * @file ui_data.c
 * @brief Aggregate data-layer initializer.
 */

#include "data/ui_data.h"
#include "data/nav_data.h"
#include "data/protimer_data.h"
#include "data/clockalarm_data.h"

void ui_data_init(void)
{
	nav_data_init();
	protimer_data_init();
	clockalarm_data_init();
}
