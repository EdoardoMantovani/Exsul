void __bitmap_shift_left(unsigned long *dst, const unsigned long *src, unsigned int shift, unsigned int nbits){
	int k = 0;
	unsigned int lim = 0;
	unsigned int off = 0;
	unsigned int rem = 0;
	unsigned long upper = 0;
	unsigned long lower = 0;

	lim = BITS_TO_LONGS(nbits);
	off = shift / BITS_PER_LONG;
	rem = shift % BITS_PER_LONG;
	for (k = lim - off - 1; k >= 0; --k) {
		upper = 0;
		lower = 0;

		/*
		 * If shift is not word aligned, take upper rem bits of
		 * word below and make them the bottom rem bits of result.
		 */
		if (rem && k > 0){
			lower = src[k - 1] >> (BITS_PER_LONG - rem);
		}else{
			lower = 0;
		}
		upper = src[k] << rem;
		dst[k + off] = lower | upper;
	}
	if (off){
		memset(dst, 0, off * sizeof(unsigned long));
	}
}
