/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sifive_common
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       Interface for SiFive PLIC
 * @author      Leon Hielscher <hielscher@fzi.de>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "vendor/plic_driver.h"
#include "periph_cpu.h"
#include "vendor/platform.h"


static external_isr_ptr_t _ext_isrs[PLIC_NUM_INTERRUPTS];

/* NULL interrupt handler */
void null_isr(long num)
{
    (void) num;
}

void rv_plic_init(void) {
	
    PLIC_init(PLIC_CTRL_ADDR, PLIC_NUM_INTERRUPTS, PLIC_NUM_PRIORITIES);
    
    for (int i = 0; i < PLIC_NUM_INTERRUPTS; i++) {
        _ext_isrs[i] = null_isr;
    } 
	
}

void rv_plic_hanlde_isr(void) {
	
	plic_source intNum = PLIC_claim_interrupt();

    if ((intNum > 0) && (intNum < PLIC_NUM_INTERRUPTS)) {
        _ext_isrs[intNum]((rvxlen_unsigned_t) intNum);
    }

    PLIC_complete_interrupt(intNum);
}

/**
 * @brief   Set External ISR callback
 */
void set_external_isr_cb(rvxlen_signed_t intNum, external_isr_ptr_t cbFunc)
{
    if ((intNum > 0) && (intNum < PLIC_NUM_INTERRUPTS)) {
        _ext_isrs[intNum] = cbFunc;
    }
}
