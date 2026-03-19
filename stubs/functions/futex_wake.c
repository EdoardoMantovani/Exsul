int futex_wake(atomic_int *addr, int flags, int nr_wake, u32 bitset){
	#ifdef __WIN64__
	if(flags == 1){
		WakeByAddressSingle(addr);
		return true;
	}
	WakeByAddressAll(addr);
	return flags;
	#else
	return (int)syscall(SYS_futex, (int *)addr, FUTEX_WAKE, flags, NULL, NULL, 0);
	#endif
}
