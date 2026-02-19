unsigned long __find_nth_and_bit(const unsigned long *addr1, const unsigned long *addr2, unsigned long size, unsigned long n){
	return FIND_NTH_BIT(addr1[idx] & addr2[idx], size, n);
}
