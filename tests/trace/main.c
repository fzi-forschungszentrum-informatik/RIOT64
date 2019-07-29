/*
 * Copyright (C) 2016 Freie Universität Berlin
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests od module.
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "trace.h"

void subFunction(void) {
    trace_print();
}

int main(void)
{
    printf("TRACE_SIZE: %u\n", TRACE_SIZE);
    subFunction();
    return 0;
}
