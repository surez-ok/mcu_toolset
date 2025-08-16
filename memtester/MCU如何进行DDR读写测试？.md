# MCU如何进行DDR读写测试？

[TOC]

**问题背景：**

嵌入式调试过程中，如果内存（DDR，SRAM，FLASH等）不稳定，那么软件在运行过程中可能会出现各种乱七八糟的状况，一些问题简直是灵异事件，调试起来特别困难，比较靠谱的做法是先进行memery读写测试，看看内存是否有问题，**注意：** 这种方法仅仅是粗筛，并不能检测出所有内存问题。

## 1 测试工具 memtester 

我们可以使用的工具：memtester 当前最新版本：4.7.1

下载地址： [memtester 官网](https://pyropus.ca./software/memtester/)

## 2 包含测试项

memtester 包含的测试项如下（部分）：

**测试项1：**

* test_stuck_address（地址线卡住）

从测试内存起始地址开始，如果偏移是偶数，写入其地址值，如果是奇数，写入地址的按位取反。然后再读出比较，重复16次。

这个测试可以检测**地址线卡住**问题，即检测内存是否可以正确寻址，这个测试需要在其他测试之前完成，如果这个测试有问题，那么内存测试几乎有问题，不用看后面用例测试情况了。

>参考：README.tests
>
>There is also a test (Stuck Address) which is run first.  It determines if the 
>memory locations the program attempts to access are addressed properly or not.  
>If this test reports errors, there is almost certainly a problem somewhere in 
>the memory subsystem.  Results from the rest of the tests cannot be considered 
>accurate if this test fails:

**测试项2：**

* test_random_value（随机模式写入一致性）
* test_xor_comparison
* test_sub_comparison
* test_mul_comparison
* test_div_comparison
* test_or_comparison
* test_and_comparison
* test_seqinc_comparison

上述测试是将待测试的内存区间分为相等两半，分别进行相同的操作，然后比较数据是否一致。**注意**：这些测试有一定局限性，比如某段内存出故障了，不管写什么都是0，那么这些测试用例依旧是PASS，并不能检测出来

**测试项3：**

* test_solidbits_comparison  （全 0/全 1 模式）
* test_blockseq_comparison
* test_checkerboard_comparison（位间串扰、棋盘格干扰）
* test_bitflip_comparison （单个位稳定性）
* test_walkbits1_comparison （行走位干扰）
* test_walkbits0_comparison （行走位干扰）

上述测试是将待测试的内存区间分为相等两半，写入全1，或者0x55555555 或0xAAAAAAAA，或者其他值，用来检测位翻转、串扰等问题




## 3 测试

分为linux和裸机MCU测试。所以分为两个版本。

linux版本：https://github.com/surez-ok/mcu_toolset/tree/main/memtester/linux

mcu版本：https://github.com/surez-ok/mcu_toolset/tree/main/memtester/mcu



### 3.1 Linux下memtest测试

**step1：下载源码**

~~~sh
$ git clone https://github.com/surez-ok/mcu_toolset.git
~~~

**step2：编译或者交叉编译**

编译主机上运行的可执行文件

~~~sh
$ cd memtester/linux
$ make
~~~

得到可执行文件 memtester，通过file命令可以查看可执行文件为x86格式

~~~sh
$ file memtester
~~~

输出为：

~~~log
memtester: ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2
~~~

也可以进行交叉编译，以RISC-V gcc为例：

需要修改conf-cc，conf-ld两个文件：

~~~sh
--- a/conf-cc
+++ b/conf-cc
@@ -1,4 +1,4 @@
-cc -O2 -DPOSIX -D_POSIX_C_SOURCE=200809L -D_FILE_OFFSET_BITS=64 -DTEST_NARROW_WRITES -c
+riscv64-unknown-linux-gnu-gcc -O2 -DPOSIX -D_POSIX_C_SOURCE=200809L -D_FILE_OFFSET_BITS=64 -DTEST_NARROW_WRITES -c

--- a/conf-ld
+++ b/conf-ld
@@ -1,3 +1,3 @@
-cc -s
+riscv64-unknown-linux-gnu-gcc -s -static

 This will be used to link .o files into an executable.
~~~

链接选项中加入了-static表示静态链接。

~~~sh
$ make clean
$ make
~~~

得到 memtester，通过file命令可以查看，可执行文件为RISC-V架构静态可执行文件。

~~~log
memtester: ELF 64-bit LSB executable, UCB RISC-V, version 1 (SYSV), statically linked, for GNU/Linux 4.15.0, stripped
~~~

**step3：测试**

~~~log
# ./memtester 128M 2
memtester version 4.7.1 (64-bit)
Copyright (C) 2001-2024 Charles Cazabon.
Licensed under the GNU General Public License version 2 (only).

pagesize is 4096
pagesizemask is 0xfffffffffffff000
want 128MB (134217728 bytes)
got  128MB (134217728 bytes), trying mlock ...locked.
Loop 1/2:
  Stuck Address       : ok
  Random Value        : ok
  Compare XOR         : ok
  Compare SUB         : ok
  Compare MUL         : ok
  Compare DIV         : ok
  Compare OR          : ok
  Compare AND         : ok
  Sequential Increment: ok
  Solid Bits          : ok
  Block Sequential    : ok
  Checkerboard        : ok
  Bit Spread          : ok
  Bit Flip            : ok
  Walking Ones        : ok
  Walking Zeroes      : ok
  8-bit Writes        : ok
  16-bit Writes       : ok

Loop 2/2:
  ...
~~~

### 3.2 MCU下memtest 测试

MCU memtest测试代码由linux版本删减而来。

**step1：下载源码**

~~~sh
$ git clone https://github.com/surez-ok/mcu_toolset.git
~~~

**step2：切到mcu版本**

~~~sh
$ cd memtester/mcu
~~~

**step3：移植并测试**

调用API如下所示，需要输入测试的起始物理地址，测试区间多少Bytes，循环多少次。

**注意**：裸机版本需要关闭Dcache，否则数据将会在Dcache中打转，测试不到DDR

~~~c
int main(void)
{
     // Warnging: Should cLose Dcache
    /*
     * arg1: Test start ADDR
     * arg2: Test size (Bytes)
     * arg3: Test total loops
     */
    memtester(0x80000000, 0x1000, 5);

    return 0;
}
~~~




**参考：**

1. [Linux 内存测试工具memtester安装与使用](https://www.cnblogs.com/fortunely/articles/14924305.html)
2. [memtester4.3.0_stuck address-CSDN博客](https://blog.csdn.net/jdc317264476/article/details/82790121)