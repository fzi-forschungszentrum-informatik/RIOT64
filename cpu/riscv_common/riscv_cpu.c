 /*
 * Copyright (C) 2017 Ken Rabold, JP Bonn
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
 * @file        cpu.c
 * @brief       Common low-level functions for RISC-V CPUs
 *
 * @author      Ken Rabold
 * @author      Leon Hielscher <hielscher@fzi.de>

 * @}
 */


#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "thread.h"
#include "irq.h"
#include "sched.h"
#include "thread.h"
#include "irq.h"
#include "cpu.h"
#include "context_frame.h"
#include "periph_cpu.h"
#include "panic.h"
#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "riscv_arch.h"
#include "fpu_context.h"
#include "kernel_types.h"
#include "assert.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)


volatile int __in_isr = 0;

void trap_entry(void);

/* PLIC external ISR function list */
//static external_isr_ptr_t _ext_isrs[PLIC_NUM_INTERRUPTS];
extern void rv_plic_init(void);
extern void rv_plic_hanlde_isr(void);


#ifdef USE_LAZY_FPU_CONTEXT

volatile thread_t* fpu_owner_thread;


static void fpu_insn_trap(void) {

    // Check if FPU is already enabled
    if(unlikely(RV_IS_FPU_DIRTY())) {
        core_panic(PANIC_GENERAL_ERROR, "Illegal FPU instruction.");
        return;
    }

    // Check if thread has FPU context
    struct fpu_context* restoreCtx = sched_active_thread->fpucontext;
    if (unlikely(restoreCtx == NULL || __in_isr)) {
        // Bad thread!
        core_panic(PANIC_GENERAL_ERROR, "FPU instruction in thread without FPU context.");
        return;
    }

    RV_FPU_ENABLE();

    // Save context of old owner (if there was one)
    if (likely(fpu_owner_thread != NULL)) {
		struct fpu_context* fpu_owner_ctx = fpu_owner_thread->fpucontext;
		assert(fpu_owner_ctx != NULL); // A thread without FPU context should never become FPU owner
		__asm__ __volatile__ (RV_FSTORE_IASM(ft0)  ", " ft0_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft1)  ", " ft1_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft2)  ", " ft2_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft3)  ", " ft3_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft4)  ", " ft4_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft5)  ", " ft5_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft6)  ", " ft6_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft7)  ", " ft7_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft8)  ", " ft8_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft9)  ", " ft9_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft10) ", " ft10_OFFSET_IASM "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(ft11) ", " ft11_OFFSET_IASM "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs0)  ", " fs0_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs1)  ", " fs1_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs2)  ", " fs2_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs3)  ", " fs3_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs4)  ", " fs4_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs5)  ", " fs5_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs6)  ", " fs6_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs7)  ", " fs7_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs8)  ", " fs8_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs9)  ", " fs9_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs10) ", " fs10_OFFSET_IASM "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fs11) ", " fs11_OFFSET_IASM "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fa0)  ", " fa0_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fa1)  ", " fa1_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fa2)  ", " fa2_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fa3)  ", " fa3_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fa4)  ", " fa4_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fa5)  ", " fa5_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fa6)  ", " fa6_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ (RV_FSTORE_IASM(fa7)  ", " fa7_OFFSET_IASM  "(%0)" : : "r" (fpu_owner_ctx));
		__asm__ __volatile__ ("frcsr %0" : : "r" (fpu_owner_ctx->fcsr));
    }

    fpu_owner_thread = sched_active_thread;
    
    // Restore Context of running thread
    __asm__ __volatile__ (RV_FLOAD_IASM(ft0)  ", " ft0_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft1)  ", " ft1_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft2)  ", " ft2_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft3)  ", " ft3_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft4)  ", " ft4_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft5)  ", " ft5_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft6)  ", " ft6_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft7)  ", " ft7_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft8)  ", " ft8_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft9)  ", " ft9_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft10) ", " ft10_OFFSET_IASM "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(ft11) ", " ft11_OFFSET_IASM "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs0)  ", " fs0_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs1)  ", " fs1_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs2)  ", " fs2_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs3)  ", " fs3_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs4)  ", " fs4_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs5)  ", " fs5_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs6)  ", " fs6_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs7)  ", " fs7_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs8)  ", " fs8_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs9)  ", " fs9_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs10) ", " fs10_OFFSET_IASM "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fs11) ", " fs11_OFFSET_IASM "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fa0)  ", " fa0_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fa1)  ", " fa1_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fa2)  ", " fa2_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fa3)  ", " fa3_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fa4)  ", " fa4_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fa5)  ", " fa5_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fa6)  ", " fa6_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ (RV_FLOAD_IASM(fa7)  ", " fa7_OFFSET_IASM  "(%0)" : : "r" (restoreCtx));
    __asm__ __volatile__ ("fscsr %0" : : "r" (restoreCtx->fcsr));

    DEBUG("Lazy FPU context switch performed for thread %" PRIkernel_pid ".\r\n", sched_active_pid);
}

