// See LICENSE for license details.

#include "encoding.h"

  .section .rodata
  .globl logo
logo:
  .incbin "arch/riscv/bbl/riscv_logo.txt"
  .byte 0
