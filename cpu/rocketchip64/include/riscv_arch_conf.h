/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 *
  * @author      Leon Hielscher <hielscher@fzi.de>
 *
 */
 
#ifndef RISCV_ARCH_CONF_H
#define RISCV_ARCH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif
 
#define RV_HAS_CORE_TIMER
#define RV_MTIMECMP_ADDR 	0x02004000
#define RV_MTIME_ADDR    	0x0200BFF8

#ifdef __cplusplus
}
#endif

#endif /* RISCV_ARCH_CONF_H */


