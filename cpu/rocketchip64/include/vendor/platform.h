/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _ROCKETCHIP64_PLATFORM_H_
#define _ROCKETCHIP64_PLATFORM_H_

#include "vendor/plic.h"
#include "vendor/clint.h"
#include "vendor/uart.h"

#define MASKROM_MEM_ADDR 		(0x00001000)
#define TRAPVEC_TABLE_CTRL_ADDR	(0x00001010)
#define CLINT_CTRL_ADDR 		(0x02000000)
#define PLIC_CTRL_ADDR 			(0x0C000000)
#define UART0_CTRL_ADDR 		(0x64000000)


#define IOF0_UART0_MASK         (0x00030000)
#define IOF_UART0_RX   			(16u)
#define IOF_UART0_TX   			(17u)


#define INT_RESERVED 			0
#define INT_UART0_BASE 			2

#define _REG32(p, i) 			(*(volatile uint32_t *) ((p) + (i)))
#define _REG32P(p, i) 			((volatile uint32_t *) ((p) + (i)))
#define CLINT_REG(offset) 		_REG32(CLINT_CTRL_ADDR, offset)
#define PLIC_REG(offset) 		_REG32(PLIC_CTRL_ADDR, offset)
#define UART0_REG(offset) 		_REG32(UART0_CTRL_ADDR, offset)


#define PLIC_NUM_INTERRUPTS 52
#define PLIC_NUM_PRIORITIES 7

#define RTC_FREQ 			1000000

#endif /* _ROCKETCHIP64_PLATFORM_H_ */
