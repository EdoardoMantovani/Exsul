void init_completion(struct completion *x){
	atomic_store_explicit(&x->done, 0, memory_order_relaxed);
}
