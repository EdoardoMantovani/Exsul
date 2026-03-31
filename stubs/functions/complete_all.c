void complete_all(struct completion *x){
	atomic_store_explicit(&x->done, INT_MAX, memory_order_release);
	futex_wake(&x->done, INT_MAX, 0, 0);
}
