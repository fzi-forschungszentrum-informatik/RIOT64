/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef HOTSNOT_CONF_H
#define HOTSNOT_CONF_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HOTSNOT_CONF_WRITE_ENABLE
#define HOTSNOT_CONF_WRITE_ENABLE (0)
#endif

#define HOTSNOT_CONF_FEATURE_LFOSC		(1 << 0)
#define HOTSNOT_CONF_FEATURE_BATSENSE	(1 << 1)
#define HOTSNOT_CONF_FEATURE_SHTC3		(1 << 2)
#define HOTSNOT_CONF_FEATURE_BMP280		(1 << 3)
#define HOTSNOT_CONF_FEATURE_HALL		(1 << 4)
#define HOTSNOT_CONF_FEATURE_DCDC		(1 << 5)

#define HOTSNOT_CONF_HW_ID_LEN 			(8)
#define HOTSNOT_CONF_HW_REV_LEN 		(4)
#define HOTSNOT_CONF_PROD_DATE_LEN  	(12)
#define HOTSNOT_CONF_FRIENDLY_NAME_LEN	(16)

#define HOTSNOT_CONF_HW_ID_STR {'H', 'o', 'T', 'S', 'N', 'o', 'T', '\0'}

/**
 * @brief Permanent HoTSNoT Board parameters
 */
typedef struct __attribute__((packed)) hotsnot_conf {
	char hw_id[HOTSNOT_CONF_HW_ID_LEN];			/**< Constant HW ID, should be "HoTSNoT" (zero terminated) */
	char hw_rev[HOTSNOT_CONF_HW_REV_LEN];		/**< Hardware revision (zero terminated) */
	char prod_date[HOTSNOT_CONF_PROD_DATE_LEN]; /**< Production Date "yyyy-mm-dd" (zero terminated) */
	char friendly_name[HOTSNOT_CONF_FRIENDLY_NAME_LEN]; /**< Human readable identifier (zero terminated) */
	uint32_t serial_no;							/**< Serial Number */
	uint32_t feature_mask;						/**< Bit Mask of available features */
	uint32_t shunt_mohms;						/**< Resistance of current shunt in mOhms. May be Zero.*/
	uint32_t _reserved;							/**< Reserved. Keep as zero. */
	uint32_t _parity;							/**< Parity for internal verification. */
} hotsnot_conf_t;


bool hotsnot_conf_init(void);
hotsnot_conf_t* hotsnot_conf_get(void);
void hotsnot_conf_force_features_enabled(uint16_t featureMask);
void hotsnot_conf_force_features_disabed(uint16_t featureMask);
bool hotsnot_conf_has_features(uint16_t featureMask);


#if HOTSNOT_CONF_WRITE_ENABLE
bool hotsnot_conf_write(const hotsnot_conf_t* conf_to_write);
#endif


#endif /* HOTSNOT_CONF_H */
