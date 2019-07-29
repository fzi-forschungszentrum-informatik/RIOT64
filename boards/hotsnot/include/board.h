/*
 * Copyright (C) 2016-2017 Freie Universität Berlin
 * Copyright (C) 2019 FZI Forschungszentrum Informatik 
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_hotsnot HoTSNoT board
 * @ingroup     boards
 * @brief		Support for the HoTSNoT nRf52840 Bluetooth sensor board
 * @{
 *
 * @file
 * @brief       Board specific configuration for the HoTSNoT platform
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Leon Hielscher <hielscher@fzi.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 23)

#define LED_PORT            (NRF_P0)
#define LED0_MASK           (1 << 23)

#define LED_MASK            (LED0_MASK)

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

/** @} */

/**
 * @name    Hall sensor pin configuration
 * @{
 */

#define HALL_POWER_PORT		(NRF_P0)
#define	HALL_POWER_PIN		17
#define HALL_SENSE_PORT		(NRF_P0)
#define	HALL_SENSE_PIN		16
/** @} */


/**
 * @brief   Initialize the platform
 */
void board_init(void);

/**
 * @brief   Initialize hall sensor GPIO
 */
void board_hall_init(void);

/**
 * @brief   Power up the hall sensor
 */
void board_hall_sense_enable(void);

/**
 * @brief   Power down the hall sensor
 */
void board_hall_sense_disable(void);

/**
 * @brief Get the hall sensor state
 */
int board_hall_get_state(void);


#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
