#ifndef __EXSUL_GLOBALS__
#define __EXSUL_GLOBALS__

/* global symbol table */
struct internal_cb_table sym_table[SYM_ENTRIES];
struct module_controller director = {
	.elf_signal_pipe = {-1, -1},
};
__thread void *percpu_area;
__thread struct __task_struct *tls_current;
#endif
