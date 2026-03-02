void wait_for_completion(struct completion *x){
	int ret = 0;
	int cur = 0;

	for (;;) {
		if (try_consume_completion(x)){
			return;
		}

		cur = atomic_load_explicit(&x->done, memory_order_acquire);
		if (cur == 0) {
			ret = futex_wait(&x->done, 0);
			if (ret < 0 && errno == EINTR) {
				continue;
			}
		}
	}
}
