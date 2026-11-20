void sort_nonatomic(void *base, size_t num, size_t size, cmp_func_t cmp_func, swap_func_t swap_func){
	struct wrapper {
		cmp_func_t cmp;
		swap_func_t swap;
	} w = {
		.cmp  = cmp_func,
		.swap = swap_func,
	};

	return __sort_r(base, num, size, _CMP_WRAPPER, SWAP_WRAPPER, &w, true);
}
