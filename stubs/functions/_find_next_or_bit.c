unsigned long _find_next_or_bit(const unsigned long *addr1, const unsigned long *addr2, unsigned long nbits, unsigned long start){
	return FIND_NEXT_BIT(addr1[idx] | addr2[idx], /* nop */, nbits, start);
}
