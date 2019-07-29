/*
 * Copyright 2019 FZI Forschungszentrum Informatik
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
 * @brief       SHTC3 Device Driver implementation
 *
 * @author      Leon Hielscher <hielscher@fzi.de>
 * @}
 */


#include <inttypes.h>

#include "shtc3.h"
#include "shtc3_params.h"
#include "shtc3_internals.h"
#include "xtimer.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"


/**
 * @brief	Look up table required for CRC-8 calculation
 *
 * Values taken from the Application Note PDF of Sensirion (December 2011)
 */
static const uint8_t crc_lookup_table[] = {
    0x00, 0x31, 0x62, 0x53,  0xc4, 0xf5, 0xa6, 0x97,
    0xb9, 0x88, 0xdb, 0xea,  0x7d, 0x4c, 0x1f, 0x2e,
    0x43, 0x72, 0x21, 0x10,  0x87, 0xb6, 0xe5, 0xd4,
    0xfa, 0xcb, 0x98, 0xa9,  0x3e, 0x0f, 0x5c, 0x6d,
    0x86, 0xb7, 0xe4, 0xd5,  0x42, 0x73, 0x20, 0x11,
    0x3f, 0x0e, 0x5d, 0x6c,  0xfb, 0xca, 0x99, 0xa8,
    0xc5, 0xf4, 0xa7, 0x96,  0x01, 0x30, 0x63, 0x52,
    0x7c, 0x4d, 0x1e, 0x2f,  0xb8, 0x89, 0xda, 0xeb,
    0x3d, 0x0c, 0x5f, 0x6e,  0xf9, 0xc8, 0x9b, 0xaa,
    0x84, 0xb5, 0xe6, 0xd7,  0x40, 0x71, 0x22, 0x13,
    0x7e, 0x4f, 0x1c, 0x2d,  0xba, 0x8b, 0xd8, 0xe9,
    0xc7, 0xf6, 0xa5, 0x94,  0x03, 0x32, 0x61, 0x50,
    0xbb, 0x8a, 0xd9, 0xe8,  0x7f, 0x4e, 0x1d, 0x2c,
    0x02, 0x33, 0x60, 0x51,  0xc6, 0xf7, 0xa4, 0x95,
    0xf8, 0xc9, 0x9a, 0xab,  0x3c, 0x0d, 0x5e, 0x6f,
    0x41, 0x70, 0x23, 0x12,  0x85, 0xb4, 0xe7, 0xd6,
    0x7a, 0x4b, 0x18, 0x29,  0xbe, 0x8f, 0xdc, 0xed,
    0xc3, 0xf2, 0xa1, 0x90,  0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5b, 0x6a,  0xfd, 0xcc, 0x9f, 0xae,
    0x80, 0xb1, 0xe2, 0xd3,  0x44, 0x75, 0x26, 0x17,
    0xfc, 0xcd, 0x9e, 0xaf,  0x38, 0x09, 0x5a, 0x6b,
    0x45, 0x74, 0x27, 0x16,  0x81, 0xb0, 0xe3, 0xd2,
    0xbf, 0x8e, 0xdd, 0xec,  0x7b, 0x4a, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55,  0xc2, 0xf3, 0xa0, 0x91,
    0x47, 0x76, 0x25, 0x14,  0x83, 0xb2, 0xe1, 0xd0,
    0xfe, 0xcf, 0x9c, 0xad,  0x3a, 0x0b, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57,  0xc0, 0xf1, 0xa2, 0x93,
    0xbd, 0x8c, 0xdf, 0xee,  0x79, 0x48, 0x1b, 0x2a,
    0xc1, 0xf0, 0xa3, 0x92,  0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1a, 0x2b,  0xbc, 0x8d, 0xde, 0xef,
    0x82, 0xb3, 0xe0, 0xd1,  0x46, 0x77, 0x24, 0x15,
    0x3b, 0x0a, 0x59, 0x68,  0xff, 0xce, 0x9d, 0xac,
};

#define CRC_INITIAL 0xFF

static int shtc3_send_command (shtc3_dev_t *dev, uint16_t cmd) {
	const uint8_t cmdBuf[2] = { (cmd  >> 8), (cmd & 0xFF) };
	int res = i2c_write_bytes(dev->params.i2c_dev, dev->params.i2c_addr, cmdBuf, 2, 0);
	DEBUG("Sent command to SHTC3: 0x%" PRIx16 ", Err = %d\n", cmd, res);
	switch(res) {
	case 0:
		return SHTC3_OK;
	case -EIO:
	case -ENXIO:
	case -ETIMEDOUT:
		return SHTC3_NO_RESPONSE;
	default:
		return SHTC3_BUS_ERROR;
	}
}

