long prepare_to_wait_event(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry, int state){
	int ret = 0;
	struct task_struct *task_struct = NULL;

	task_struct = current;
	if (task_struct == NULL){
		ret = -ENOMEM;
		return ret;
	}

	if ((state & TASK_INTERRUPTIBLE) && atomic_load(&task_struct->should_stop)){
		ret = -ERESTART;
		return ret;
	}

	spin_lock(&wq_head->lock);
	if (!wq_entry_is_linked(wq_entry)){
		list_add_tail(&wq_entry->entry, &wq_head->head);
	}
	atomic_store(&task_struct->__state, state);
	spin_unlock(&wq_head->lock);
	return ret;
}
