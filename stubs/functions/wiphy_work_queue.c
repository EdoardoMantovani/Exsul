void wiphy_work_queue(struct wiphy *wiphy, struct wiphy_work *work){
	bool inserted = false;
	pthread_mutex_t *lock = wiphy_work_queue_lock();

	if(atomic_load(&director.finalizing) || lock == NULL || work == NULL || director.rdev == NULL || work->func == NULL || (uintptr_t)work->func <= 4096){
		goto end;
	}

	pthread_mutex_lock(lock);
	if(work->entry.next == NULL || work->entry.prev == NULL){
		INIT_LIST_HEAD(&work->entry);
	}
	if(list_empty(&work->entry)){
		list_add_tail(&work->entry, &director.rdev->wiphy_work_list);
		inserted = true;
	}
	pthread_mutex_unlock(lock);
	if(inserted){
		queue_work(&director.system_dfl_wq, &director.rdev->wiphy_work);
	}
	end:
}
