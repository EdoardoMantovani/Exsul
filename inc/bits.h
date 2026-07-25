/* header used for bit manipulation */

/*
 * small_const_nbits(n) is true precisely when it is known at compile-time
 * that BITMAP_SIZE(n) is 1, i.e. 1 <= n <= BITS_PER_LONG. This allows
 * various bit/bitmap APIs to provide a fast inline implementation. Bitmaps
 * of size 0 are very rare, and a compile-time-known-size 0 is most likely
 * a sign of error. They will be handled correctly by the bit/bitmap APIs,
 * but using the out-of-line functions, so that the inline implementations
 * can unconditionally dereference the pointer(s).
 */
#define small_const_nbits(nbits)	(__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG && (nbits) > 0)
#define for_each_set_bit(bit, addr, size) for ((bit) = 0; (bit) = find_next_bit((addr), (size), (bit)), (bit) < (size); (bit)++)

static inline bool bitmap_intersects(unsigned long *src1, unsigned long *src2, unsigned int nbits){
	if (small_const_nbits(nbits)){
		return ((*src1 & *src2) & BITMAP_LAST_WORD_MASK(nbits)) != 0;
	}else{
		return __bitmap_intersects(src1, src2, nbits);
	}
}

static inline bool bitmap_subset(unsigned long *src1, unsigned long *src2, unsigned int nbits){
	if (small_const_nbits(nbits)){
		return ! ((*src1 & ~(*src2)) & BITMAP_LAST_WORD_MASK(nbits));
	}else{
		return __bitmap_subset(src1, src2, nbits);
	}
}

static inline void linkmode_set_bit_array(int *array, int array_size, unsigned long *addr){
	for (int i = 0; i < array_size; i++){
		linkmode_set_bit(array[i], addr);
	}
}

static inline int linkmode_equal(unsigned long *src1, unsigned long *src2){
	return bitmap_equal(src1, src2, __ETHTOOL_LINK_MODE_MASK_NBITS);
}

static inline int linkmode_intersects(unsigned long *src1, unsigned long *src2){
	return bitmap_intersects(src1, src2, __ETHTOOL_LINK_MODE_MASK_NBITS);
}

static inline int linkmode_subset(unsigned long *src1, unsigned long *src2){
	return bitmap_subset(src1, src2, __ETHTOOL_LINK_MODE_MASK_NBITS);
}

/*
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @size: The bitmap size in bits
 * @offset: The bitnumber to start searching at
 *
 * Returns the bit number for the next set bit
 * If no bits are set, returns @size.
 */
static inline unsigned long find_next_bit(const unsigned long *addr, unsigned long size, unsigned long offset){
	unsigned long val = 0;

	if (small_const_nbits(size)) {
		if (unlikely(offset >= size)){
			return size;
		}
		val = *addr & GENMASK(size - 1, offset);
		return val ? __ffs(val) : size;
	}

	return _find_next_bit(addr, size, offset);
}
