int timer_delete_sync(struct timer_list *timer){
	int ret = 0;

	ret = __timer_delete_sync(timer, false);
	return ret;
}
