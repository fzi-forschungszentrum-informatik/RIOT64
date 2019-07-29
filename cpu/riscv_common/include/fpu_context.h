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
 * @brief       FPU context frame for RISC-V
 * @author      Leon Hielscher <hielscher@fzi.de>
 * @}
 */

#ifndef FPU_CONTEXT_H
#define FPU_CONTEXT_H

#ifdef USE_LAZY_FPU_CONTEXT

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef RV_FPU_DOUBLE_PRECISION
#ifdef RV_FPU_SINGLE_PRECISION
#error Double and single precision FPU must not be defined at the same time.
#endif
#endif

#ifndef RV_FPU_DOUBLE_PRECISION
#ifndef RV_FPU_SINGLE_PRECISION
#error Double or single precision FPU not defined.
#endif
#endif

#ifdef RV_FPU_DOUBLE_PRECISION
#if __riscv_flen != 64
#error "RV_FPU_DOUBLE_PRECISION does not match compiler ABI."
#endif
#define RV_FPU_REG_ALIGN 8
#define RV_FPU_REG_TYPE uint64_t
#define RV_FLOAD_IASM(reg) "fld " #reg
#define RV_FSTORE_IASM(reg) "fsd " #reg

#define ft0_OFFSET_IASM "0"
#define ft1_OFFSET_IASM "8"
#define ft2_OFFSET_IASM "16"
#define ft3_OFFSET_IASM "24"
#define ft4_OFFSET_IASM "32"
#define ft5_OFFSET_IASM "40"
#define ft6_OFFSET_IASM "48"
#define ft7_OFFSET_IASM "56"
#define ft8_OFFSET_IASM "64"
#define ft9_OFFSET_IASM "72"
#define ft10_OFFSET_IASM "80"
#define ft11_OFFSET_IASM "88"
#define fs0_OFFSET_IASM "96"
#define fs1_OFFSET_IASM "104"
#define fs2_OFFSET_IASM "112"
#define fs3_OFFSET_IASM "120"
#define fs4_OFFSET_IASM "128"
#define fs5_OFFSET_IASM "136"
#define fs6_OFFSET_IASM "144"
#define fs7_OFFSET_IASM "152"
#define fs8_OFFSET_IASM "160"
#define fs9_OFFSET_IASM "168"
#define fs10_OFFSET_IASM "176"
#define fs11_OFFSET_IASM "184"
#define fa0_OFFSET_IASM "192"
#define fa1_OFFSET_IASM "200"
#define fa2_OFFSET_IASM "208"
#define fa3_OFFSET_IASM "216"
#define fa4_OFFSET_IASM "224"
#define fa5_OFFSET_IASM "232"
#define fa6_OFFSET_IASM "240"
#define fa7_OFFSET_IASM "248"
#define fcsr_OFFSET_IASM "256"


#endif

#ifdef RV_FPU_SINGLE_PRECISION
#if __riscv_flen != 32
#error "RV_FPU_SINGLE_PRECISION does not match compiler ABI."
#endif
#define RV_FPU_REG_ALIGN 4
#define RV_FPU_REG_TYPE uint32_t
#define RV_FLOAD_IASM(reg) "flw " #reg
#define RV_FSTORE_IASM(reg) "fsw " #reg

#define ft0_OFFSET_IASM "0"
#define ft1_OFFSET_IASM "4"
#define ft2_OFFSET_IASM "8"
#define ft3_OFFSET_IASM "12"
#define ft4_OFFSET_IASM "16"
#define ft5_OFFSET_IASM "20"
#define ft6_OFFSET_IASM "24"
#define ft7_OFFSET_IASM "28"
#define ft8_OFFSET_IASM "32"
#define ft9_OFFSET_IASM "36"
#define ft10_OFFSET_IASM "40"
#define ft11_OFFSET_IASM "44"
#define fs0_OFFSET_IASM "48"
#define fs1_OFFSET_IASM "52"
#define fs2_OFFSET_IASM "56"
#define fs3_OFFSET_IASM "60"
#define fs4_OFFSET_IASM "64"
#define fs5_OFFSET_IASM "68"
#define fs6_OFFSET_IASM "72"
#define fs7_OFFSET_IASM "76"
#define fs8_OFFSET_IASM "80"
#define fs9_OFFSET_IASM "84"
#define fs10_OFFSET_IASM "88"
#define fs11_OFFSET_IASM "92"
#define fa0_OFFSET_IASM "96"
#define fa1_OFFSET_IASM "100"
#define fa2_OFFSET_IASM "104"
#define fa3_OFFSET_IASM "108"
#define fa4_OFFSET_IASM "112"
#define fa5_OFFSET_IASM "116"
#define fa6_OFFSET_IASM "120"
#define fa7_OFFSET_IASM "124"
#define fcsr_OFFSET_IASM "128"
#endif

#define FPU_CONTEXT_ALINGMENT 	RV_FPU_REG_ALIGN

struct __attribute__((packed, aligned(FPU_CONTEXT_ALINGMENT))) fpu_context {
    RV_FPU_REG_TYPE ft0;
    RV_FPU_REG_TYPE ft1;
    RV_FPU_REG_TYPE ft2;
    RV_FPU_REG_TYPE ft3;
    RV_FPU_REG_TYPE ft4;
    RV_FPU_REG_TYPE ft5;
    RV_FPU_REG_TYPE ft6;
    RV_FPU_REG_TYPE ft7;
    RV_FPU_REG_TYPE ft8;
    RV_FPU_REG_TYPE ft9;
    RV_FPU_REG_TYPE ft10;
    RV_FPU_REG_TYPE ft11;
    RV_FPU_REG_TYPE fs0;
    RV_FPU_REG_TYPE fs1;
    RV_FPU_REG_TYPE fs2;
    RV_FPU_REG_TYPE fs3;
    RV_FPU_REG_TYPE fs4;
    RV_FPU_REG_TYPE fs5;
    RV_FPU_REG_TYPE fs6;
    RV_FPU_REG_TYPE fs7;
    RV_FPU_REG_TYPE fs8;
    RV_FPU_REG_TYPE fs9;
    RV_FPU_REG_TYPE fs10;
    RV_FPU_REG_TYPE fs11;
    RV_FPU_REG_TYPE fa0;
    RV_FPU_REG_TYPE fa1;
    RV_FPU_REG_TYPE fa2;
    RV_FPU_REG_TYPE fa3;
    RV_FPU_REG_TYPE fa4;
    RV_FPU_REG_TYPE fa5;
    RV_FPU_REG_TYPE fa6;
    RV_FPU_REG_TYPE fa7;
    unsigned long fcsr;
};


#ifdef __cplusplus
}
#endif

#endif // USE_LAZY_FPU_CONTEXT

#endif /* FPU_CONTEXT_H */
