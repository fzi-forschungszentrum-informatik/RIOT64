/*
 * Copyright 2017 Ken Raboldi
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_riscv_common
 * @ingroup     cpu
 * @{
 *
 * @file        timer.c
 * @brief       Low-level core timer implementation for RISC-V CPUs 
 *              compatible with the RISC-V privileged specification.
 *
 * @author      Ken Rabold
 * @author      Leon Hielscher <hielscher@fzi.de>
 * @}
 */

#include <stdlib.h>
#include <unistd.h>

#include "cpu.h"
#include "cpu_conf.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "riscv_arch_conf.h"
#include "periph/timer.h"
#include "vendor/encoding.h"
#include "vendor/platform.h"

#ifdef RV_HAS_CORE_TIMER

#ifndef RV_CORE_TIMER_FREQ
#error "Frequency for RISC-V Core Timer not defined."
#endif

#ifndef RV_MTIME_ADDR
#error "Address of MTIME Register not defined."
#endif

#ifndef RV_MTIMECMP_ADDR
#error "Address of MTIMECMP Register not defined."
#endif

#ifndef SPURIOUS_TIMER_ISR_MAX_SPIN
#define SPURIOUS_TIMER_ISR_MAX_SPIN 10
#endif


#if __riscv_xlen == 64

// 64-Bit access

static inline uint64_t read_time_csr(void) {
	volatile uint64_t *mtime =
        (uint64_t *) (RV_MTIME_ADDR);
	return *mtime;
}

static inline uint64_t read_mtimecmp(void) {
	volatile uint64_t *mtimecmp =
        (uint64_t *) (RV_MTIMECMP_ADDR);
	return *mtimecmp;
}


static inline void write_mtimecmp_csr(uint64_t cmpVal) {
	volatile uint64_t *mtimecmp =
        (uint64_t *) (RV_MTIMECMP_ADDR);
	*mtimecmp = cmpVal;
}

static inline void set_max_mtimecmp(void) {
	volatile uint64_t *mtimecmp =
        (uint64_t *) (RV_MTIMECMP_ADDR);
	*mtimecmp = 0xFFFFFFFFFFFFFFFFUL;
}

static inline void clear_mtip(void) {
	volatile uint64_t *mtimecmp =
        (uint64_t *) (RV_MTIMECMP_ADDR);
    *mtimecmp = *mtimecmp;
}

#else

// 32-Bit accesss

static inline uint64_t read_time_csr(void) {
	register uint32_t highA, highB, low;
	volatile uint32_t *mtimel =
        (uint32_t *) (RV_MTIME_ADDR);
    volatile uint32_t *mtimeh =
       (uint32_t *) (RV_MTIME_ADDR + 4);
	do {
		highA = *mtimeh;
		low = *mtimel;
		highB = *mtimeh;
	} while(highA != highB);
	return ((uint64_t)highA << 32) | low;
}

static inline void write_mtimecmp_csr(uint64_t cmpVal) {
	volatile uint32_t *mtimecmp =
        (uint32_t *) (RV_MTIMECMP_ADDR);
        
	mtimecmp[0] = 0xFFFFFFFFUL;
	mtimecmp[1] = (uint32_t)(cmpVal >> 32);
	mtimecmp[0] = (uint32_t)(cmpVal);
}

static inline void set_max_mtimecmp(void) {
	volatile uint32_t *mtimecmp =
        (uint32_t *) (RV_MTIMECMP_ADDR);
    mtimecmp[0] = 0xFFFFFFFFUL;
	mtimecmp[1] = 0xFFFFFFFFUL;
}

static inline uint64_t read_mtimecmp(void) {
	volatile uint32_t *mtimecmp =
        (uint32_t *) (RV_MTIMECMP_ADDR);
    return ((uint64_t)mtimecmp[1] << 32) | mtimecmp[0];
}

static inline void clear_mtip(void) {
	volatile uint32_t *mtimecmp =
        (uint32_t *) (RV_MTIMECMP_ADDR);
    mtimecmp[1] = mtimecmp[1];
}

#endif



/**
 * @brief   Save reference to the timer callback
 */
static timer_cb_t isr_cb;

/**
 * @brief   Save argument for the callback
 */
static void *isr_arg;

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    /* Using RISC-V built in timer (64bit value) */
    if (dev != 0) {
        return -1;
    }

    if (freq != RV_CORE_TIMER_FREQ) {
        return -1;
    }

    /* Save timer callback and arg */
    isr_cb = cb;
    isr_arg = arg;

    /* No other configuration */
    return 0;
}

int timer_set(tim_t dev, int channel, timer_val_t timeout)
{
    /* Compute delta for timer */
    uint64_t now = read_time_csr();
    uint64_t then = now + timeout;

    if (dev != 0 || channel != 0) {
        return -1;
    }

    /* Avoid spurious timer intr */
    clear_csr(mie, MIP_MTIP);

    /* New intr time */
	write_mtimecmp_csr(then);

    /* Re-enalble timer intr */
    set_csr(mie, MIP_MTIP);
    return 0;
}

int timer_set_absolute(tim_t dev, int channel, timer_val_t value)
{

    if (dev != 0 || channel != 0) {
        return -1;
    }

    /* Avoid spurious timer intr */
    clear_csr(mie, MIP_MTIP);

    write_mtimecmp_csr(value);

    /* Re-enable timer intr */
    set_csr(mie, MIP_MTIP);
    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (dev != 0 || channel != 0) {
        return -1;
    }

    return 0;
}


timer_val_t timer_read(tim_t dev)
{
    uint64_t now = read_time_csr();

    if (dev != 0) {
        return 0;
    }

    return now;
}

void timer_start(tim_t dev)
{
    if (dev != 0) {
        return;
    }

    /* Timer is continuous running
     * Enable the timer interrupt */
    set_csr(mie, MIP_MTIP);
}

void timer_stop(tim_t dev)
{
    if (dev != 0) {
        return;
    }

    /* Disable the timer interrupt */
    clear_csr(mie, MIP_MTIP);
}

void timer_isr(void)
{
    uint64_t now = read_time_csr();
    uint64_t timecmp = read_mtimecmp();

    /* Clear intr */
    clear_csr(mie, MIP_MTIP);


    while (now < timecmp) {
    	// Spurious timer interrupt should be ignored, but make sure we don't miss the next actual one.
    	now = read_time_csr();
    	if ((now + SPURIOUS_TIMER_ISR_MAX_SPIN) < timecmp) {
			 clear_mtip();
    		 set_csr(mie, MIP_MTIP);
    		 return;
    	}
    }

    /* Set mtimecmp to largest value to clear the interrupt */
    set_max_mtimecmp();

    /* Call timer callback function */
    if (isr_cb) {
        isr_cb(isr_arg, 0);
    }

    /* Reset interrupt */
    set_csr(mie, MIP_MTIP);
}

#endif
