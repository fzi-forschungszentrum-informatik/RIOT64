include ../Makefile.tests_common

# some boards have not enough rom and/or ram
BOARD_BLACKLIST += nucleo-f031k6 nucleo-f042k6 nucleo-l031k6 pic32-clicker
BOARD_INSUFFICIENT_MEMORY += arduino-duemilanove arduino-nano arduino-uno

# override PRNG if desired (see sys/random for alternatives)
# USEMODULE += prng_minstd

USEMODULE += fmt
USEMODULE += random
USEMODULE += shell
USEMODULE += xtimer

FEATURES_OPTIONAL += periph_hwrng

TEST_ON_CI_WHITELIST += native native64

include $(RIOTBASE)/Makefile.include
