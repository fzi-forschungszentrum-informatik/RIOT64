 /*
 * Copyright (C) 2017 JP Bonn
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 #include "vendor/encoding.h" 
 #include "context_frame.h"
 
#if __riscv_xlen == 64
#define REG_L			ld
#define REG_S			sd
#define XLEN_BYTES		8
#define NAT_ALIGN_LG	3 
#define IRQ_SW_CAUSE	((1 << 63) + 3) 
#elif __riscv_xlen == 32
#define REG_L			lw
#define REG_S			sw
#define XLEN_BYTES		4
#define NAT_ALIGN_LG	2
#define IRQ_SW_CAUSE	((1 << 31) + 3) 
#else
#error "Unexpected __riscv_xlen"
#endif

/* from platform.h TODO:fix this hard code.... */
CLINT_CTRL_ADDR = 0x02000000

  .section      .text.entry
  .align NAT_ALIGN_LG
  .global trap_entry

#ifdef USE_LAZY_FPU_CONTEXT
  .extern fpu_owner_ctx 
#endif

trap_entry:
    /*
     * Save all regs on the currently active stack.
     * This coule be the active thread's stack,
     * or if no thread is active, it is saved on ISR stack
     * (if initial startup) or on the deactivated threads
     * stack (in the case of thread exit).  In the latter
     * two cases the stack is just abandoned.
     */
    addi sp, sp, -CONTEXT_FRAME_SIZE

    REG_S s0, s0_OFFSET(sp)
    REG_S s1, s1_OFFSET(sp)
    REG_S s2, s2_OFFSET(sp)
    REG_S s3, s3_OFFSET(sp)
    REG_S s4, s4_OFFSET(sp)
    REG_S s5, s5_OFFSET(sp)
    REG_S s6, s6_OFFSET(sp)
    REG_S s7, s7_OFFSET(sp)
    REG_S s8, s8_OFFSET(sp)
    REG_S s9, s9_OFFSET(sp)
    REG_S s10, s10_OFFSET(sp)
    REG_S s11, s11_OFFSET(sp)
    REG_S ra, ra_OFFSET(sp)
    REG_S tp, tp_OFFSET(sp)
    REG_S t0, t0_OFFSET(sp)
    REG_S t1, t1_OFFSET(sp)
    REG_S t2, t2_OFFSET(sp)
    REG_S t3, t3_OFFSET(sp)
    REG_S t4, t4_OFFSET(sp)
    REG_S t5, t5_OFFSET(sp)
    REG_S t6, t6_OFFSET(sp)
    REG_S a0, a0_OFFSET(sp)
    REG_S a1, a1_OFFSET(sp)
    REG_S a2, a2_OFFSET(sp)
    REG_S a3, a3_OFFSET(sp)
    REG_S a4, a4_OFFSET(sp)
    REG_S a5, a5_OFFSET(sp)
    REG_S a6, a6_OFFSET(sp)
    REG_S a7, a7_OFFSET(sp) 


    /* Get the interrupt cause */
    csrr a0, mcause

    /* Save active thread stack pointer in a callee save register */
    mv s1, sp

    /* Switch to ISR stack.  Interrupts are not nested so use fixed
     *  starting address and just abandon stack when finished. */
    la  sp, _sp
    addi sp, sp, -XLEN_BYTES

    /*  Is it a software interrupt? */
    li t0, IRQ_SW_CAUSE
    beq a0, t0, context_switch

    /*  Call handle_trap with MCAUSE register value as arg */
    jal handle_trap

    /*  See if a context switch was requested by the ISR */
    #if __riscv_xlen == 64
    lwu a0, sched_context_switch_request
    #else
    lw a0, sched_context_switch_request
    #endif 
    bnez a0, context_switch

    /*  Restore active thread stack pointer */
    mv sp, s1

    /* Restore remaining registers */
trap_exit:

    REG_L  s0, s0_OFFSET(sp)
    REG_L  s1, s1_OFFSET(sp)
    REG_L  s2, s2_OFFSET(sp)
    REG_L  s3, s3_OFFSET(sp)
    REG_L  s4, s4_OFFSET(sp)
    REG_L  s5, s5_OFFSET(sp)
    REG_L  s6, s6_OFFSET(sp)
    REG_L  s7, s7_OFFSET(sp)
    REG_L  s8, s8_OFFSET(sp)
    REG_L  s9, s9_OFFSET(sp)
    REG_L  s10, s10_OFFSET(sp)
    REG_L  s11, s11_OFFSET(sp)
    REG_L  ra, ra_OFFSET(sp)
    REG_L  tp, tp_OFFSET(sp)
    REG_L  t0, t0_OFFSET(sp)
    REG_L  t1, t1_OFFSET(sp)
    REG_L  t2, t2_OFFSET(sp)
    REG_L  t3, t3_OFFSET(sp)
    REG_L  t4, t4_OFFSET(sp)
    REG_L  t5, t5_OFFSET(sp)
    REG_L  t6, t6_OFFSET(sp)
    REG_L  a0, a0_OFFSET(sp)
    REG_L  a1, a1_OFFSET(sp)
    REG_L  a2, a2_OFFSET(sp)
    REG_L  a3, a3_OFFSET(sp)
    REG_L  a4, a4_OFFSET(sp)
    REG_L  a5, a5_OFFSET(sp)
    REG_L  a6, a6_OFFSET(sp)
    REG_L  a7, a7_OFFSET(sp) 

    addi sp, sp, CONTEXT_FRAME_SIZE
    mret


 context_switch:
    /*  clear the software interrupt */
    li t0, CLINT_CTRL_ADDR
    sw zero, (t0) // SiFive CLINT only allows 32-bit access

    /*  save the active thread's PC prior to interrupt on the stack */
    csrr a0, mepc
    REG_S a0, pc_OFFSET(s1)

    #ifdef USE_LAZY_FPU_CONTEXT
    /* Disable the FPU . */
    li t0, MSTATUS_FS
    csrc mstatus, t0
    #endif

    /*  get the active thread - it may be 0 if none currently active */
    REG_L t0, sched_active_thread
    /*  was there a previously running thread? */
    beqz t0, no_sp_save
    /*  if so, save the thread's SP in the _thread structure */
    REG_S s1,SP_OFFSET_IN_THREAD(t0)

no_sp_save:
    /*  all current thread state is saved - schedule a new thread */
    call sched_run
    REG_L tp, sched_active_thread

    /*  set the threads SP from the newly scheduled thread
     *  and abandon ISR stack. */
    REG_L sp, SP_OFFSET_IN_THREAD(tp)

    /*  restore the PC */
    REG_L a0, pc_OFFSET(sp)

#ifdef USE_LAZY_FPU_CONTEXT
    /* Reenable FPU if next thread already owns it. */
    REG_L t0, fpu_owner_thread
    bne t0, tp, thread_return
    li t0, MSTATUS_FS
    csrs mstatus, t0

thread_return:
#endif

    csrw mepc, a0

    j trap_exit
    
