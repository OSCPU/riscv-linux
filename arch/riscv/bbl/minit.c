// See LICENSE for license details.

#include "mtrap.h"
#include "atomic.h"
#include "vm.h"
#include "fp_emulation.h"
#include "fdt.h"
//#include "finisher.h"
#include "disabled_hart_mask.h"
#include <linux/string.h>
//#include <string.h>
//#include <limits.h>

#   define ULONG_MAX  18446744073709551615UL

pte_t* root_page_table;
uintptr_t mem_size;
volatile uint64_t* mtime;
volatile uint32_t* plic_priorities;
size_t plic_ndevs;
void* kernel_start;
void* kernel_end;
void __am_init_uartlite(void);

static void mstatus_init(void)
{
  // Enable FPU
  //if (supports_extension('D') || supports_extension('F'))
  //  write_csr(mstatus, MSTATUS_FS);

  // Enable user/supervisor use of perf counters
  //if (supports_extension('S'))
    write_csr(scounteren, -1);
  if (supports_extension('U'))
    write_csr(mcounteren, -1);

  // Enable software interrupts
  write_csr(mie, MIP_MSIP);

  // Disable paging
  //if (supports_extension('S'))
    write_csr(sptbr, 0);
}

// send S-mode interrupts and most exceptions straight to S-mode
static void delegate_traps(void)
{
  printm("misa = 0x%lx\n", read_csr(misa));
  if (!supports_extension('S')) {
    printm("???\n");
//    return;
  }

  uintptr_t interrupts = MIP_SSIP | MIP_STIP | MIP_SEIP;
  uintptr_t exceptions =
    (1U << CAUSE_MISALIGNED_FETCH) |
    (1U << CAUSE_FETCH_PAGE_FAULT) |
    (1U << CAUSE_BREAKPOINT) |
    (1U << CAUSE_LOAD_PAGE_FAULT) |
    (1U << CAUSE_STORE_PAGE_FAULT) |
    (1U << CAUSE_USER_ECALL);

  write_csr(mideleg, interrupts);
  write_csr(medeleg, exceptions);
  assert(read_csr(mideleg) == interrupts);
  assert(read_csr(medeleg) == exceptions);
  printm("medeleg = 0x%lx\n", exceptions);
}

static void dump_misa(uint32_t misa) {
  int i;
  for (i = 0; i < 26; i ++) {
    if (misa & 0x1) printm("%c ", 'A' + i);
    misa >>= 1;
  }
  printm("\n");
}

static void fp_init(void)
{
  if (!supports_extension('D') && !supports_extension('F'))
    return;

  assert(read_csr(mstatus) & MSTATUS_FS);

#ifdef __riscv_flen
  for (int i = 0; i < 32; i++)
    init_fp_reg(i);
  write_csr(fcsr, 0);
#else
  uintptr_t fd_mask = (1 << ('F' - 'A')) | (1 << ('D' - 'A'));
  clear_csr(misa, fd_mask);
  dump_misa(read_csr(misa));
  //assert(!(read_csr(misa) & fd_mask));
#endif
}

hls_t* hls_init(uintptr_t id)
{
  hls_t* hls = OTHER_HLS(id);
  memset(hls, 0, sizeof(*hls));
  return hls;
}

static void memory_init(void)
{
  mem_size = mem_size / MEGAPAGE_SIZE * MEGAPAGE_SIZE;
}

static void hart_init(void)
{
  mstatus_init();
  //fp_init();
#ifndef BBL_BOOT_MACHINE
  delegate_traps();
#endif /* BBL_BOOT_MACHINE */
  setup_pmp();
}

static void plic_init(void)
{
  size_t i;
  for (i = 1; i <= plic_ndevs; i++)
    plic_priorities[i] = 1;
}

static void prci_test(void)
{
  assert(!(read_csr(mip) & MIP_MSIP));
  *HLS()->ipi = 1;
  assert(read_csr(mip) & MIP_MSIP);
  *HLS()->ipi = 0;

  assert(!(read_csr(mip) & MIP_MTIP));
  *HLS()->timecmp = 0;
  assert(read_csr(mip) & MIP_MTIP);
  *HLS()->timecmp = -1ULL;
}

static void hart_plic_init(void)
{
  // clear pending interrupts
  *HLS()->ipi = 0;
  *HLS()->timecmp = -1ULL;
  write_csr(mip, 0);

  if (!plic_ndevs)
    return;

  size_t ie_words = (plic_ndevs + 8 * sizeof(uintptr_t) - 1) /
		(8 * sizeof(uintptr_t));
  size_t i;
  for (i = 0; i < ie_words; i++) {
     if (HLS()->plic_s_ie) {
        // Supervisor not always present
        HLS()->plic_s_ie[i] = ULONG_MAX;
     }
  }
  *HLS()->plic_m_thresh = 1;
  if (HLS()->plic_s_thresh) {
      // Supervisor not always present
      *HLS()->plic_s_thresh = 0;
  }
}

static void wake_harts(void)
{
  int hart;
  for (hart = 0; hart < MAX_HARTS; ++hart)
    if ((((~disabled_hart_mask & hart_mask) >> hart) & 1))
      *OTHER_HLS(hart)->ipi = 1; // wakeup the hart
}

