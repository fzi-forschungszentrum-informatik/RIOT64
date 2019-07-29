/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2019 FZI Forschungszentrum Informatik
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       ssp test application
 *
 * This test should crash badly when *not* using the ssp module, and panic if
 * using it.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

void __attribute__ ((optimize("0"))) test_func(void)
{
    char buf[16];

    /* clang will detect the buffer overflow
     * and throw an error if we use `buf` directly */
    char *buffer_to_confuse_compiler = buf;

    /* cppcheck-suppress bufferAccessOutOfBounds
     * (reason: deliberately overflowing stack) */
    for (unsigned int i = 0; i < 32; ++i) {
        *buffer_to_confuse_compiler++ = 0x00;
    }
}

int main(void)
{
    puts("calling stack corruption function");

    test_func();

    puts("back to main");

    return 0;
}
