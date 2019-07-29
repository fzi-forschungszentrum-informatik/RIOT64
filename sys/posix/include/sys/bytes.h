/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  posix_sockets
 */
/**
 * @{
 *
 * @file
 * @brief   System-internal byte operations.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SYS_BYTES_H
#define SYS_BYTES_H

#include <stddef.h>
#include <limits.h>
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __socklen_t_defined
#if SIZE_MAX < UINT32_MAX
typedef size_t socklen_t;           /**< socket address length */
#else
typedef uint32_t socklen_t;
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_BYTES_H */
/** @} */
