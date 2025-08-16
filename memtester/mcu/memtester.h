#ifndef _MEMTESTER_H_
#define _MEMTESTER_H_

#include "types.h"

/*
 * arg1: Test start ADDR
 * arg2: Test size (Bytes)
 * arg3: Test total loops
 */
int memtester(ul test_start_physaddr, size_t test_size, ul loops);

#endif
