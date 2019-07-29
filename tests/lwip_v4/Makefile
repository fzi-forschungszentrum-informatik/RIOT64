include ../Makefile.tests_common

#TODO: Re-enable for 32 and 64 bit.

# lwIP's memory management doesn't seem to work on non 32-bit platforms at the
# moment.
BOARD_BLACKLIST := arduino-uno arduino-duemilanove arduino-mega2560 chronos \
                   msb-430 msb-430h telosb waspmote-pro wsn430-v1_3b \
                   wsn430-v1_4 z1 jiminy-mega256rfr2 mega-xplained \
		   native native64 rocketchip-generic64bit nrf52840dk # for now disabled
BOARD_INSUFFICIENT_MEMORY := airfy-beacon hifive1 nrf6310 nucleo-f031k6 nucleo-f042k6 \
                             nucleo-l031k6 nucleo-f030r8 nucleo-f334r8 nucleo-l053r8 \
                             stm32f0discovery yunjia-nrf51822

# including lwip_ipv6_mld would currently break this test on at86rf2xx radios
USEMODULE += lwip lwip_ethernet lwip_sock_ip lwip_arp lwip_netdev
USEMODULE += lwip_udp lwip_sock_udp
USEMODULE += lwip_tcp lwip_sock_tcp
USEMODULE += lwip_ipv4 ipv4_addr
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += od
USEMODULE += netdev_default

ifeq ($(BOARD),native)
  USEMODULE += lwip_ethernet
endif

ifeq ($(BOARD),rocketchip-generic64bit)
  USEMODULE += rocketif
endif



# Test only implemented for native
ifeq ($(BOARD),native)
test:
	./tests/01-run.py
endif

include $(RIOTBASE)/Makefile.include
