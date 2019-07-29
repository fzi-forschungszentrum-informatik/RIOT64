/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 *
 * @author      Leon Hielscher <hielscher@fzi.de>
 *
 */

#include <stdio.h>
#include <errno.h>

#include "cpu.h"
#include "board.h"


#define OCD_MAGIC_NUMBER_INIT 0xC001C0DE
__attribute__((used)) __attribute__((section (".openocd"))) volatile uint32_t ocd_riot_magic_number = 1;

void board_init(void)
{

    /* Initialize CPU and clocks */
    cpu_init();
    /* Initialize newlib-nano library stubs */
    nanostubs_init();
    fpu_init();
    ocd_riot_magic_number = OCD_MAGIC_NUMBER_INIT;
}
