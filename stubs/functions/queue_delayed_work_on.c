int queue_delayed_work_on(int cpu, struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay){
	int ret = 0;
	long expected = 0;
	long desired = 0;

	if (wq == NULL || dwork == NULL){
		return ret;
	}

	if(delay == 0){
		ret = queue_work_on(cpu, wq, &dwork->work);
		return ret;
	}

	expected = atomic_load_explicit(&dwork->work.data, memory_order_acquire);
	if (expected & (WORK_STRUCT_PENDING | WORK_STRUCT_RUNNING)) {
		return 0;
	}
	desired = work_encode(wq, WORK_STRUCT_PENDING | WORK_STRUCT_DELAYED);
	ret = atomic_compare_exchange_strong_explicit(&dwork->work.data, &expected, desired, memory_order_acq_rel, memory_order_relaxed);

	if(ret == 0){
		return ret;
	}

	pthread_mutex_lock(&wq->lock);

	if (wq->stop) {
		pthread_mutex_unlock(&wq->lock);
		atomic_store_explicit(&dwork->work.data, 0, memory_order_release);
		return ret;
	}

	dwork->expires = timespec_add_ms(wq_now(wq), delay);

	delayed_list_insert_sorted(wq, dwork);

	pthread_cond_signal(&wq->cond);
	pthread_mutex_unlock(&wq->lock);
	ret = true;
	return ret;
}
