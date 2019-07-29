/*
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 *
  * @author      Leon Hielscher <hielscher@fzi.de>
 *
 */

#ifndef CPU_RV64_INCLUDE_VENDOR_CPU_PERFMON_H_
#define CPU_RV64_INCLUDE_VENDOR_CPU_PERFMON_H_

#define CPU_PERFMON_INST_EXCEPT			(1 << 8)
#define CPU_PERFMON_INST_ILOAD			(1 << 9)
#define CPU_PERFMON_INST_ISTORE			(1 << 10)
#define CPU_PERFMON_INST_ATOMIC			(1 << 11)
#define CPU_PERFMON_INST_SYSTEM			(1 << 12)
#define CPU_PERFMON_INST_IARITH			(1 << 13)
#define CPU_PERFMON_INST_CONDB			(1 << 14)
#define CPU_PERFMON_INST_JAL			(1 << 15)
#define CPU_PERFMON_INST_JALR			(1 << 16)
#define CPU_PERFMON_INST_IMUL			(1 << 17)
#define CPU_PERFMON_INST_IDIV			(1 << 18)
#define CPU_PERFMON_INST_FLOAD			(1 << 19)
#define CPU_PERFMON_INST_FSTORE			(1 << 20)
#define CPU_PERFMON_INST_FADD			(1 << 21)
#define CPU_PERFMON_INST_FMUL			(1 << 22)
#define CPU_PERFMON_INST_FFMA			(1 << 23)
#define CPU_PERFMON_INST_FDIVSQRT		(1 << 24)
#define CPU_PERFMON_INST_FOTHER			(1 << 25)

#define CPU_PERFMON_ARCH_LUSEILOCK		(1 << 8)
#define CPU_PERFMON_ARCH_LLATILOCK		(1 << 9)
#define CPU_PERFMON_ARCH_CSRRDILOCK		(1 << 10)
#define CPU_PERFMON_ARCH_ICACHEBUSY		(1 << 11)
#define CPU_PERFMON_ARCH_DCACHEBUSY		(1 << 12)
#define CPU_PERFMON_ARCH_BDIRMISPRED	(1 << 13)
#define CPU_PERFMON_ARCH_BTGTMISPRED	(1 << 14)
#define CPU_PERFMON_ARCH_PFLUSHCSRW		(1 << 15)
#define CPU_PERFMON_ARCH_PFLUSHOTHER	(1 << 16)
#define CPU_PERFMON_ARCH_IMULILOCK		(1 << 17)
#define CPU_PERFMON_ARCH_FPILOCK		(1 << 18)

#define CPU_PERFMON_MEM_ICACHEMISS		(1 << 8)
#define CPU_PERFMON_MEM_DCAHCEMISSIO	(1 << 9)
#define CPU_PERFMON_MEM_DCACHEWB		(1 << 10)
#define CPU_PERFMON_MEM_ITLBMISS		(1 << 11)
#define CPU_PERFMON_MEM_DTLBMISS		(1 << 12)


#define CPU_GET_CYCLE() ({ unsigned long __tmp; \
  __asm__ volatile ("csrr %0, mcycle" : "=r"(__tmp)); \
  __tmp; })

#define CPU_GET_INSTRET() ({ unsigned long __tmp; \
  __asm__ volatile ("csrr %0, minstret" : "=r"(__tmp)); \
  __tmp; })


#define CPU_GET_PERFMON(n) ({ unsigned long __tmp; \
  __asm__ volatile ("csrr %0, mhpmcounter"#n "" : "=r"(__tmp)); \
  __tmp; })

#define CPU_RESET_PERFMON(n) __asm__ volatile ("csrw mhpmcounter"#n ", zero")
#define CPU_DISABLE_PERFMON(n) __asm__ volatile ("csrw mhpmevent"#n ", zero")


#define CPU_ENABLE_PERFMON_INST(n, flags) ({ register unsigned long __tmp = flags | 0;  \
  __asm__ volatile ("csrw mhpmevent"#n ", %0" : : "r"(__tmp));})

#define CPU_ENABLE_PERFMON_ARCH(n, flags) ({ register unsigned long __tmp = flags | 1;  \
  __asm__ volatile ("csrw mhpmevent"#n ", %0" : : "r"(__tmp));})

#define CPU_ENABLE_PERFMON_MEM(n, flags) ({ register unsigned long __tmp = flags | 2;  \
  __asm__ volatile ("csrw mhpmevent"#n ", %0" : : "r"(__tmp));})

#endif /* CPU_RV64_INCLUDE_VENDOR_CPU_PERFMON_H_ */