static void illegal_system_insn_trap(uint32_t insn) {
	switch (insn & MASK_CSRRW) {
		case MATCH_CSRRW:
		case MATCH_CSRRS:
		case MATCH_CSRRC:
		case MATCH_CSRRWI:
		case MATCH_CSRRSI:
		case MATCH_CSRRCI:
			switch (insn >> 20) {
				case CSR_FFLAGS:
				case CSR_FRM:
				case CSR_FCSR:
					// FPU Status register access
					fpu_insn_trap();
					return;
				default:
				core_panic(PANIC_GENERAL_ERROR, "Illegal CSR access.");
				UNREACHABLE();
			}
		default:
			core_panic(PANIC_GENERAL_ERROR, "Illegal system instruction.");
			UNREACHABLE();
	}  
}

#endif // USE_LAZY_FPU_CONTEXT


// 32-Bit illegal instruction jump table
static inline void illegal_insn32_jumptable(uint32_t insn) {
	#ifdef USE_LAZY_FPU_CONTEXT
    switch((insn & 0x7F) >> 2) {
        
        case INSN_FLOAD_SUFFIX:
        case INSN_FSTORE_SUFFIX:
        case INSN_FOPR_SUFFIX:
        case INSN_FMADD_SUFFIX:
        case INSN_FMSUB_SUFFIX:
        case INSN_FNMSUB_SUFFIX:
        case INSN_FNMADD_SUFFIX:
            fpu_insn_trap();
            return;
        case INSN_SYSTEM_SUFFIX:
			illegal_system_insn_trap(insn);
			return;
        default:
            core_panic(PANIC_GENERAL_ERROR, "Unhandled illegal instruction trap");
            UNREACHABLE();
    }
    #else
    (void) insn;
    core_panic(PANIC_GENERAL_ERROR, "Unhandled illegal instruction trap");
    UNREACHABLE();
    #endif
}

#ifdef __riscv_compressed
// 16-Bit illegal instruction jump table, only required with 'C' extension
static inline void illegal_insn16_jumptable(uint16_t insn) {
	switch (insn & INSNC_MASK) {
		#ifdef USE_LAZY_FPU_CONTEXT
		case INSNC_FLD:
		case INSNC_FSW:
		case INSNC_FLDSP:
		case INSNC_FSDSP:
			fpu_insn_trap();
			return;
		#endif
		default:
		core_panic(PANIC_GENERAL_ERROR, "Unhandled illegal (compressed) instruction trap");
		UNREACHABLE();

	}
}
#endif __riscv_compressed


/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    volatile uint64_t *mtimecmp =
        (uint64_t *) (CLINT_CTRL_ADDR + CLINT_MTIMECMP);

    /* Setup trap handler function */
    write_csr(mtvec, &trap_entry);

    
    /* Enable FPU if present */
 //   #ifdef RV_ENABLE_FPU_CONTEXT_SWITCHING
 //   if (read_csr(misa) & (1 << ('F' - 'A'))) {
 //       RV_FPU_ENABLE();
 //       write_csr(fcsr, 0);             /* initialize rounding mode, undefined at reset */
 //   }
 //   #endif
    

    /* Clear all interrupt enables */
    write_csr(mie, 0);
    
    

    /* Initial PLIC external interrupt controller */
    //PLIC_init(PLIC_CTRL_ADDR, PLIC_NUM_INTERRUPTS, PLIC_NUM_PRIORITIES);

    /* Initialize ISR function list */
    //for (int i = 0; i < PLIC_NUM_INTERRUPTS; i++) {
    //    _ext_isrs[i] = null_isr;
   // } */
   
   rv_plic_init();
    

    /* Set mtimecmp to largest value to avoid spurious timer interrupts */
    *mtimecmp = 0xFFFFFFFFFFFFFFFF;

    /* Enable SW, timer and external interrupts */
    set_csr(mie, MIP_MSIP);
    set_csr(mie, MIP_MTIP);
    set_csr(mie, MIP_MEIP);

    /*  Set default state of mstatus */
    set_csr(mstatus, MSTATUS_DEFAULT);
}

