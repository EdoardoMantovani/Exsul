bool __bitmap_intersects(unsigned long *bitmap1, unsigned long *bitmap2, unsigned int bits){
	unsigned int k, lim = bits/BITS_PER_LONG;

	for (k = 0; k < lim; ++k){
		if (bitmap1[k] & bitmap2[k]){
			return true;
		}
	}

	if (bits % BITS_PER_LONG){
		if ((bitmap1[k] & bitmap2[k]) & BITMAP_LAST_WORD_MASK(bits)){
			return true;
		}
	}

	return false;
}
