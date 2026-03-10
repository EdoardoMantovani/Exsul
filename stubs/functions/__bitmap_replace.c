void __bitmap_replace(unsigned long *dst, unsigned long *old, const unsigned long *new, unsigned long *mask, unsigned int nbits){
	unsigned int k;
	unsigned int nr = BITS_TO_LONGS(nbits);

	for (k = 0; k < nr; k++){
		dst[k] = (old[k] & ~mask[k]) | (new[k] & mask[k]);
	}
}
