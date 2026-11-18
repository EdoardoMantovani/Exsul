bool completion_done(struct completion *x){
	if (x == NULL) {
		return true;
	}
	if (!atomic_load_explicit(&x->done, memory_order_acquire)) {
		return false;
	}
	atomic_thread_fence(memory_order_seq_cst);
	return true;
}
