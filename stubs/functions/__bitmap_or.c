void __bitmap_or(unsigned long *dst, const unsigned long *bitmap1, const unsigned long *bitmap2, unsigned int bits){
	unsigned int k = 0;
	unsigned int nr = 0;

	nr = BITS_TO_LONGS(bits);
	for (k = 0; k < nr; k++){
		dst[k] = bitmap1[k] | bitmap2[k];
	}
}
