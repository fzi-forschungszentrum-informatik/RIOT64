/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief pthread test application
 *
 * @author Raphael Hiesgen <raphael.hiesgen@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 12

#define FACTORIAL_EXPECTED (479001600UL)

pthread_t ths[NUM_THREADS];

pthread_mutex_t mtx;
volatile uint32_t storage = 1;

void *run(void *parameter)
{
    unsigned int arg = (uintptr_t) parameter;
    printf("My arg: %u\n", arg);

    int err = pthread_mutex_lock(&mtx);

    if (err != 0) {
        printf("[!!!] pthread_mutex_lock failed with %d\n", err);
        return NULL;
    }

    storage *= arg;
    printf("val = %"PRIu32"\n", storage);
    pthread_mutex_unlock(&mtx);

    return NULL;
}

int main(void)
{
    puts("START");
    pthread_attr_t th_attr;
    pthread_attr_init(&th_attr);
    pthread_mutex_init(&mtx, NULL);

    for (uintptr_t i = 0; i < NUM_THREADS; ++i) {
        printf("Creating thread with arg %"PRIdPTR"\n", (i + 1));
        int error =  pthread_create(&ths[i], &th_attr, run, (void *)(i + 1));
        if (error < 0) {
			puts("FAILURE: Thread creation failed.");
			return 0;
		}
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        printf("join thread %d\n", (i + 1));
        pthread_join(ths[i], NULL);
    }

    printf("Factorial: %"PRIu32"\n", storage);

    pthread_mutex_destroy(&mtx);
    pthread_attr_destroy(&th_attr);

    if (storage == FACTORIAL_EXPECTED) {
        puts("SUCCESS");
    }
    else {
        puts("FAILURE: Error, expected: 12!= 479001600.");
    }

    return 0;
}
