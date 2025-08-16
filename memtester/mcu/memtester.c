/*
 * memtester version 4
 *
 * Very simple but very effective user-space memory tester.
 * Originally by Simon Kirby <sim@stormix.com> <sim@neato.org>
 * Version 2 by Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Version 3 not publicly released.
 * Version 4 rewrite:
 * Copyright (C) 2004-2024 Charles Cazabon <charlesc-memtester@pyropus.ca>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */

#define __version__ "4.7.1"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include "memtester.h"
#include "tests.h"

#define EXIT_FAIL_NONSTARTER    0x01
#define EXIT_FAIL_ADDRESSLINES  0x02
#define EXIT_FAIL_OTHERTEST     0x04

struct test tests[] = {
    { "Random Value", test_random_value },
    { "Compare XOR", test_xor_comparison },
    { "Compare SUB", test_sub_comparison },
    { "Compare MUL", test_mul_comparison },
    { "Compare DIV",test_div_comparison },
    { "Compare OR", test_or_comparison },
    { "Compare AND", test_and_comparison },
    { "Sequential Increment", test_seqinc_comparison },
    { "Solid Bits", test_solidbits_comparison },
    { "Block Sequential", test_blockseq_comparison },
    { "Checkerboard", test_checkerboard_comparison },
    { "Bit Spread", test_bitspread_comparison },
    { "Bit Flip", test_bitflip_comparison },
    { "Walking Ones", test_walkbits1_comparison },
    { "Walking Zeroes", test_walkbits0_comparison },
#ifdef TEST_NARROW_WRITES
    { "8-bit Writes", test_8bit_wide_random },
    { "16-bit Writes", test_16bit_wide_random },
#endif
    { NULL, NULL }
};


/*
 * arg1: Test start ADDR
 * arg2: Test size (Bytes)
 * arg3: Test total loops
 */
int memtester(ul test_start_physaddr, size_t test_size, ul loops)
{
    ul loop, i;
    size_t bufsize, halflen, count;
    ulv *buf;
    ulv *bufa, *bufb;
    int exit_code = 0;

    printf("memtester version " __version__ " (%d-bit)\n", UL_LEN);
    printf("Copyright (C) 2001-2024 Charles Cazabon.\n");
    printf("Licensed under the GNU General Public License version 2 (only).\n");
    printf("\n");
    printf("Warning: Ensure you have closed Dcache!!\n");
    printf("\nMemtest start ADDR: " FMT_TARGET ", Test Size is 0x%lx Bytes, Test loops is %lu\n", test_start_physaddr,  test_size, loops);

    /* In theory, you might be able to get slightly better error detection if you randomly seed
       the pseudo-random number generator and run memtester multiple times in sequence.
       However, that benefit is probably very, very slight and won't matter.  Leaving it unseeded
       results in a constant seed, so exactly the same values are used test-to-test, giving
       better reproducibility, so this is disabled by default.  You can uncomment it to enable.
       Note there are no security implications here */
    srand(1);

    buf = (ulv *)test_start_physaddr;
    bufsize = test_size;

    halflen = bufsize / 2;
    count = halflen / sizeof(ul);
    bufa = (ulv *) buf;
    bufb = (ulv *) ((size_t) buf + halflen);

    for(loop=1; ((!loops) || loop <= loops); loop++) {
        printf("Loop %lu", loop);
        if (loops) {
            printf("/%lu", loops);
        }
        printf(":\n");
        printf("  %-20s: ", "Stuck Address");
        fflush(stdout);
        if (!test_stuck_address(buf, bufsize / sizeof(ul))) {
             printf("ok\n");
        } else {
            exit_code |= EXIT_FAIL_ADDRESSLINES;
        }
        for (i=0;;i++) {
            if (!tests[i].name) break;
            printf("  %-20s: ", tests[i].name);
            if (!tests[i].fp(bufa, bufb, count)) {
                printf("ok\n");
            } else {
                exit_code |= EXIT_FAIL_OTHERTEST;
            }
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
        if (exit_code) break;
    }

    if (exit_code) {
        printf("Mem Test Fail!, exit_code = %d\n", exit_code);
        printf("Fail, please check!!\n");
        fflush(stdout);
    } else {
        printf("Mem Test Pass!\n");
        printf("OK.\n");
        fflush(stdout);
    }
    
    return 0;
}
