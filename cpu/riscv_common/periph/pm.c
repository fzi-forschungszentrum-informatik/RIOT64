/*
 * Copyright 2017 Ken Rabold
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_riscv_common
 * @ingroup     cpu
 * @{
 *
 * @file        pm.c
 * @brief       Implementation of the CPU power management for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */


#include "periph/pm.h"
#include "vendor/platform.h"

void pm_set_lowest(void)
{
     __asm__("wfi"); 
}

void pm_off(void)
{
    while(1) {
        __asm__("wfi");
    }
}

void pm_reboot(void)
{
    while(1) {
        __asm__("wfi");
    }
}