static int shtc3_read_id(shtc3_dev_t *dev, uint16_t *idVal) {
	int res;
	uint8_t ioBuf[2] = {(SHTC3_REG_ID >> 8), (SHTC3_REG_ID & 0xFF) };

	// Send ID Reg adress
	res = i2c_write_bytes(dev->params.i2c_dev, dev->params.i2c_addr, ioBuf, 2, 0);

	if (res != 0) {
		return res;
	}

	// Read Back
	res = i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr, ioBuf, 2, 0);

	if (res != 0) {
		return res;
	}

	*idVal = ((uint16_t)ioBuf[0] << 8) | ioBuf[1];

	return 0;
}

static int shtc3_power_down(shtc3_dev_t *dev) {

	if (!dev->awake || dev->params.power_conf == SHTC3_ALWAYS_AWAKE) {
		DEBUG("SHTC3 power down ignored.\n");
		return SHTC3_OK;
	}

	int res = shtc3_send_command(dev, SHTC3_CMD_SLEEP);

	if (res != SHTC3_OK) {
		// Something went wrong
		DEBUG("SHTC3 power down error.");
		return res;
	}

	DEBUG("SHTC3 now sleeping.\n");
	dev->awake = false;
	return SHTC3_OK;

}

static int shtc3_wake_up(shtc3_dev_t *dev) {

	if (dev->awake || dev->params.power_conf == SHTC3_ALWAYS_AWAKE) {
		DEBUG("SHTC3 wake up ignored.\n");
		return SHTC3_OK;
	}

	int res = shtc3_send_command(dev, SHTC3_CMD_WAKEUP);

	if (res != SHTC3_OK) {
		// Something went wrong
		DEBUG("SHTC3 wake up error.");
		return res;
	}

	DEBUG("SHTC3 woken up.\n");
	dev->awake = true;
	return SHTC3_OK;

}

int shtc3_init(shtc3_dev_t *dev, const shtc3_params_t *params) {

	DEBUG("Initializing SHTC3...\n");

	dev->initialized = false;
	dev->params = *params;

	i2c_acquire(dev->params.i2c_dev);
	// Send wake-up and reset to make sure the device is up and running
	shtc3_send_command(dev, SHTC3_CMD_WAKEUP);
	shtc3_send_command(dev, SHTC3_CMD_RESET);
	i2c_release(dev->params.i2c_dev);
	dev->awake = true;
	xtimer_usleep(dev->params.power_up_delay_usec);

	// Check if device is present
	uint16_t idVal;
	int res;
	i2c_acquire(dev->params.i2c_dev);
	res = shtc3_read_id(dev, &idVal);

	if (res != 0) {
		// Something went wrong
		DEBUG("Unable to read ID of SHTC3: %d\n", res);
		i2c_release(dev->params.i2c_dev);
		switch(res) {
		case -EIO:
		case -ENXIO:
		case -ETIMEDOUT:
			return SHTC3_NO_RESPONSE;
		default:
			return SHTC3_BUS_ERROR;
		}
	}

	DEBUG("SHTC3 ID Register value: 0x%" PRIx16 "\n", idVal);

	// Verify ID
	if ((idVal & SHTC3_ID_MASK) != SHTC3_ID_REF) {
		i2c_release(dev->params.i2c_dev);
		return SHTC3_ID_MISMATCH;
	}

	// All good, send SHTC3 to sleep
	dev->initialized = true;
	res = shtc3_power_down(dev);
	i2c_release(dev->params.i2c_dev);
	return res;
}

void shtc3_deinit(shtc3_dev_t *dev) {
	dev->initialized = false;
	i2c_acquire(dev->params.i2c_dev);
	shtc3_send_command(dev, SHTC3_CMD_SLEEP);
	i2c_release(dev->params.i2c_dev);
	return;
}

int shtc3_do_measurement(shtc3_dev_t *dev, shtc3_raw_measurement_t* data) {
	uint8_t inputBuf[6];
	int res;

	if (!dev->initialized) {
		return SHTC3_UNINITIALIZED;
	}

	DEBUG("Start SHTC3 measurement\n");

	i2c_acquire(dev->params.i2c_dev);

	// Make sure device is awake
	if (!dev->awake) {
		res = shtc3_wake_up(dev);
		if (res != SHTC3_OK) {
			i2c_release(dev->params.i2c_dev);
			return res;
		} else {
			i2c_release(dev->params.i2c_dev);
			xtimer_usleep(dev->params.power_up_delay_usec);
			i2c_acquire(dev->params.i2c_dev);
		}
	}

	// Send measure command
	uint16_t command;

	if (dev->params.clock_conf == SHTC3_CLOCK_STRETCHING_DISABLED) {
		if (dev->params.power_conf == SHTC3_LOW_POWER_MODE)
			command = SHTC3_CMD_NO_STRETCH_T_FIRST_LOWPWR;
		else
			command = SHTC3_CMD_NO_STRETCH_T_FIRST_NORMAL;
	} else {
		if (dev->params.power_conf == SHTC3_LOW_POWER_MODE)
			command = SHTC3_CMD_STRETCH_EN_T_FIRST_LOWPWR;
		else
			command = SHTC3_CMD_STRETCH_EN_T_FIRST_NORMAL;
	}

	res = shtc3_send_command(dev, command);
	if (res != SHTC3_OK) {
		i2c_release(dev->params.i2c_dev);
		return res;
	}

	// Read results
	if (dev->params.clock_conf == SHTC3_CLOCK_STRETCHING_DISABLED) {
		// Give the sensor time to measure
		xtimer_usleep(dev->params.power_conf == SHTC3_LOW_POWER_MODE ? dev->params.low_power_meas_delay_usec
							: dev->params.normal_meas_delay_usec);
	}

	DEBUG("Reading SHTC3 measurement\n");
	res = i2c_read_bytes(dev->params.i2c_dev, dev->params.i2c_addr, inputBuf, 6, 0);
	switch(res) {	// Check for errors
	case 0:
		break;
	case -EIO:
	case -ENXIO:
	case -ETIMEDOUT:
		i2c_release(dev->params.i2c_dev);
		return SHTC3_NO_RESPONSE;
	default:
		i2c_release(dev->params.i2c_dev);
		return SHTC3_BUS_ERROR;
	}

	// Back to sleep
	res = shtc3_power_down(dev);
	i2c_release(dev->params.i2c_dev);
	if (res != SHTC3_OK) {
		return res;
	}

	// Evaluate response
	uint8_t crc = CRC_INITIAL;
	crc = crc_lookup_table[inputBuf[0] ^ crc];
	crc = crc_lookup_table[inputBuf[1] ^ crc];
	if (crc != inputBuf[2]) {
		DEBUG("SHTC temperature CRC error.\n");
		return SHTC3_CRC_ERROR;
	}

	crc = CRC_INITIAL;
	crc = crc_lookup_table[inputBuf[3] ^ crc];
	crc = crc_lookup_table[inputBuf[4] ^ crc];
	if (crc != inputBuf[5]) {
		DEBUG("SHTC humidity CRC error.\n");
		return SHTC3_CRC_ERROR;
	}

	data->temp_raw =	((uint16_t)inputBuf[0] << 8) | inputBuf[1];
	data->humid_raw =	((uint16_t)inputBuf[3] << 8) | inputBuf[4];
	return SHTC3_OK;
}

int shtc3_get_centi_celsius(uint16_t temp_raw) {
	// degC = -45 + 175 * temp_raw / 2^16
	uint32_t temperature = (uint32_t)temp_raw * 17500;
	temperature >>= 16;
	return ((int)temperature - 4500);
}

int shtc3_get_relHumid_mil(uint16_t humid_raw) {
	// rel% = 100 * humid_raw / 2^16
	uint32_t humid = (uint32_t)humid_raw * 1000;
	humid >>= 16;
	return (int)humid;
}

int shtc3_set_power_mode(shtc3_dev_t *dev, shtc3_power_conf_t power_conf) {
	int res;
	if (!dev->initialized) {
		return SHTC3_UNINITIALIZED;
	}

	if ((power_conf == SHTC3_ALWAYS_AWAKE && dev->awake) ||
			(power_conf != SHTC3_ALWAYS_AWAKE && !dev->awake)) {
		// No need to change device state
		res = SHTC3_OK;
	} else {

		i2c_acquire(dev->params.i2c_dev);
		if (power_conf == SHTC3_ALWAYS_AWAKE) {
			res = shtc3_send_command(dev, SHTC3_CMD_WAKEUP);
			dev->awake = true;
		} else {
			res = shtc3_send_command(dev, SHTC3_CMD_SLEEP);
			dev->awake = false;
		}

		i2c_release(dev->params.i2c_dev);
	}

	dev->params.power_conf = power_conf;
	return res;
}

int shtc3_set_clock_mode(shtc3_dev_t *dev, shtc3_clock_conf_t clock_conf) {
	if (!dev->initialized) {
		return SHTC3_UNINITIALIZED;
	}

	dev->params.clock_conf = clock_conf;
	return SHTC3_OK;
}

