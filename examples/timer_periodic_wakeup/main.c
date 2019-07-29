/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       example application for setting a periodic wakeup
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include "xtimer.h"
#include "timex.h"

/* set interval to 1 second */
#define INTERVAL (1U * US_PER_SEC)

int main(void)
{
    xtimer_ticks32_t last_wakeup = xtimer_now();
    xtimer_ticks64_t now64;

    while(1) {
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        now64 = xtimer_now64();
        printf("slept until %" PRIu64 "\n", xtimer_usec_from_ticks64(now64));
    }

    return 0;
}
