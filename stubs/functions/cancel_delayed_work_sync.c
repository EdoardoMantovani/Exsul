int cancel_delayed_work_sync(struct delayed_work *dwork){
	int ret = 0;

	if (dwork == NULL){
		return ret;
	}
	ret = cancel_delayed_work(dwork);
	flush_work(&dwork->work);
	return ret;
}
