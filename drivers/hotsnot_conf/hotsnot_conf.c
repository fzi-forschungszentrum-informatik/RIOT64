/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "hotsnot_conf.h"
#include "periph/i2c.h"
#include "hotsnot_conf_int.h"

#include <string.h>
#include <stdlib.h>


static const char id_ref[HOTSNOT_CONF_HW_ID_LEN] = HOTSNOT_CONF_HW_ID_STR;
static hotsnot_conf_t conf_data;

static const hotsnot_conf_t invalid_conf = {
		.hw_id = HOTSNOT_CONF_HW_ID_STR,
		.hw_rev = 		 {'E', 'R', 'R', '\0'},
		.prod_date =     {'x', 'x', 'x', 'x', '-', 'x', 'x', '-', 'x', 'x', '\0', '\0'},
		.friendly_name = {'I', 'N', 'V', 'A', 'L', 'I', 'D', ' ', 'C', 'O', 'N', 'F', 'I', 'G', '\0', '\0'},
		.serial_no = 0,
		.feature_mask = 0,
		.shunt_mohms = 0,
		._reserved = 0,
		._parity = 0
};

static bool hotsnot_conf_verify(hotsnot_conf_t* conf) {
	uint32_t parity = HOTSNOT_PARITY_INIT;
	uint32_t* data = (uint32_t*)(conf);
	size_t i;

	if(memcmp(id_ref, conf_data.hw_id, HOTSNOT_CONF_HW_ID_LEN) != 0) {
		return false;
	}

	for (i = 0; i < (sizeof(hotsnot_conf_t) / 4); i++) {
		parity ^= data[i];
	}

	return (parity == 0);
}

bool hotsnot_conf_init(void) {
	uint8_t adrBuf[2];
	int res;

	// Clear data
	memcpy(&conf_data, &invalid_conf, sizeof(hotsnot_conf_t));

	adrBuf[1] = HOTSNOT_EEPROM_OFFSET & 0xFF;
	adrBuf[0] = (HOTSNOT_EEPROM_OFFSET >> 8) & 0xFF;

	i2c_acquire(I2C_DEV(HOTSNOT_I2C_DEV));

	// Write Address
	res = i2c_write_bytes(I2C_DEV(HOTSNOT_I2C_DEV), HOTSNOT_EEPROM_ADDRESS, adrBuf, 2, 0);
	if (res != I2C_ACK) {
		i2c_release(I2C_DEV(HOTSNOT_I2C_DEV));
		return false;
	}

	// Read Data
	res = i2c_read_bytes(I2C_DEV(HOTSNOT_I2C_DEV), HOTSNOT_EEPROM_ADDRESS, &conf_data, sizeof(hotsnot_conf_t), 0);
	if (res != I2C_ACK) {
		// Clear Data again
		memcpy(&conf_data, &invalid_conf, sizeof(hotsnot_conf_t));
		i2c_release(I2C_DEV(HOTSNOT_I2C_DEV));
		return false;
	}
	i2c_release(I2C_DEV(HOTSNOT_I2C_DEV));

	// Verify ID
	if (!hotsnot_conf_verify(&conf_data)) {
		// Clear Data again
		memcpy(&conf_data, &invalid_conf, sizeof(hotsnot_conf_t));
		return false;
	} else {
		return true;
	}
}

hotsnot_conf_t* hotsnot_conf_get(void) {
	return &conf_data;
}

void hotsnot_conf_force_features_enabled(uint16_t featureMask) {
	conf_data.feature_mask |= featureMask;
}

void hotsnot_conf_force_features_disabed(uint16_t featureMask) {
	conf_data.feature_mask &= ~featureMask;
}

bool hotsnot_conf_has_features(uint16_t featureMask) {
	return (conf_data.feature_mask & featureMask) == featureMask;
}

#if HOTSNOT_CONF_WRITE_ENABLE
static bool eeprom_write_ack(void) {
	char dummy[2] = {0, 0};
	int res = i2c_write_bytes(I2C_DEV(HOTSNOT_I2C_DEV), HOTSNOT_EEPROM_ADDRESS, dummy, 2, 0);
	return (res == I2C_ACK);
}


bool hotsnot_conf_write(const hotsnot_conf_t* conf_to_write) {

	uint8_t* i2c_buffer = (uint8_t*) malloc(HOTSNOT_EEPROM_PAGE_SIZE + 2);
	if (i2c_buffer == NULL) {
		return false;
	}

	memcpy(&conf_data, conf_to_write, sizeof(hotsnot_conf_t));
	conf_data._parity = HOTSNOT_PARITY_INIT;
	uint32_t* data = (uint32_t*)(conf_to_write);
	size_t i;
	for (i = 0; i < (sizeof(hotsnot_conf_t) / 4) - 1; i++) {
		conf_data._parity ^= data[i];
	}


	memcpy(i2c_buffer + 2, &conf_data, sizeof(hotsnot_conf_t));

	int res = I2C_ACK;
	size_t byte_offset = 0;
	i2c_acquire(I2C_DEV(HOTSNOT_I2C_DEV));

	if (!eeprom_write_ack()) {
		i2c_release(I2C_DEV(HOTSNOT_I2C_DEV));
		memset(&conf_data, 0, sizeof(hotsnot_conf_t));
		free(i2c_buffer);
		return false;
	}

	while (byte_offset < sizeof(hotsnot_conf_t)) {
		size_t chunk = (sizeof(hotsnot_conf_t) - byte_offset) > HOTSNOT_EEPROM_PAGE_SIZE ? HOTSNOT_EEPROM_PAGE_SIZE
							: (sizeof(hotsnot_conf_t) - byte_offset);
		memcpy(i2c_buffer + 2, (uint8_t*)(&conf_data) + byte_offset, chunk);
		i2c_buffer[1] = (HOTSNOT_EEPROM_OFFSET + byte_offset) & 0xFF;
		i2c_buffer[0] = ((HOTSNOT_EEPROM_OFFSET + byte_offset) >> 8) & 0xFF;
		res |= i2c_write_bytes(I2C_DEV(HOTSNOT_I2C_DEV), HOTSNOT_EEPROM_ADDRESS, i2c_buffer, chunk + 2, 0);
		while(!eeprom_write_ack()) {
			continue;
		}
		byte_offset += chunk;
	}

	i2c_release(I2C_DEV(HOTSNOT_I2C_DEV));
	memset(&conf_data, 0, sizeof(hotsnot_conf_t));
	free(i2c_buffer);

	return (res == I2C_ACK);
}
#endif
