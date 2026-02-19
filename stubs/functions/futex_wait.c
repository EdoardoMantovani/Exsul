int futex_wait(atomic_int *addr, int expected){
	return (int)syscall(SYS_futex, (int *)addr, FUTEX_WAIT, expected, NULL, NULL, 0);
}
