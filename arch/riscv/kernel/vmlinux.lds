OUTPUT_ARCH(riscv)
ENTRY(_start)
jiffies = jiffies_64;

MEMORY {
  flash (rxai!w) : ORIGIN = 0xffffffff40000000, LENGTH = 16M
  ram (wxa!ri)  : ORIGIN = 0xffffffff80000000, LENGTH = 4M
}

SECTIONS
{
 . = ORIGIN(flash);
 _start = .;
 .head.text : { KEEP(*(.head.text)) } > flash
 . = ALIGN((1 << (12))); .init.text : { _sinittext = .; *(.init.text .init.text.*) *(.text.startup) *(.meminit.text*) _einittext = .; } > flash
 .text : {
  _text = .;
  _stext = .;
  . = ALIGN(8); *(.text.hot .text .text.fixup .text.unlikely) *(.text..refcount) *(.ref.text)
  . = ALIGN(8); __sched_text_start = .; *(.sched.text) __sched_text_end = .;
  . = ALIGN(8); __cpuidle_text_start = .; *(.cpuidle.text) __cpuidle_text_end = .;
  . = ALIGN(8); __lock_text_start = .; *(.spinlock.text) __lock_text_end = .;
  . = ALIGN(8); __kprobes_text_start = .; *(.kprobes.text) __kprobes_text_end = .;
  . = ALIGN(8); __entry_text_start = .; *(.entry.text) __entry_text_end = .;
  . = ALIGN(8); __irqentry_text_start = .; *(.irqentry.text) __irqentry_text_end = .;
  *(.fixup)
  _etext = .;
 } > flash
 .exit.text :
 {
  *(.exit.text) *(.text.exit) *(.memexit.text)
 } > flash

  .data_flash : {
    . = ALIGN(8);
    PROVIDE( _data_flash_start = . );
  } >flash

  .data_ram : {
    . = ALIGN(8);
    PROVIDE( _data_ram_start = . );
    PROVIDE( __init_begin = . );
  } >ram AT>flash
 
 .init.data : { KEEP(*(SORT(___kentry+*))) *(.init.data init.data.*) *(.meminit.data*) *(.init.rodata .init.rodata.*) *(.meminit.rodata) . = ALIGN(8); __clk_of_table = .; KEEP(*(__clk_of_table)) KEEP(*(__clk_of_table_end)) . = ALIGN(8); __timer_of_table = .; KEEP(*(__timer_of_table)) KEEP(*(__timer_of_table_end)) . = ALIGN(32); __dtb_start = .; KEEP(*(.dtb.init.rodata)) __dtb_end = .; . = ALIGN(8); __irqchip_of_table = .; KEEP(*(__irqchip_of_table)) KEEP(*(__irqchip_of_table_end)) . = ALIGN(16); __setup_start = .; KEEP(*(.init.setup)) __setup_end = .; __initcall_start = .; KEEP(*(.initcallearly.init)) __initcall0_start = .; KEEP(*(.initcall0.init)) KEEP(*(.initcall0s.init)) __initcall1_start = .; KEEP(*(.initcall1.init)) KEEP(*(.initcall1s.init)) __initcall2_start = .; KEEP(*(.initcall2.init)) KEEP(*(.initcall2s.init)) __initcall3_start = .; KEEP(*(.initcall3.init)) KEEP(*(.initcall3s.init)) __initcall4_start = .; KEEP(*(.initcall4.init)) KEEP(*(.initcall4s.init)) __initcall5_start = .; KEEP(*(.initcall5.init)) KEEP(*(.initcall5s.init)) __initcallrootfs_start = .; KEEP(*(.initcallrootfs.init)) KEEP(*(.initcallrootfss.init)) __initcall6_start = .; KEEP(*(.initcall6.init)) KEEP(*(.initcall6s.init)) __initcall7_start = .; KEEP(*(.initcall7.init)) KEEP(*(.initcall7s.init)) __initcall_end = .; __con_initcall_start = .; KEEP(*(.con_initcall.init)) __con_initcall_end = .; __security_initcall_start = .; KEEP(*(.security_initcall.init)) __security_initcall_end = .; . = ALIGN(4); __initramfs_start = .; KEEP(*(.init.ramfs)) . = ALIGN(8); KEEP(*(.init.ramfs.info))
 
  *(.exit.data .exit.data.*) *(.fini_array) *(.dtors) *(.memexit.data*) *(.memexit.rodata*)

 . = ALIGN((1 << (12)));
  __per_cpu_load = .; __per_cpu_start = .; *(.data..percpu..first) . = ALIGN((1 << (12))); *(.data..percpu..page_aligned) . = ALIGN((1 << 6)); *(.data..percpu..read_mostly) . = ALIGN((1 << 6)); *(.data..percpu) *(.data..percpu..shared_aligned) __per_cpu_end = .;
 __init_end = .;
 _sdata = .;
 . = ALIGN(((1 << 6)));
  __start_rodata = .; *(.rodata) *(.rodata.*) __start_ro_after_init = .; *(.data..ro_after_init) __end_ro_after_init = .; KEEP(*(__vermagic)) . = ALIGN(8); __start___tracepoints_ptrs = .; KEEP(*(__tracepoints_ptrs)) __stop___tracepoints_ptrs = .; *(__tracepoints_strings)
  *(.rodata1)
  __start_pci_fixups_early = .; KEEP(*(.pci_fixup_early)) __end_pci_fixups_early = .; __start_pci_fixups_header = .; KEEP(*(.pci_fixup_header)) __end_pci_fixups_header = .; __start_pci_fixups_final = .; KEEP(*(.pci_fixup_final)) __end_pci_fixups_final = .; __start_pci_fixups_enable = .; KEEP(*(.pci_fixup_enable)) __end_pci_fixups_enable = .; __start_pci_fixups_resume = .; KEEP(*(.pci_fixup_resume)) __end_pci_fixups_resume = .; __start_pci_fixups_resume_early = .; KEEP(*(.pci_fixup_resume_early)) __end_pci_fixups_resume_early = .; __start_pci_fixups_suspend = .; KEEP(*(.pci_fixup_suspend)) __end_pci_fixups_suspend = .; __start_pci_fixups_suspend_late = .; KEEP(*(.pci_fixup_suspend_late)) __end_pci_fixups_suspend_late = .;
  __start_builtin_fw = .; KEEP(*(.builtin_fw)) __end_builtin_fw = .;
  __start___ksymtab = .; KEEP(*(SORT(___ksymtab+*))) __stop___ksymtab = .;
  __start___ksymtab_gpl = .; KEEP(*(SORT(___ksymtab_gpl+*))) __stop___ksymtab_gpl = .;
  __start___ksymtab_unused = .; KEEP(*(SORT(___ksymtab_unused+*))) __stop___ksymtab_unused = .;
  __start___ksymtab_unused_gpl = .; KEEP(*(SORT(___ksymtab_unused_gpl+*))) __stop___ksymtab_unused_gpl = .;
 __start___ksymtab_gpl_future = .; KEEP(*(SORT(___ksymtab_gpl_future+*))) __stop___ksymtab_gpl_future = .;
 __start___kcrctab = .; KEEP(*(SORT(___kcrctab+*))) __stop___kcrctab = .;
 __start___kcrctab_gpl = .; KEEP(*(SORT(___kcrctab_gpl+*))) __stop___kcrctab_gpl = .;
 __start___kcrctab_unused = .; KEEP(*(SORT(___kcrctab_unused+*))) __stop___kcrctab_unused = .;
 __start___kcrctab_unused_gpl = .; KEEP(*(SORT(___kcrctab_unused_gpl+*))) __stop___kcrctab_unused_gpl = .;
 __start___kcrctab_gpl_future = .; KEEP(*(SORT(___kcrctab_gpl_future+*))) __stop___kcrctab_gpl_future = .;
 *(__ksymtab_strings)
 *(.ref.rodata)
 __start___param = .; KEEP(*(__param)) __stop___param = .;
 __start___modver = .; KEEP(*(__modver)) __stop___modver = .; . = ALIGN(((1 << 6))); __end_rodata = .;
 . = ALIGN(((1 << 6)));
  *(.srodata*)
 . = ALIGN((1 << (12)));
 . = ALIGN(((1 << (12)) << (1))); __start_init_task = .; init_thread_union = .; init_stack = .; KEEP(*(.data..init_task)) KEEP(*(.data..init_thread_info)) . = __start_init_task + ((1 << (12)) << (1)); __end_init_task = .; . = ALIGN((1 << (12))); __nosave_begin = .; *(.data..nosave) . = ALIGN((1 << (12))); __nosave_end = .; . = ALIGN((1 << (12))); *(.data..page_aligned) . = ALIGN((1 << 6)); *(.data..cacheline_aligned) . = ALIGN((1 << 6)); *(.data..read_mostly) . = ALIGN((1 << 6)); *(.xiptext) *(.data) *(.ref.data) *(.data..shared_aligned) *(.data.unlikely) __start_once = .; *(.data.once) __end_once = .; . = ALIGN(32); *(__tracepoints) . = ALIGN(8); __start___jump_table = .; KEEP(*(__jump_table)) __stop___jump_table = .; . = ALIGN(8); __start___verbose = .; KEEP(*(__verbose)) __stop___verbose = .; CONSTRUCTORS
 . = ALIGN(8);  __start___bug_table = .; KEEP(*(__bug_table)) __stop___bug_table = .;

 . = ALIGN(0x10);  __start___ex_table = .; KEEP(*(__ex_table)) __stop___ex_table = .;
 __start_notes = .; KEEP(*(.note.*)) __stop_notes = .;

  __global_pointer$ = . + 0x800;
  *(.sdata*)
  . = ALIGN(8);
  _edata = .;
 } > ram AT>flash
 __bss_start = .; . = ALIGN(0);
 .bss : {
   *(.dynsbss) *(.sbss) *(.scommon)
    *(.bss..page_aligned) *(.dynbss) *(.bss) *(COMMON)
 } > ram
 __bss_stop = .;

 .rel.dyn : {
  *(.rel.dyn*)
 } > ram
 _end = .;

 .stab 0 : { *(.stab) } .stabstr 0 : { *(.stabstr) } .stab.excl 0 : { *(.stab.excl) } .stab.exclstr 0 : { *(.stab.exclstr) } .stab.index 0 : { *(.stab.index) } .stab.indexstr 0 : { *(.stab.indexstr) } .comment 0 : { *(.comment) }
 .debug 0 : { *(.debug) } .line 0 : { *(.line) } .debug_srcinfo 0 : { *(.debug_srcinfo) } .debug_sfnames 0 : { *(.debug_sfnames) } .debug_aranges 0 : { *(.debug_aranges) } .debug_pubnames 0 : { *(.debug_pubnames) } .debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) } .debug_abbrev 0 : { *(.debug_abbrev) } .debug_line 0 : { *(.debug_line) } .debug_frame 0 : { *(.debug_frame) } .debug_str 0 : { *(.debug_str) } .debug_loc 0 : { *(.debug_loc) } .debug_macinfo 0 : { *(.debug_macinfo) } .debug_pubtypes 0 : { *(.debug_pubtypes) } .debug_ranges 0 : { *(.debug_ranges) } .debug_weaknames 0 : { *(.debug_weaknames) } .debug_funcnames 0 : { *(.debug_funcnames) } .debug_typenames 0 : { *(.debug_typenames) } .debug_varnames 0 : { *(.debug_varnames) } .debug_gnu_pubnames 0 : { *(.debug_gnu_pubnames) } .debug_gnu_pubtypes 0 : { *(.debug_gnu_pubtypes) } .debug_types 0 : { *(.debug_types) } .debug_macro 0 : { *(.debug_macro) } .debug_addr 0 : { *(.debug_addr) }
 /DISCARD/ : { *(.exit.text) *(.text.exit) *(.memexit.text) *(.exit.data .exit.data.*) *(.fini_array) *(.dtors) *(.memexit.data*) *(.memexit.rodata*) *(.exitcall.exit) *(.discard) *(.discard.*) }
}
