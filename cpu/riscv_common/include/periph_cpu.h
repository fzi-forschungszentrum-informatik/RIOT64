/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 *
 * @ingroup     cpu_riscv_common
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       Common definitions for RISC-V core peripherals
 * @author      Leon Hielscher <hielscher@fzi.de>
 * @}
 *
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "riscv_arch_conf.h"

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (__riscv_xlen / 8 * 3)

/**
 * @brief   Timer ISR
 */
void timer_isr(void);

/**
 * @brief   64-bit HW timer present
 */
#ifdef RV_HAS_CORE_TIMER
#define TIMER_64BIT_HW
#define PERIPH_TIMER_PROVIDES_SET
#endif


/**
 * @brief   External ISR callback
 */
typedef void (*external_isr_ptr_t)(long intNum);

/**
 * @brief   Set External ISR callback
 */
void set_external_isr_cb(long intNum, external_isr_ptr_t cbFunc);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