void __am_uartlite_putchar(char ch);

static void uart_printhex(uint32_t x, int byte) {
  int i;
  __am_uartlite_putchar('0');
  __am_uartlite_putchar('x');
  for (i = 0; i < byte * 2; i ++, x <<= 4)
    __am_uartlite_putchar("0123456789abcdef"[(x >> (byte * 8 - 4)) & 0xf]);
}

static void uart_printstr(const char *str) {
  while (*str) __am_uartlite_putchar(*str ++);
}

static void check_data(void) {
  extern char _data_flash_start;
  extern char _data_ram_start;
  extern char _edata;
  char *mem = &_data_ram_start;
  char *flash = &_data_flash_start;
  uint32_t size = &_edata - &_data_ram_start;
  uart_printstr("data_ram_start = ");
  uart_printhex((uint32_t)(uint64_t)mem, 4);
  uart_printstr(", data_flash_start = ");
  uart_printhex((uint32_t)(uint64_t)flash, 4);
  uart_printstr(", size = ");
  uart_printhex(size, 4);
  uart_printstr("\n");

  int i;
  int wrong_time = 0;
  for (i = 0; i < size; i ++) {
    char membyte = mem[i];
    char flashbyte = flash[i];
    if (membyte != flashbyte) {
      wrong_time ++;
      uart_printhex(wrong_time, 4);
      uart_printstr("th wrong byte detect at address ");
      uart_printhex((uint32_t)(uint64_t)(mem + i), 4);
      uart_printstr(", right = ");
      uart_printhex(flashbyte, 1);
      uart_printstr(", wrong = ");
      uart_printhex(membyte, 1);
      uart_printstr(", diff = ");
      uart_printhex(flashbyte ^ membyte, 1);
      uart_printstr("\n");
    }
  }
  uart_printstr("check end!\n");
}

void fix_ras(int deep) {
  if (deep == 0) {
    uart_printstr("Filling RAS with legal address...\n");
    return;
  }
  fix_ras(deep - 1);
  volatile int i;
  i ++;
}

void bbl_self_load_data(void);

void init_first_hart(uintptr_t hartid, uintptr_t dtb)
{
#ifndef __QEMU__
  extern char dtb_start;
  dtb = (uintptr_t)&dtb_start;
#endif

  __am_init_uartlite();
  fix_ras(20);
  uart_printstr("Loading data from FLASH to SDRAM...\r\n");
  bbl_self_load_data();
  uart_printstr("Checking data...\r\n");
  check_data();
  printm("bbl loader\r\n");

  hart_init();
  hls_init(0); // this might get called again from parse_config_string

  // Find the power button early as well so die() works
  //query_finisher(dtb);

  //query_mem(dtb);
  query_harts(dtb);
  query_clint(dtb);
  query_plic(dtb);
  query_chosen(dtb);

  wake_harts();

  plic_init();
  hart_plic_init();
  //prci_test();
  memory_init();
  boot_loader(dtb);
}

void init_other_hart(uintptr_t hartid, uintptr_t dtb)
{
  hart_init();
  hart_plic_init();
  boot_other_hart(dtb);
}

void setup_pmp(void)
{
  // Set up a PMP to permit access to all of memory.
  // Ignore the illegal-instruction trap if PMPs aren't supported.
  uintptr_t pmpc = PMP_NAPOT | PMP_R | PMP_W | PMP_X;
  asm volatile ("la t0, 1f\n\t"
                "csrrw t0, mtvec, t0\n\t"
                "csrw pmpaddr0, %1\n\t"
                "csrw pmpcfg0, %0\n\t"
                ".align 2\n\t"
                "1: csrw mtvec, t0"
                : : "r" (pmpc), "r" (-1UL) : "t0");
}

void enter_supervisor_mode(void (*fn)(uintptr_t), uintptr_t arg0, uintptr_t arg1)
{
  uintptr_t mstatus = read_csr(mstatus);
  mstatus = INSERT_FIELD(mstatus, MSTATUS_MPP, PRV_S);
  mstatus = INSERT_FIELD(mstatus, MSTATUS_MPIE, 0);
  write_csr(mstatus, mstatus);
  write_csr(mscratch, MACHINE_STACK_TOP() - MENTRY_FRAME_SIZE);
#ifndef __riscv_flen
  uintptr_t *p_fcsr = MACHINE_STACK_TOP() - MENTRY_FRAME_SIZE; // the x0's save slot
  *p_fcsr = 0;
#endif
  write_csr(mepc, fn);

  register uintptr_t a0 asm ("a0") = arg0;
  register uintptr_t a1 asm ("a1") = arg1;
  asm volatile ("mret" : : "r" (a0), "r" (a1));
  __builtin_unreachable();
}

void enter_machine_mode(void (*fn)(uintptr_t, uintptr_t), uintptr_t arg0, uintptr_t arg1)
{
  uintptr_t mstatus = read_csr(mstatus);
  mstatus = INSERT_FIELD(mstatus, MSTATUS_MPIE, 0);
  write_csr(mstatus, mstatus);
  write_csr(mscratch, MACHINE_STACK_TOP() - MENTRY_FRAME_SIZE);

  /* Jump to the payload's entry point */
  fn(arg0, arg1);

  __builtin_unreachable();
}