void fpu_init(void) {
    RV_FPU_DISABLE(); // Disable the FPU for now, if any.
    // Ensure we have compiled with the right settings
    #ifdef RV_FPU_SINGLE_PRECISION
    rvxlen_unsigned_t misa = read_csr(misa);
    if (misa != 0) {
		if(! (misa & (1 << ('F' - 'A')) ) )  {
			core_panic(PANIC_GENERAL_ERROR, "CPU lacks float extension.");
			UNREACHABLE();
		}
	}
    #endif

    #ifdef RV_FPU_DOUBLE_PRECISION
    rvxlen_unsigned_t misa = read_csr(misa);
    if (misa != 0) {
		if(! (read_csr(misa) & (1 << ('D' - 'A')) ) ) {
			core_panic(PANIC_GENERAL_ERROR, "CPU lacks double float extension.");
			UNREACHABLE();
		}
	}
    #endif
    
    
#ifdef USE_LAZY_FPU_CONTEXT
    fpu_owner_thread = NULL;
#endif
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned irq_enable(void)
{
    /* Enable all interrupts */
    unsigned previous_state = (unsigned)(read_csr(mstatus) & MSTATUS_MIE);
    set_csr(mstatus, MSTATUS_MIE);
    return previous_state;
}

/**
 * @brief Disable all maskable interrupts
 */
unsigned irq_disable(void)
{
    unsigned previous_state = (unsigned)(read_csr(mstatus) & MSTATUS_MIE);
    /* Disable all interrupts */
    clear_csr(mstatus, MSTATUS_MIE);
    return previous_state;
}

/**
 * @brief Restore the state of the IRQ flags
 */
void irq_restore(unsigned state)
{
    /* Restore all interrupts to given state */
    if ((state & MSTATUS_MIE) != 0)
		set_csr(mstatus, MSTATUS_MIE);
	else
		clear_csr(mstatus, MSTATUS_MIE);
}

/**
 * @brief See if the current context is inside an ISR
 */
int irq_is_in(void)
{
    return __in_isr;
}


/**
 * @brief Global trap and interrupt handler
 */
void handle_trap(rvxlen_unsigned_t mcause)
{

    /*  Tell RIOT to set sched_context_switch_request instead of
     *  calling thread_yield(). */
    /* Check for INT or TRAP */
    if ((mcause & MCAUSE_INT) == MCAUSE_INT) {
        __in_isr = 1;

#ifdef USE_LAZY_FPU_CONTEXT
        /* Rememeber FPU state and turn it off */
        int fpuStat = RV_IS_FPU_DIRTY();
        RV_FPU_DISABLE();
#endif

        /* Cause is an interrupt - determine type */
        switch (mcause & MCAUSE_CAUSE) {
#ifdef MODULE_PERIPH_TIMER
            case IRQ_M_TIMER:
                /* Handle timer interrupt */
                timer_isr();
                break;
#endif
            case IRQ_M_EXT:
                /* Handle external interrupt */
                rv_plic_hanlde_isr();
                break;

            default:
                /* Unknown interrupt */
                core_panic(PANIC_GENERAL_ERROR, "Unhandled interrupt");
                UNREACHABLE();
                break;
        }
        
#ifdef USE_LAZY_FPU_CONTEXT
        /* Restore FPU state */
        if (fpuStat) {
            RV_FPU_ENABLE();
        }
#endif

        /* ISR done - no more changes to thread states */
        __in_isr = 0;
    } else {
        // Machine trap
        if (likely(mcause == CAUSE_ILLEGAL_INSTRUCTION) ) {

            rvxlen_unsigned_t mepc = read_csr(mepc);
            
#ifdef __riscv_compressed
  	// For compressed instructions we need to check the length of the illegal instruction and its alignment
            if ((mepc & 0x3) == 0) {
		uint32_t insn = *((uint32_t*)mepc);
		if ((insn & INSN_32BIT_MASK) == INSN_32BIT_MASK) {
			// Aligned 32-Bit Instruction
			illegal_insn32_jumptable(insn);
		} else {
			// Aligned 16-Bit Instruction
			illegal_insn16_jumptable((uint16_t)insn);
		}
            } else {
            	uint32_t insn = (uint32_t)(*((uint16_t*)mepc)); // Ugh..
            	if ((insn & INSN_32BIT_MASK) == INSN_32BIT_MASK) {
            		// Unaligned 32-Bit Instruction
            		insn += *(((uint16_t*)mepc) + 1) << 16;
            		illegal_insn32_jumptable(insn);
            	} else {
            		illegal_insn16_jumptable((uint16_t)insn);
            	}
            }
#else 
	    uint32_t insn = *((uint32_t*)mepc);
	    illegal_insn32_jumptable(insn);
#endif   // #ifdef __riscv_compressed       

        }  else {
            core_panic(PANIC_GENERAL_ERROR, "Unhandled machine mode trap");
            UNREACHABLE();
        }
    }
}

/**
 * @brief   Noticeable marker marking the beginning of a stack segment
 *
 * This marker is used e.g. by *thread_start_threading* to identify the
 * stacks beginning.
 */
#if (__riscv_xlen == 64) 
#define STACK_MARKER                (0xEEEEEEEE77777777UL)
#else
#define STACK_MARKER                (0x77777777)
#endif

/**
 * @brief Initialize a thread's stack
 *
 * RIOT saves the tasks registers on the stack, not in the task control
 * block.  thread_stack_init() is responsible for allocating space for
 * the registers on the stack and adjusting the stack pointer to account for
 * the saved registers.
 *
 * The stack_start parameter is the bottom of the stack (low address).  The
 * return value is the top of stack: stack_start + stack_size - space reserved
 * for thread context save - space reserved to align stack.
 *
 * thread_stack_init is called for each thread.
 *
 * RISCV ABI is here: https://github.com/riscv/riscv-elf-psabi-doc
 * From ABI:
 * The stack grows downwards and the stack pointer shall be aligned to a
 * 128-bit boundary upon procedure entry, except for the RV32E ABI, where it
 * need only be aligned to 32 bits. In the standard ABI, the stack pointer
 * must remain aligned throughout procedure execution. Non-standard ABI code
 * must realign the stack pointer prior to invoking standard ABI procedures.
 * The operating system must realign the stack pointer prior to invoking a
 * signal handler; hence, POSIX signal handlers need not realign the stack
 * pointer. In systems that service interrupts using the interruptee's stack,
 * the interrupt service routine must realign the stack pointer if linked
 * with any code that uses a non-standard stack-alignment discipline, but
 * need not realign the stack pointer if all code adheres to the standard ABI.
 *
 * @param[in] task_func     pointer to the thread's code
 * @param[in] arg           argument to task_func
 * @param[in] stack_start   pointer to the start address of the thread
 * @param[in] stack_size    the maximum size of the stack
 *
 * @return                  pointer to the new top of the stack (128bit aligned)
 *
 */
char *thread_stack_init(thread_task_func_t task_func,
                             void *arg,
                             void *stack_start,
                             int stack_size)
{
    struct context_switch_frame *sf;
    rvxlen_unsigned_t *reg;
    rvxlen_unsigned_t *stk_top;

    /* calculate the top of the stack */
    stk_top = (rvxlen_unsigned_t *)((uintptr_t)stack_start + stack_size);

    /* Put a marker at the top of the stack.  This is used by
     * thread_stack_print to determine where to stop dumping the
     * stack.
     */
    stk_top--;
    *stk_top = STACK_MARKER;

    /* per ABI align stack pointer to 16 byte boundary. */
    stk_top = (rvxlen_unsigned_t *)(((rvxlen_unsigned_t)stk_top) & ~((rvxlen_unsigned_t)0xf));

    /* reserve space for the stack frame. */
    stk_top = (rvxlen_unsigned_t *)((uint8_t *) stk_top - sizeof(*sf));

    /* populate the stack frame with default values for starting the thread. */
    sf = (struct context_switch_frame *) stk_top;

    /* a7 is register with highest memory address in frame */
    reg = &sf->a7;
    while (reg != &sf->pc) {
        *reg-- = 0;
    }
    sf->pc = (rvxlen_unsigned_t) task_func;
    sf->a0 = (rvxlen_unsigned_t) arg;

    /* if the thread exits go to sched_task_exit() */
    sf->ra = (rvxlen_unsigned_t) sched_task_exit;

    return (char *) stk_top;
}

void thread_print_stack(void)
{
    int count = 0;
    rvxlen_unsigned_t *sp = (rvxlen_unsigned_t *) sched_active_thread->sp;

    printf("printing the current stack of thread %" PRIkernel_pid "\n",
           thread_getpid());

#ifdef DEVELHELP
    printf("thread name: %s\n", sched_active_thread->name);
    printf("stack start: 0x%016lx\n", (rvxlen_unsigned_t)(sched_active_thread->stack_start));
    printf("stack end  : 0x%016lx\n", (rvxlen_unsigned_t)(sched_active_thread->stack_start + sched_active_thread->stack_size));
#endif

    printf("  address:      data:\n");

    do {
        printf("  0x%016lx:   0x%016lx\n", (rvxlen_unsigned_t) sp, (rvxlen_unsigned_t) *sp);
        sp++;
        count++;
    } while (*sp != STACK_MARKER);

    printf("current stack size: %i words\n", count);
}

int thread_isr_stack_usage(void)
{
    return 0;
}

void *thread_isr_stack_pointer(void)
{
    return NULL;
}

void *thread_isr_stack_start(void)
{
    return NULL;
}

/**
 * @brief Start or resume threading by loading a threads initial information
 * from the stack.
 *
 * This is called is two situations: 1) after the initial main and idle threads
 * have been created and 2) when a thread exits.
 *
 * sched_active_thread is not valid when cpu_switch_context_exit() is
 * called.  sched_run() must be called to determine the next valid thread.
 * This is exploited in the context switch code.
 */
void cpu_switch_context_exit(void)
{
    /* enable interrupts */
    irq_enable();

    /* start the thread */
    thread_yield();
    UNREACHABLE();
}

void thread_yield_higher(void)
{
    /* Use SW intr to schedule context switch */
    CLINT_REG(CLINT_MSIP) = 1;
    //RV_ASM_WFI();
    RV_ASM_FENCE_IORW();
}

#ifdef RV_PANIC_PRINT
void panic_arch(void) {
    printf("MCAUSE = 0x%" PRIxRVXLEN "\r\n", read_csr(mcause));
    printf("MEPC   = 0x%" PRIxRVXLEN "\r\n", read_csr(mepc));
}
#endif

#ifdef USE_LAZY_FPU_CONTEXT
void thread_fpu_context_init(struct fpu_context* fc, kernel_pid_t pid) {
	// Take away FPU ownership if a new thread with the same PID appears
	if (fpu_owner_thread != NULL && fpu_owner_thread->pid == pid) {
		fpu_owner_thread = NULL;
	}

	// If the new thread has an FPU Context, initialize it to zero
	if (fc != NULL) {
		memset(fc, 0, sizeof(struct fpu_context));
	}
}
#endif
