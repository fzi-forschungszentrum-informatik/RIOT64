/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_shtc3 Humidity and Temperature Sensor
 * @ingroup     drivers_sensors
 * @brief       Driver for Sensirion SHTC3 Humidity and Temperature I2C Sensor
 * @{
 *
 * @file
 * @brief       SHTC3 Device Driver
 *
 * @author      Leon Hielscher <hielscher@fzi.de>
 */


#ifndef SHTC3_H
#define SHTC3_H

#include <inttypes.h>
#include <stdbool.h>
#include <errno.h>

#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Clock stretching setting for the SHTC3
 *
 */
typedef enum {
	/** Do not use I2C clock stretching **/
	SHTC3_CLOCK_STRETCHING_DISABLED = 0,
	/** Use I2C clock stretching (may not be compatible with all I2C peripherals) **/
	SHTC3_CLOCK_STRETCHING_ENABLED,
} shtc3_clock_conf_t;

/**
 * @brief Power configuration for the SHTC3
 *
 */
typedef enum {
	/** Normal power measurements, sleep during idle phases **/
	SHTC3_NORMAL_MODE = 0,
	/** Low power measurements, sleep during idle phases **/
	SHTC3_LOW_POWER_MODE,
	/** Normal power measurements, stay awake during idle phases **/
	SHTC3_ALWAYS_AWAKE
} shtc3_power_conf_t;


/**
 * @brief SHTC3 temperature humidity sensor parameters
 */
typedef struct {
    /* I2C details */
    i2c_t i2c_dev;                      		/**< I2C device which is used */
    uint8_t i2c_addr;                  		 	/**< I2C address */
    shtc3_clock_conf_t clock_conf;				/**< I2C clock stretching configuration */
    shtc3_power_conf_t power_conf;				/**< Power mode selection */
    uint16_t	low_power_meas_delay_usec;		/**< Time for a low power measurement in microseconds */
    uint16_t	normal_meas_delay_usec;			/**< Time for a normal measurement in microseconds */
    uint16_t 	power_up_delay_usec;			/**< Time delay after an wake-up command in microseconds */
} shtc3_params_t;

/**
 * @brief SHTC3 temperature humidity sensor device
 */
typedef struct {
	shtc3_params_t params; /**< Device parameters */
	bool initialized; /**< Has device been initialized */
	bool awake; /**< Is device awake or sleeping*/
} shtc3_dev_t;

/**
 * @brief SHTC3 raw measurement values#include "saul.h"
 */
typedef struct {
	uint16_t temp_raw; /**< Raw temperature value provided by sensor */
	uint16_t humid_raw; /**< Raw humidity value provided by sensor */
} shtc3_raw_measurement_t;


/**
 * @brief   Status and error return codes
 */
enum {
    SHTC3_OK			=  0,    /**< Everything was fine */
	SHTC3_NO_RESPONSE	= -1,	 /**< Device not responding*/
    SHTC3_ID_MISMATCH	= -2,    /**< ID register mismatch*/
    SHTC3_CRC_ERROR		= -3,    /**< CRC mismatch */
	SHTC3_UNINITIALIZED	= -4,    /**< Device not initialized */
	SHTC3_BUS_ERROR		= -5,    /**< I2C bus error */
};


/**
 * @brief   Initialize the given SHTC3 device
 *
 * @param[out] dev          Initialized device descriptor of the SHTC3 device
 * @param[in]  params       The parameters for the SHTC3 device
 *
 * @return                  SHTC3_OK on success
 * @return                  SHTC3_NO_RESPONSE if communication with the device failed
 * @return                  SHTC3_BUS_ERROR on internal bus errors
 * @return                  SHTC3_ID_MISSMATCH when the value of the ID register does not match the device
 */
int shtc3_init(shtc3_dev_t *dev, const shtc3_params_t *params);

/**
 * @brief   Deinitialize  the given SHTC3 device
 *
 *  Issues a sleep command to the device and deinitializes the driver.
 *  This should be called after an error has occured in order to attempt to reduce
 *  power conmsumption and reset the driver to a defined state.
 *
 */
void shtc3_deinit(shtc3_dev_t *dev);

/**
 * @brief   Perform a single humidity and temperature measurement.
 *
 * @param[in] dev		Initialized device descriptor of the SHTC3 device
 * @param[out] data		Raw measurement data
 *
 * @return              SHTC3_OK on success
 * @return              SHTC3_NO_RESPONSE if communication with the device failed
 * @return              SHTC3_BUS_ERROR on internal bus errors
 * @return				SHTC3_CRC_ERROR on a checksum mismatch
 * @return				SHTC3_UNITITILAIZED if device has not been initialized
 *
 */
int shtc3_do_measurement(shtc3_dev_t *dev, shtc3_raw_measurement_t* data);

/**
 * @brief Convert a raw temperature measurement value to centi degrees celsius.
 *
 * E.g. a return value of -1234 corresponds to a measurement of -12.34 degC.
 *
 * @param[in] temp_raw		Raw temperature measurement value
 *
 * @returns				Temperature in centi degrees celsius.
 */
int shtc3_get_centi_celsius(uint16_t temp_raw);

/**
 * @brief Convert a raw humidity measurement value to relative humidity in per mil.
 *
 * E.g. a return value of -425 corresponds to a measurement of 42.5%.
 *
 * @param[in] humid_raw		Raw humidity measurement value
 *
 * @returns				Relative humidity in per mil.
 */
int shtc3_get_relHumid_mil(uint16_t humid_raw);

/**
 * @brief Change the power mode of the SHTC3 device
 *
 *
 * @param[in] dev			Initialized device descriptor of the SHTC3 device
 * @param[in] power_conf	The new power mode.
 *
 * @return              SHTC3_OK on success
 * @return              SHTC3_NO_RESPONSE if communication with the device failed
 * @return              SHTC3_BUS_ERROR on internal bus errors
 * @return				SHTC3_UNITITILAIZED if device has not been initialized
 */
int shtc3_set_power_mode(shtc3_dev_t *dev, shtc3_power_conf_t power_conf);

/**
 * @brief Change the clock settings for the SHTC3 device
 *
 *
 * @param[in] dev			Initialized device descriptor of the SHTC3 device
 * @param[in] clock_conf	The clock setting.
 *
 * @return              SHTC3_OK on success
 * @return				SHTC3_UNITITILAIZED if device has not been initialized
 */
int shtc3_set_clock_mode(shtc3_dev_t *dev, shtc3_clock_conf_t clock_conf);

#ifdef __cplusplus
}
#endif


#endif /* SHTC3_H */
/** @} */
