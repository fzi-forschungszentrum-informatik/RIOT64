/*
 * Copyright (C) 2017 Freie Universität Berlin
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_hotsnot
 * @{
 *
 * @file
 * @brief       Board initialization for the HoTSNoT platform
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Leon Hielscher <hielscher@fzi.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#ifdef MODULE_HOTSNOT_CONF
#include "hotsnot_conf.h"
#endif


#ifdef MODULE_HOTSNOT_CONF
void board_hall_sense_enable(void) {
	if (hotsnot_conf_has_features(HOTSNOT_CONF_FEATURE_HALL)) {
		HALL_POWER_PORT->OUTSET = 1 << (HALL_POWER_PIN & 0x1F);
		HALL_SENSE_PORT->PIN_CNF[HALL_SENSE_PIN & 0x1F] = 	(GPIO_PIN_CNF_DIR_Input			<< GPIO_PIN_CNF_DIR_Pos) 	|
															(GPIO_PIN_CNF_INPUT_Connect		<< GPIO_PIN_CNF_INPUT_Pos)	|
															(GPIO_PIN_CNF_PULL_Pullup		<< GPIO_PIN_CNF_PULL_Pos)	|
															(GPIO_PIN_CNF_DRIVE_S0S1		<< GPIO_PIN_CNF_DRIVE_Pos)	|
															(GPIO_PIN_CNF_SENSE_Disabled	<< GPIO_PIN_CNF_SENSE_Pos);
	}
}

void board_hall_sense_disable(void) {
	if (hotsnot_conf_has_features(HOTSNOT_CONF_FEATURE_HALL)) {
		HALL_POWER_PORT->OUTCLR = 1 << (HALL_POWER_PIN & 0x1F);
		HALL_SENSE_PORT->PIN_CNF[HALL_SENSE_PIN & 0x1F] = 	(GPIO_PIN_CNF_DIR_Input			<< GPIO_PIN_CNF_DIR_Pos) 	|
				    										(GPIO_PIN_CNF_INPUT_Connect		<< GPIO_PIN_CNF_INPUT_Pos)	|
															(GPIO_PIN_CNF_PULL_Pulldown		<< GPIO_PIN_CNF_PULL_Pos)	|
															(GPIO_PIN_CNF_DRIVE_S0S1		<< GPIO_PIN_CNF_DRIVE_Pos)	|
															(GPIO_PIN_CNF_SENSE_Disabled	<< GPIO_PIN_CNF_SENSE_Pos);
	}
}

void board_hall_init(void) {
    if (hotsnot_conf_has_features(HOTSNOT_CONF_FEATURE_HALL)) {

		HALL_SENSE_PORT->PIN_CNF[HALL_SENSE_PIN & 0x1F] = 	(GPIO_PIN_CNF_DIR_Input			<< GPIO_PIN_CNF_DIR_Pos) 	|
		    												(GPIO_PIN_CNF_INPUT_Connect		<< GPIO_PIN_CNF_INPUT_Pos)	|
															(GPIO_PIN_CNF_PULL_Pulldown		<< GPIO_PIN_CNF_PULL_Pos)	|
															(GPIO_PIN_CNF_DRIVE_S0S1		<< GPIO_PIN_CNF_DRIVE_Pos)	|
															(GPIO_PIN_CNF_SENSE_Disabled	<< GPIO_PIN_CNF_SENSE_Pos);
    	HALL_POWER_PORT->PIN_CNF[HALL_POWER_PIN & 0x1F] = 	(GPIO_PIN_CNF_DIR_Output		<< GPIO_PIN_CNF_DIR_Pos) 	|
    														(GPIO_PIN_CNF_INPUT_Disconnect	<< GPIO_PIN_CNF_INPUT_Pos)	|
															(GPIO_PIN_CNF_PULL_Disabled		<< GPIO_PIN_CNF_PULL_Pos)	|
															(GPIO_PIN_CNF_DRIVE_D0H1		<< GPIO_PIN_CNF_DRIVE_Pos)	|
															(GPIO_PIN_CNF_SENSE_Disabled	<< GPIO_PIN_CNF_SENSE_Pos);
		HALL_POWER_PORT->OUTCLR = 1 << (HALL_POWER_PIN & 0x1F);
    }
}

int board_hall_get_state(void) {
	if (hotsnot_conf_has_features(HOTSNOT_CONF_FEATURE_HALL)) {
		return (HALL_SENSE_PORT->IN & (1 << (HALL_SENSE_PIN & 0x1F))) == 0;
	} else {
		return 0;
	}
}

#else

void board_hall_sense_enable(void) {
	return;
}

void board_hall_sense_disable(void) {
	return;
}

void board_hall_init(void) {
	return;
}

int board_hall_get_state(void) {
	return 0;
}

#endif

void board_init(void)
{
    /* initialize the boards LEDs */
    LED_PORT->DIRSET = (LED_MASK);
    LED_PORT->OUTSET = (LED_MASK);

    /* initialize the CPU */
    cpu_init();

#ifdef MODULE_HOTSNOT_CONF
    hotsnot_conf_init();

    if (hotsnot_conf_has_features(HOTSNOT_CONF_FEATURE_DCDC)) {
    	NRF_POWER->DCDCEN = 1;
    }

    board_hall_init();
#endif
}


