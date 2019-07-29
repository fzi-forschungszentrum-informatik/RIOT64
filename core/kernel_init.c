/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013 Freie Universität Berlin
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_internal
 * @{
 *
 * @file
 * @brief       Platform-independent kernel initilization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include "kernel_init.h"
#include "thread.h"
#include "irq.h"
#include "log.h"

#include "periph/pm.h"

#ifdef MODULE_SCHEDSTATISTICS
#include "sched.h"
#endif

#ifdef USE_LAZY_FPU_CONTEXT
#include "fpu_context.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_AUTO_INIT
#include <auto_init.h>
#endif

extern int main(void);
static void *main_trampoline(void *arg)
{
    (void) arg;

#ifdef MODULE_AUTO_INIT
    auto_init();
#endif

#ifdef MODULE_SCHEDSTATISTICS
    schedstat_t *ss = &sched_pidlist[thread_getpid()];
    ss->laststart = 0;
#endif

    LOG_INFO("main(): This is RIOT! (Version: " RIOT_VERSION  " - "__DATE__ " " __TIME__ ")\n");

    main();
    return NULL;
}

static void *idle_thread(void *arg)
{
    (void) arg;

    while (1) {
        pm_set_lowest();
    }

    return NULL;
}

const char *main_name = "main";
const char *idle_name = "idle";

static char main_stack[THREAD_STACKSIZE_MAIN];
static char idle_stack[THREAD_STACKSIZE_IDLE];

#ifdef USE_LAZY_FPU_CONTEXT
static struct fpu_context main_fpu_ctxt;
#endif

void kernel_init(void)
{
    (void) irq_disable();

    thread_create(idle_stack, sizeof(idle_stack),
            THREAD_PRIORITY_IDLE,
            THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
            idle_thread, NULL, idle_name);

    #ifdef USE_LAZY_FPU_CONTEXT
    thread_create_with_fp(main_stack, sizeof(main_stack), &main_fpu_ctxt,
            THREAD_PRIORITY_MAIN,
            THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
            main_trampoline, NULL, main_name);
    #else
    thread_create(main_stack, sizeof(main_stack),
            THREAD_PRIORITY_MAIN,
            THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
            main_trampoline, NULL, main_name);
    #endif

    cpu_switch_context_exit();
}
