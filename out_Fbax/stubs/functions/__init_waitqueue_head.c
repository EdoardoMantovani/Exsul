void __init_waitqueue_head(struct wait_queue_head *wq_head, const char *name, void *key){
	spin_lock_init(&wq_head->lock);
	INIT_LIST_HEAD(&wq_head->head);
}
