/*
 * Copyright (C) Freie Universität Berlin
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Frederik Haxel <haxel@fzi.de>
 */

#include "lwip/tcpip.h"
#include "lwip/netif/netdev.h"
#include "lwip/netif.h"
#include "netif/lowpan6.h"

#ifdef MODULE_NETDEV_TAP
#include "netdev_tap.h"
#include "netdev_tap_params.h"
#endif

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif

#ifdef MODULE_MRF24J40
#include "mrf24j40.h"
#include "mrf24j40_params.h"
#endif

#ifdef MODULE_SOCKET_ZEP
#include "socket_zep.h"
#include "socket_zep_params.h"
#endif

#ifdef MODULE_ROCKETIF
#include "rocketif.h"
#endif

#include "lwip.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef MODULE_NETDEV_TAP
#define LWIP_NETIF_NUMOF        (NETDEV_TAP_MAX)
#if LWIP_IPV4
static ip4_addr_t ipaddrs[LWIP_NETIF_NUMOF];
static ip4_addr_t netmasks[LWIP_NETIF_NUMOF];
static ip4_addr_t gws[LWIP_NETIF_NUMOF];
#endif
#endif

#ifdef MODULE_AT86RF2XX     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        (sizeof(at86rf2xx_params) / sizeof(at86rf2xx_params[0]))
#endif

#ifdef MODULE_MRF24J40     /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        (sizeof(mrf24j40_params) / sizeof(mrf24j40_params[0]))
#endif

#ifdef MODULE_SOCKET_ZEP   /* is mutual exclusive with above ifdef */
#define LWIP_NETIF_NUMOF        (sizeof(socket_zep_params) / sizeof(socket_zep_params[0]))
#endif

#ifdef MODULE_ROCKETIF
#define LWIP_NETIF_NUMOF (RIF_INTERFACES)
#if LWIP_IPV4
static ip4_addr_t ipaddrs[LWIP_NETIF_NUMOF];
static ip4_addr_t netmasks[LWIP_NETIF_NUMOF];
static ip4_addr_t gws[LWIP_NETIF_NUMOF];
#endif
#endif

#ifdef LWIP_NETIF_NUMOF
static struct netif netif[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_NETDEV_TAP
static netdev_tap_t netdev_taps[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_devs[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_MRF24J40
static mrf24j40_t mrf24j40_devs[LWIP_NETIF_NUMOF];
#endif

#ifdef MODULE_SOCKET_ZEP
static socket_zep_t socket_zep_devs[LWIP_NETIF_NUMOF];
#endif

void lwip_bootstrap(void) {
	/* TODO: do for every eligable netdev */
#ifdef LWIP_NETIF_NUMOF
#ifdef MODULE_NETDEV_TAP

	for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
		netdev_tap_setup(&netdev_taps[i], &netdev_tap_params[i]);

		uint8_t ipv4Addr[] = { 0, 0, 0, 0 };
		getIPv4Address(&netdev_taps[i], ipv4Addr);

#if LWIP_IPV4
		if (ipv4Addr[0] == 0) {
			IP4_ADDR(&ipaddrs[i], 192, 168, 0, (10 + i));
			IP4_ADDR(&netmasks[i], 255, 255, 255, 0);
			IP4_ADDR(&gws[i], 192, 168, 0, 1);
		} else {
			IP4_ADDR(&ipaddrs[i], ipv4Addr[0], ipv4Addr[1], ipv4Addr[2], ipv4Addr[3] + 1);
			IP4_ADDR(&netmasks[i], 255, 255, 255, 248); //
			IP4_ADDR(&gws[i], ipv4Addr[0], ipv4Addr[1], ipv4Addr[2], ipv4Addr[3]);
		}
#endif

		if (netif_add(&netif[i],
#if LWIP_IPV4
				&ipaddrs[i], &netmasks[i], &gws[i],
#endif
				&netdev_taps[i], lwip_netdev_init, tcpip_input) == NULL) {
			DEBUG("Could not add netdev_tap device\n");
			return;
		}
	}
#elif defined(MODULE_MRF24J40)
	for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
		mrf24j40_setup(&mrf24j40_devs[i], &mrf24j40_params[i]);
		if (netif_add(&netif[i], &mrf24j40_devs[i], lwip_netdev_init,
						tcpip_6lowpan_input) == NULL) {
			DEBUG("Could not add mrf24j40 device\n");
			return;
		}
	}
#elif defined(MODULE_AT86RF2XX)
	for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
		at86rf2xx_setup(&at86rf2xx_devs[i], &at86rf2xx_params[i]);
		if (netif_add(&netif[i], &at86rf2xx_devs[i], lwip_netdev_init,
						tcpip_6lowpan_input) == NULL) {
			DEBUG("Could not add at86rf2xx device\n");
			return;
		}
	}
#elif defined(MODULE_SOCKET_ZEP)
    for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
        socket_zep_setup(&socket_zep_devs[i], &socket_zep_params[i]);
        if (netif_add(&netif[i], &socket_zep_devs[i], lwip_netdev_init,
                      tcpip_6lowpan_input) == NULL) {
            DEBUG("Could not add socket_zep device\n");
            return;
        }
    }
#elif defined(MODULE_ROCKETIF)
	for (unsigned i = 0; i < LWIP_NETIF_NUMOF; i++) {
#if LWIP_IPV4
		IP4_ADDR(&ipaddrs[i], 192, 168, 0, (10 + i));
		IP4_ADDR(&netmasks[i], 255, 255, 255, 0);
		IP4_ADDR(&gws[i], 192, 168, 0, 1);
#endif
		rocketif_t *newIf = rocketif_create(i);
		if (netif_add(&netif[i],
#if LWIP_IPV4
						&ipaddrs[i], &netmasks[i], &gws[i],
#endif
						newIf, lwip_netdev_init, tcpip_input) == NULL) {
			DEBUG("Could not add rocketif device\n");
			return;
		}
	}
#endif
	if (netif[0].state != NULL) {
		/* state is set to a netdev_t in the netif_add() functions above */
		netif_set_default(&netif[0]);
	}
#endif
	/* also allow for external interface definition */
	tcpip_init(NULL, NULL);
}

#if defined(MODULE_ROCKETIF) || defined(MODULE_NETDEV_TAP)
 #if LWIP_IPV4
void setIPv4Address(uint8_t netif_id, const ip4_addr_t* addr, const ip4_addr_t* netmask, const ip4_addr_t* gw) {
	if (netif_id < LWIP_NETIF_NUMOF) {
		ipaddrs[netif_id] = *addr;
		netmasks[netif_id] = *netmask;
		gws[netif_id] = *gw;
		netif_set_addr(&netif[netif_id],&ipaddrs[netif_id], &netmasks[netif_id], &gws[netif_id]);
	} else {
		DEBUG("Could not set ipv4 address\n");
	}
}
 #endif
#endif

/** @} */
