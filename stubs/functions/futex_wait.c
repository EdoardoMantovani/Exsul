int futex_wait(atomic_int *addr, int expected){
	#ifdef __WIN64__
	if(expected == 1){
		WakeByAddressSingle(addr);
		return true;
	}
	WakeByAddressAll(addr);
	return expected;
	#else
	return (int)syscall(SYS_futex, (int *)addr, FUTEX_WAIT, expected, NULL, NULL, 0);
	#endif
}
