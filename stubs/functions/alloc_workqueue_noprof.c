struct workqueue_struct *alloc_workqueue_noprof(const char *fmt, unsigned int flags, int max_active){
	struct workqueue_struct *wq = NULL;
	printf("neter!\n");
	wq = malloc(sizeof(*wq));
	//wq = alloc_workqueue_noprof(fmt, flags, max_active);
	return wq;
}
