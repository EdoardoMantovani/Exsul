unsigned long _find_first_zero_bit(const unsigned long *addr, unsigned long size){
	return FIND_FIRST_BIT(~addr[idx], /* nop */, size);
}
