# exclude submodule sources from *.c wildcard source selection
SRC := $(filter-out mbox.c msg.c thread_flags.c,$(wildcard *.c))

# enable submodules
SUBMODULES := 1

.PHONY: .TOUCH
all: .TOUCH
.TOUCH: 
	touch $(RIOTBASE)/core/kernel_init.c

include $(RIOTBASE)/Makefile.base


