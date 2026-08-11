void __bitmap_shift_right(unsigned long *dst, const unsigned long *src, unsigned shift, unsigned nbits){
	unsigned int lim = 0;
	unsigned int off = 0;
	unsigned int rem = 0;
	unsigned long mask = 0;
	unsigned long upper = 0;
	unsigned long lower = 0;

	lim = BITS_TO_LONGS(nbits);
	off = shift / BITS_PER_LONG;
	rem = shift % BITS_PER_LONG;
	mask = BITMAP_LAST_WORD_MASK(nbits);
	for (int k = 0; off + k < lim; ++k) {
		upper = 0;
		lower = 0;

		/*
		 * If shift is not word aligned, take lower rem bits of
		 * word above and make them the top rem bits of result.
		 */
		if (!rem || off + k + 1 >= lim){
			upper = 0;
		}else {
			upper = src[off + k + 1];
			if (off + k + 1 == lim - 1){
				upper &= mask;
			}
			upper <<= (BITS_PER_LONG - rem);
		}
		lower = src[off + k];
		if (off + k == lim - 1){
			lower &= mask;
		}
		lower >>= rem;
		dst[k] = lower | upper;
	}
	if (off){
		memset(&dst[lim - off], 0, off * sizeof(unsigned long));
	}
}
