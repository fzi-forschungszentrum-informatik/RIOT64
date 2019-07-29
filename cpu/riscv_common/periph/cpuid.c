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
 * @file        cpuid.c
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Ken Rabold
 * @}
 */


#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"
#include "cpu.h"
#include "vendor/encoding.h"
#include "vendor/platform.h"

void cpuid_get(void *id)
{
    rvxlen_unsigned_t result[3];

    /* Read RISC-V CSRs for CPU ID */
    result[0] = read_csr(mvendorid);
    result[1] = read_csr(marchid);
    result[2] = read_csr(mimpid);

    memcpy(id, &result[0], CPUID_LEN);
}
