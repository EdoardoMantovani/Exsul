int autoremove_wake_function(struct wait_queue_entry *wq_entry, unsigned mode, int sync, void *key){
	int ret = true;
	struct task_struct *task_struct = NULL;

	if (wq_entry_is_linked(wq_entry) == true){
		list_del_init(&wq_entry->entry);
	}

	task_struct = (struct task_struct *)wq_entry->private;

	if (task_struct != NULL){
		wake_up_process(task_struct);
	}

	return ret;
}
