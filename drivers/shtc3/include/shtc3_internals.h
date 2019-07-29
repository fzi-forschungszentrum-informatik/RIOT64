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
* @brief       Internal defines required by the SHTC3 driver

*
* @author      Leon Hielscher <hielscher@fzi.de>
*/

#ifndef SHTC3_INTERNALS_H
#define SHTC3_INTERNALS_H


#ifdef __cplusplus
extern "C" {
#endif


#define SHTC3_REG_ID	0xEFC8
#define SHTC3_ID_MASK	0x083F
#define SHTC3_ID_REF	0x0807

#define SHTC3_CMD_SLEEP		0xB098
#define SHTC3_CMD_WAKEUP 	0x3517
#define SHTC3_CMD_RESET		0x805D

#define SHTC3_CMD_STRETCH_EN_T_FIRST_NORMAL	0x7CA2
#define SHTC3_CMD_STRETCH_EN_T_FIRST_LOWPWR	0x6458
#define SHTC3_CMD_STRETCH_EN_H_FIRST_NORMAL	0x5C24
#define SHTC3_CMD_STRETCH_EN_H_FIRST_LOWPWR	0x44DE
#define SHTC3_CMD_NO_STRETCH_T_FIRST_NORMAL	0x7866
#define SHTC3_CMD_NO_STRETCH_T_FIRST_LOWPWR	0x609C
#define SHTC3_CMD_NO_STRETCH_H_FIRST_NORMAL	0x58E0
#define SHTC3_CMD_NO_STRETCH_H_FIRST_LOWPWR	0x401A


#ifdef __cplusplus
}
#endif


#endif /* SHTC3_INTERNALS_H */
/** @} */
