/*
 * Copyright (C) 2017 JP Bonn, Ken Rabold
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#if __riscv_xlen == 64
#define REG_L			ld
#define REG_S			sd
#define XLEN_BYTES		8
#elif __riscv_xlen == 32
#define REG_L			lw
#define REG_S			sw
#define XLEN_BYTES		4
#else
#error "Unexpected __riscv_xlen"
#endif

    .section .init
    .globl _start
    .type _start,@function

_start:
    .cfi_startproc
    .cfi_undefined ra
.option push
.option norelax
    la gp, __global_pointer$
.option pop
    la sp, _sp


    /* Load data section */
    la a0, _data_lma
    la a1, _data
    la a2, _edata
    bgeu a1, a2, 2f
1:
    REG_L t0, (a0)
    REG_S t0, (a1)
    addi a0, a0, XLEN_BYTES
    addi a1, a1, XLEN_BYTES
    bltu a1, a2, 1b
2:

    /* Clear bss section */
    la a0, __bss_start
    la a1, _end
    bgeu a0, a1, 2f
1:
    REG_S zero, (a0)
    addi a0, a0, XLEN_BYTES
    bltu a0, a1, 1b
2:

    /* Call global constructors */
    la a0, __libc_fini_array
    call atexit
    call __libc_init_array


    /* Initialize board and start kernel */
    call board_init
    call kernel_init

    /* Loop forever (should never get here) */
1:
    j 1b

    .cfi_endproc
