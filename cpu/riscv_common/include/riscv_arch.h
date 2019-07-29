/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_riscv_common
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       Common Macros for RISC-V CPUs
 *
 * @author      Leon Hielscher <hielscher@fzi.de>
 * @}
 */
 
#ifndef RISCV_ARCH_H
#define RISCV_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  RISC-V FENCE.I instruction
 */
#define RV_ASM_FENCE_I() __asm__ volatile ("fence.i")

/**
 * @brief RISC-V WFI instruction 
 */
#define RV_ASM_WFI() __asm__ volatile("wfi")

/**
 * @brief RISC-V FENCE IORW, IORW  instruction 
 */
#define RV_ASM_FENCE_IORW() __asm__ volatile ("fence iorw, iorw")

/**
 * @brief RISC-V MRET instruction 
 */
#define RV_ASM_MRET() __asm__ volatile("mret")

/**
 * @brief Default state of mstatus register 
 */
#define MSTATUS_DEFAULT     (MSTATUS_MPP | MSTATUS_MPIE)

/**
 * @brief Suffix for 32 bit RISC-V instructions
 */
#define INSN_32BIT_MASK 0b11

/**
 * @brief  
 */
//! /brief RISC-V Float instruction suffixes
//@{ 
#define INSN_FLOAD_SUFFIX 0b00001
#define INSN_FSTORE_SUFFIX 0b01001
#define INSN_FOPR_SUFFIX 0b10100
#define INSN_FMADD_SUFFIX 0b10000
#define INSN_FMSUB_SUFFIX 0b10001
#define INSN_FNMSUB_SUFFIX 0b10010
#define INSN_FNMADD_SUFFIX 0b10011

/**
 * @brief RISC-V System Insturction suffix
 */
#define INSN_SYSTEM_SUFFIX 0b11100

//! /brief  RISC-V compressed intruction masks 
//@{ 
#define INSNC_MASK 0xE003
#define INSNC_FLD		0b0010000000000000
#define INSNC_FSW		0b1010000000000000
#define INSNC_FLDSP		0b0010000000000010
#define INSNC_FSDSP  		0b1010000000000010
//@} 

/**
 * @brief RISC-V trap cause interrupt
 */
#ifndef MCAUSE_INT
#if (__riscv_xlen == 64)
#define MCAUSE_INT	(0x8000000000000000UL)
#else
#define MCAUSE_INT  (0x80000000)
#endif
#endif

/**
 * RISC-V trap cause mask
 */
#ifndef MCAUSE_CAUSE
#if (__riscv_xlen == 64)
#define MCAUSE_CAUSE	(0x7FFFFFFFFFFFFFFFUL)
#else
#define MCAUSE_CAUSE  (0x7FFFFFFF)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* RISCV_ARCH_H */
