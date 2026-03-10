/* header used for bit manipulation */

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
