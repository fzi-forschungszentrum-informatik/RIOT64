/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_shtc3
 *
 * @{
 * @file
 * @brief       Default configuration for the SHTC3 driver
 *
 * @author      Leon Hielscher <hielscher@fzi.de>
 */

#ifndef SHTC3_PARAMS_H
#define SHTC3_PARAMS_H

#include "shtc3.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name    Set default configuration parameters for the SHTC3
 * @{
 */
#ifndef SHTC3_PARAM_I2C_DEV
#define SHTC3_PARAM_I2C_DEV         I2C_DEV(0)
#endif

#ifndef SHTC3_PARAM_I2C_ADDR
#define SHTC3_PARAM_I2C_ADDR        (0x70)
#endif

#ifndef SHTC3_PARAM_LOW_POWER_DELAY_USEC
#define SHTC3_PARAM_LOW_POWER_DELAY_USEC        (800)
#endif

#ifndef SHTC3_PARAM_NORMAL_DELAY_USEC
#define SHTC3_PARAM_NORMAL_DELAY_USEC        (12100)
#endif

#ifndef SHTC3_POWERUP_DELAY_USEC
#define SHTC3_POWERUP_DELAY_USEC 240
#endif


#define SHTC3_PARAMS_DEFAULT              \
    {                                      \
        .i2c_dev = SHTC3_PARAM_I2C_DEV,   \
        .i2c_addr = SHTC3_PARAM_I2C_ADDR, \
	    .clock_conf = SHTC3_CLOCK_STRETCHING_DISABLED, \
	    .power_conf = SHTC3_NORMAL_MODE, \
	    .low_power_meas_delay_usec = SHTC3_PARAM_LOW_POWER_DELAY_USEC, \
	    .normal_meas_delay_usec = SHTC3_PARAM_NORMAL_DELAY_USEC, \
		.power_up_delay_usec = SHTC3_POWERUP_DELAY_USEC \
    }

static const shtc3_params_t shtc3_default_params =  SHTC3_PARAMS_DEFAULT;


#ifdef __cplusplus
}
#endif

#endif /* SHTC3_PARAMS_H */
/** @} */
