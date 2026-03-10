void __bitmap_xor(unsigned long *dst, unsigned long *bitmap1, unsigned long *bitmap2, unsigned int bits){
	unsigned int k;
	unsigned int nr = BITS_TO_LONGS(bits);

	for (k = 0; k < nr; k++){
		dst[k] = bitmap1[k] ^ bitmap2[k];
	}
}
