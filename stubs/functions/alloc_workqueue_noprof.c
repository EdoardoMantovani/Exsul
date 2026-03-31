struct workqueue_struct *alloc_workqueue_noprof(const char *fmt, unsigned int flags, int max_active){
	struct workqueue_struct *wq = NULL;
	wq = alloc_workqueue(fmt, flags, max_active);
	return wq;
}
