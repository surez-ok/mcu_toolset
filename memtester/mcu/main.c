#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "memtester.h"

int main(void)
{
     // Warnging: Should cLose Dcache
    /*
     * arg1: Test start ADDR
     * arg2: Test size (Bytes)
     * arg3: Test total loops
     */
    memtester(0xA0000000, 0x1000, 5);

    return 0;
}

