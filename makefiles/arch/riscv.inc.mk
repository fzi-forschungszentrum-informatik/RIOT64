# Target architecture for the build.
export TARGET_ARCH ?= riscv-none-embed

ifeq ($(RVXLEN), RV32)  
    export QEMU=qemu-system-riscv32
else
ifeq ($(RVXLEN), RV64)
    export QEMU=qemu-system-riscv64
else
$(error Unknown target architecture $(RVXLEN))
endif
endif

# define build specific options
CFLAGS_CPU   = -march=$(RVARCH) -mabi=$(RVABI) -mcmodel=$(RVMED) -msmall-data-limit=8
CFLAGS_LINK  = -nostartfiles -ffunction-sections -fdata-sections
#CFLAGS_DBG  ?= -g3 -Og -DRV_PANIC_PRINT

#Allow GNU extensions
CFLAGS += -std=gnu11 -U_POSIX_C_SOURCE -D_POSIX_C_SOURCE=0

CFLAGS_OPT  ?= -Os


ifneq (,$(findstring d,$(RVARCH)))
    CFLAGS_CPU   += -DUSE_LAZY_FPU_CONTEXT -DRV_FPU_DOUBLE_PRECISION
else
ifneq (,$(findstring g,$(RVARCH)))
    CFLAGS_CPU   += -DUSE_LAZY_FPU_CONTEXT -DRV_FPU_DOUBLE_PRECISION
else
ifneq (,$(findstring f,$(RVARCH)))
    CFLAGS_CPU   += -DUSE_LAZY_FPU_CONTEXT -DRV_FPU_SINGLE_PRECISION
endif
endif
endif

include $(RIOTCPU)/riscv_common/Makefile.include

export LINKFLAGS += -L$(RIOTBOARD)/$(BOARD)/ldscripts
export LINKFLAGS += -L$(RIOTCPU)/riscv_common/ldscripts
export LINKFLAGS += -L$(RIOTCPU)/$(CPU)/ldscripts
export LINKFLAGS += -lg -lgloss
export LINKER_SCRIPT ?= $(CPU_MODEL).ld
export LINKFLAGS += -T$(LINKER_SCRIPT)

# export compiler flags
export CFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG) $(CFLAGS_OPT) $(CFLAGS_LINK)
# export assmebly flags
export ASFLAGS += $(CFLAGS_CPU) $(CFLAGS_DBG)
# export linker flags
ifeq (,$(NEWLIB_CUSTOM))
export LINKFLAGS += $(CFLAGS_CPU) $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT) -Wl,--gc-sections
else
export LINKFLAGS += $(CFLAGS_CPU) -Xlinker -lc $(CFLAGS_LINK) $(CFLAGS_DBG) $(CFLAGS_OPT) -Wl,--gc-sections
endif

export QEMU_FLAGS += -gdb tcp::3333 -serial stdio -display none
export TESTRUNNER_TARGET=qemu


