/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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
 * @brief       Test for lwIP
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This test application tests the lwIP package.
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "common.h"
#include "lwip/netif.h"
#include "net/ipv4/addr.h"
#include "lwip.h"
#include "shell.h"

static int ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    for (struct netif *iface = netif_list; iface != NULL; iface = iface->next) {
        printf("%s_%02u: ", iface->name, iface->num);
#ifdef MODULE_LWIP_IPV6
        char addrstr[IPV6_ADDR_MAX_STR_LEN];
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (!ipv6_addr_is_unspecified((ipv6_addr_t *)&iface->ip6_addr[i])) {
                printf(" inet6 %s\n", ipv6_addr_to_str(addrstr, (ipv6_addr_t *)&iface->ip6_addr[i],
                                                       sizeof(addrstr)));
            }
        }
#endif
#ifdef MODULE_LWIP_IPV4
        char addrstr[IPV4_ADDR_MAX_STR_LEN];
		printf(" inet4 %s", ipv4_addr_to_str(addrstr, (ipv4_addr_t *)&iface->ip_addr,
												   sizeof(addrstr)));
		printf("; netmask %s", ipv4_addr_to_str(addrstr, (ipv4_addr_t *)&iface->netmask,
												   sizeof(addrstr)));
		printf("; gw %s\n", ipv4_addr_to_str(addrstr, (ipv4_addr_t *)&iface->gw,
												   sizeof(addrstr)));
#endif
        puts("");
    }
    return 0;
}

int set_ipv4(int argc, char **argv)
{
    if (argc < 4) {
        printf("usage: %s ipv4_addr netmask gateway\n", argv[0]);
        return 1;
    }

    ip4_addr_t addr, netmask, gw;
	if (ipv4_addr_from_str((ipv4_addr_t *)&addr, argv[1]) == NULL) {
		puts("Error: unable to parse address");
		return 1;
	}
	if (ipv4_addr_from_str((ipv4_addr_t *)&netmask, argv[2]) == NULL) {
		puts("Error: unable to parse netmask");
		return 1;
	}
	if (ipv4_addr_from_str((ipv4_addr_t *)&gw, argv[3]) == NULL) {
		puts("Error: unable to parse gw");
		return 1;
	}
	setIPv4Address(0, &addr, &netmask, &gw);

	printf("IPv4 set!\n");
	return 0;
}

static const shell_command_t shell_commands[] = {
#ifdef MODULE_SOCK_IP
    { "ip", "Send IP packets and listen for packets of certain type", ip_cmd },
#endif
#ifdef MODULE_SOCK_TCP
    { "tcp", "Send TCP messages and listen for messages on TCP port", tcp_cmd },
#endif
#ifdef MODULE_SOCK_UDP
    { "udp", "Send UDP messages and listen for messages on UDP port", udp_cmd },
#endif
    { "ifconfig", "Shows assigned IPv4/v6 addresses", ifconfig },
    { "ipv4", "Set IPv4 address", set_ipv4 },
    { NULL, NULL, NULL }
};
static char line_buf[SHELL_DEFAULT_BUFSIZE];

int main(void)
{
    puts("RIOT lwip test application");
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
