/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup        cpu_rocketchip64 RocketChip64
 * @{
 *
 * @file
 * @brief           CPU specific configuration options
 *
 * @author      Leon Hielscher <hielscher@fzi.de>
 * @}
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

/**
 * @name Configuration of default stack sizes
 * @{
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF   (1024)
#endif
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (8192)
#endif
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE           (1024)
#endif
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
