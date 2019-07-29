/*
 * Copyright (C) 2017 Ken Rabold
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_riscv_common RISC-V CPU Common
 * @ingroup     cpu
 * @brief       Common implementations and headers for RISC-V compatible CPUs
 * @{
 *
 * @file
 * @brief       Basic definitions for the RISC-V common module
 *
 * When ever you want to do something hardware related, that is accessing MCUs
 * registers, just include this file. It will then make sure that the MCU
 * specific headers are included.
 *
 * @author      Ken Rabold
 * @author      Leon Hielscher <hielscher@fzi.de>
 * @}
 */

#ifndef CPU_H
#define CPU_H


#ifdef __riscv

#if (__riscv_xlen == 64) || (__riscv_xlen == 32)
// Size of (unsigned) long equals xlen according to ABI 
typedef long rvxlen_signed_t;
typedef unsigned long rvxlen_unsigned_t;
#define RV_XLEN_BYTES (__riscv_xlen / 8)
#if __SIZEOF_POINTER__ != (__riscv_xlen / 8)
#error "Incompatible RISC-V ABI."
#endif
#else
#error "Incompatible or undefined RISC-V XLEN."
#endif

#else

#error "Archicetcure missmatch! Expected RISC-V."

#endif

/**
 * @brief Print format for depending on arch
 */
#if (__riscv_xlen == 64)
#define PRIxRVXLEN "016lx"
#else
#define PRIxRVXLEN "08lx"
#endif



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialization of the CPU
 */
void cpu_init(void);

/**
 * @brief   Initialization of the FPU
 */
void fpu_init(void);

/**
 * @brief   Print the last instruction's address
 *
 * @todo:   Not supported
 */
static inline void cpu_print_last_instruction(void)
{
    /* This function must exist else RIOT won't compile */
}

/**
 * @brief   Initialization of the Newlib-nano stub
 */
void nanostubs_init(void);


#ifdef RV_PANIC_PRINT
void panic_arch(void);
#endif

/**
 * @brief   Enable the FPU
 */
#define RV_FPU_ENABLE()  do {                   \
    rvxlen_unsigned_t _tmp_fpuFlag = MSTATUS_FS;     \
    __asm__ __volatile__ ( "csrs mstatus, %0" : "+r"(_tmp_fpuFlag) );     \
} while(0)

/**
 * @brief   Disable the FPU
 */
#define RV_FPU_DISABLE()  do {                   \
    rvxlen_unsigned_t _tmp_fpuFlag = MSTATUS_FS;     \
    __asm__ __volatile__ ( "csrc mstatus, %0" : "+r"(_tmp_fpuFlag) );     \
} while(0)

/**
 * @brief    Check if the FPU is dirty
 */
#define RV_IS_FPU_DIRTY()  ((read_csr(mstatus) & MSTATUS_SD) != 0)

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
/** @} */
