bool __bitmap_andnot(unsigned long *dst, unsigned long *bitmap1, unsigned long *bitmap2, unsigned int bits){
	unsigned int k = 0;
	unsigned int lim = bits/BITS_PER_LONG;
	unsigned long result = 0;

	for (k = 0; k < lim; k++){
		result |= (dst[k] = bitmap1[k] & ~bitmap2[k]);
	}
	if (bits % BITS_PER_LONG){
		result |= (dst[k] = bitmap1[k] & ~bitmap2[k] & BITMAP_LAST_WORD_MASK(bits));
	}

	return result != 0;
}

