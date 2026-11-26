void device_wakeup_disable(struct device *device){
	struct wakeup_source *ws = NULL;

	if (device == NULL){
		return;
	}
	#if 0
	ws = device_wakeup_detach(dev);
	#endif
}
