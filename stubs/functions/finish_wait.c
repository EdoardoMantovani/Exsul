void finish_wait(struct wait_queue_head *wq_head, struct wait_queue_entry *wq_entry){
	struct __task_struct *task_struct = NULL;
	task_struct = current;

	waitqueue_lock(&wq_head->lock);
	if (wq_entry_is_linked(wq_entry) == true){
		list_del_init(&wq_entry->entry);
	}
	waitqueue_unlock(&wq_head->lock);
	if (task_struct != NULL){
		atomic_store(&task_struct->__state, TASK_RUNNING);
	}
}
