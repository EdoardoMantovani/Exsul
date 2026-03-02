void complete(struct completion *x){
	atomic_fetch_add_explicit(&x->done, 1, memory_order_release);
	futex_wake(&x->done, 1, 0, 0);
}
