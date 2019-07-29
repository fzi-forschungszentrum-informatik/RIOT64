/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 *
 * @author      Frederik Haxel <haxel@fzi.de>
 * @author      Leon Hielscher <hielscher@fzi.de>
 *
 */

#ifdef MODULE_ROCKETIF

#include "rocketif.h"
#include "cpu_conf.h"
#include "net/gnrc/netif/conf.h"
#include "net/gnrc/netif/ethernet.h"

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ROCKETIF_MAC_STACKSIZE     (THREAD_STACKSIZE_DEFAULT)
#define ROCKETIF_MAC_PRIO          (GNRC_NETIF_PRIO)


static char _rocketif_stacks[RIF_INTERFACES][ROCKETIF_MAC_STACKSIZE];

void auto_init_rocketif(void)
{
    for (unsigned i = 0; i < RIF_INTERFACES; i++) {
        rocketif_t* newIf;
        newIf = rocketif_create(i);
        gnrc_netif_ethernet_create (_rocketif_stacks[i],
                                     ROCKETIF_MAC_STACKSIZE, ROCKETIF_MAC_PRIO,
                                     "rocketif",
                                     (netdev_t *)newIf);
    }
}
#else
typedef int dont_be_pedantic;
#endif 
