unsigned long __sw_hweight64(__u64 w){
#if BITS_PER_LONG == 32
	return __sw_hweight32((unsigned int)(w >> 32)) +
	       __sw_hweight32((unsigned int)w);
#elif BITS_PER_LONG == 64
#ifdef CONFIG_ARCH_HAS_FAST_MULTIPLIER
	w -= (w >> 1) & 0x5555555555555555ul;
	w =  (w & 0x3333333333333333ul) + ((w >> 2) & 0x3333333333333333ul);
	w =  (w + (w >> 4)) & 0x0f0f0f0f0f0f0f0ful;
	return (w * 0x0101010101010101ul) >> 56;
#else
	__u64 res = w - ((w >> 1) & 0x5555555555555555ul);
	res = (res & 0x3333333333333333ul) + ((res >> 2) & 0x3333333333333333ul);
	res = (res + (res >> 4)) & 0x0F0F0F0F0F0F0F0Ful;
	res = res + (res >> 8);
	res = res + (res >> 16);
	return (res + (res >> 32)) & 0x00000000000000FFul;
#endif
#endif
}
