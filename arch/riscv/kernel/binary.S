.section .binary.text
.align 12
.global init_bin_start, init_bin_end
init_bin_start:
#.incbin "/home/yzh/projectn/riscv-rootfs/apps/hello/build/hello"
.incbin "/home/yzh/projectn/riscv-rootfs/apps/busybox/repo/busybox"
init_bin_end:

#include <linux/init.h>
#include <linux/linkage.h>
#include <asm/page.h>

.globl vdso_binary_start, vdso_binary_end
	.balign PAGE_SIZE
vdso_binary_start:
	.incbin "arch/riscv/kernel/vdso/vdso.so"
	.balign PAGE_SIZE
vdso_binary_end:
