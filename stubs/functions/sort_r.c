void sort_r(void *base, size_t num, size_t size, cmp_r_func_t cmp_func, swap_r_func_t swap_func, const void *priv){
	__sort_r(base, num, size, cmp_func, swap_func, priv, false);
}
