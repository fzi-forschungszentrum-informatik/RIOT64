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

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif


#include "periph_cpu.h"

#define RV_MCYCLE_FREQ		(150000000ul)
#define RV_CORE_TIMER_FREQ	(1000000ul)


/**
 * @name    UART configuration
 *
 * @{
 */
#define UART_NUMOF                  (1)
#define UART0_RX_INTR_PRIORITY      (2)
#define UART1_RX_INTR_PRIORITY      (2)
/** @} */

/**
 * @name    Core Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK             RV_MCYCLE_FREQ
/*
 * #define CLOCK_CORECLOCK           (20000000ul)
 * #define CLOCK_CORECLOCK           (27000000ul)
 * #define CLOCK_CORECLOCK           (38400000ul)
 */
/** @} */

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (64)
#define XTIMER_HZ                   RV_CORE_TIMER_FREQ
/** @} */


#define TIMER_NUMOF                 (1U) // We only have the system timer

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
