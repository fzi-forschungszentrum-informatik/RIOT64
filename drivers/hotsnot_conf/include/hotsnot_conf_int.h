/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef HOTSNOT_CONF_INT_H
#define HOTSNOT_CONF_INT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef I2C_ACK
#define I2C_ACK         (0)
#endif

#define HOTSNOT_I2C_DEV			(0)
#define HOTSNOT_EEPROM_ADDRESS 	(0x50)
#define HOTSNOT_EEPROM_OFFSET 	(0)
#define HOTSNOT_EEPROM_PAGE_SIZE (32)

#define HOTSNOT_PARITY_INIT 	(0x55555555UL)


#endif /* HOTSNOT_CONF_INT_H */
