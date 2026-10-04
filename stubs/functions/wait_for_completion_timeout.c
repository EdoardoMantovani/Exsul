unsigned long wait_for_completion_timeout(struct completion *x, unsigned long timeout){
	int ret = 0;
	int cur = 0;
	u64 start_ns = 0;
	u64 deadline_ns = 0;
	u64 now_ns = 0;
	u64 rem_ns = 0;
	u64 v = 0;
	u64 one_jiffy_in_ns = 0;
	struct timespec rel_timeout = {};

	if (x == NULL || timeout == 0) {
		return ret;
	}

	if (try_consume_completion(x)) {
		ret = timeout;
		return ret;
	}

	start_ns = nsec_now_monotonic();
	deadline_ns = start_ns + ((u64)timeout * NSEC_PER_SEC + HZ - 1) / HZ;

	for (;;) {
		if (try_consume_completion(x)) {
			now_ns = nsec_now_monotonic();
			if (now_ns >= deadline_ns) {
				ret = true;
				return ret;
			}

			rem_ns = deadline_ns - now_ns;
			one_jiffy_in_ns = (NSEC_PER_SEC + HZ - 1) / HZ;
			v = (rem_ns + one_jiffy_in_ns - 1) / one_jiffy_in_ns;
			if (v == 0) {
				ret = true;
			}
			if (v > ULONG_MAX) {
				ret = ULONG_MAX;
			}else{
				ret = v;
			}
			return ret;
		}

		now_ns = nsec_now_monotonic();
		if (now_ns >= deadline_ns) {
			return ret;
		}

		rem_ns = deadline_ns - now_ns;
		rel_timeout.tv_sec = (time_t)(rem_ns / NSEC_PER_SEC);
		rel_timeout.tv_nsec = (long)(rem_ns % NSEC_PER_SEC);

		cur = atomic_load_explicit(&x->done, memory_order_acquire);
		if (cur != 0) {
			continue;
		}

		ret = futex_wait_timeout(&x->done, 0, &rel_timeout);
		if (ret < 0) {
			if (errno == EINTR || errno == EAGAIN) {
				continue;
			}
			if (errno == ETIMEDOUT) {
				return 0;
			}
			return 0;
		}
	}
}
