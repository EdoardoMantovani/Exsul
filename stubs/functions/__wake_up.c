int __wake_up(struct wait_queue_head *wq_head, unsigned int mode, int nr_exclusive, void *key){
	int ret = 0;
	int wake_up_executed = 0;
	unsigned long flags = 0;
	struct wait_queue_entry *pos = NULL;
	struct wait_queue_entry *n = NULL;

	waitqueue_lock(&wq_head->lock);

	list_for_each_entry_safe(pos, n, &wq_head->head, entry) {
		flags = pos->flags;

		if (pos->func != NULL){
			ret = pos->func(pos, mode, 0, key);
		}
		if (ret > 0 && (flags & WQ_FLAG_EXCLUSIVE)){
			wake_up_executed++;
		}
		if (nr_exclusive > 0 && wake_up_executed >= nr_exclusive){
			break;
		}
	}

	waitqueue_unlock(&wq_head->lock);
	return wake_up_executed;
}
