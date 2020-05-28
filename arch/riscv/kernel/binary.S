.section .binary.text
.align 12
.global init_bin_start, init_bin_end
init_bin_start:
#.incbin "/home/yzh/projectn/riscv-rootfs/apps/hello/build/hello"
.incbin "/home/yzh/projectn/riscv-rootfs/apps/busybox/repo/busybox"
init_bin_end:
