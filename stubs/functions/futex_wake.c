int futex_wake(atomic_int *addr, int flags, int nr_wake, u32 bitset){
	return (int)syscall(SYS_futex, (int *)addr, FUTEX_WAKE, flags, NULL, NULL, 0);
}
