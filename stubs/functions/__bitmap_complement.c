void __bitmap_complement(unsigned long *dst, const unsigned long *src, unsigned int bits){
	unsigned int lim = 0;

	lim = BITS_TO_LONGS(bits);
	for (int k = 0; k < lim; ++k){
		dst[k] = ~src[k];
	}
}
