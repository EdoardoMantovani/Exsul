/* header for the standalone ELF/PE API */

void *get_director(void){
	return &director;
}

/* get_ieee80211_local: return the pointer to the internal 'ieee80211_local' struct */
void *get_ieee80211_local(void){
	return director.ieee80211_local;
}

/* get_wiphy: return the current module's 'wiphy' struct */
void *get_wiphy(void){
	return director.wiphy;
}

/* get_wireless_dev: return the current module's 'wireless_dev' struct */
void *get_wireless_dev(void){
	return director.wdev;
}

/* get_net_device: return the current module's 'net_device' struct */
void *get_net_device(void){
	return director.net_device;
}

/* get_ieee80211_hw: used in the standalone ELF api for getting the pointer of the struct 'ieee80211_hw' used in the module */
void *get_ieee80211_hw(void){
	if(director.ieee80211_local == NULL){
		return NULL;
	}
	return director.ieee80211_local->ieee80211_hw;
}

/* get_ieee80211_priv_data: get the 'ieee80211_hw->priv' ptr */
void *get_ieee80211_priv_data(void){
	void *hw = NULL;

	hw = get_ieee80211_hw();
	if(hw == NULL){
		return NULL;
	}
	return ieee80211_priv(hw);
}

/* skb_get_data: API function for the standalone ELF which permits to get the 'data' field fo the 'skb', not possible from the tcc scripting environment */
void *skb_get_data(struct sk_buff *skb){
	if(skb == NULL){
		return NULL;
	}
	return skb->data;
}

/* wifi_scan_stop: stop periodic scans and let the active hardware scan finish */
int wifi_scan_stop(void){
	int ret;

	stop_wifi_scan_thread();
	ret = connection_leave_idle(director.ieee80211_local);
	if (ret)
		return ret;
	/* Calling a driver's cancel_hw_scan callback directly bypasses mac80211's
	 * scan state machine and can leave its firmware command path quiesced. */
	for (int i = 0; i < 140 && atomic_load(&director.scan_in_progress); i++){
		usleep(100000);
	}
	return atomic_load(&director.scan_in_progress) ? -EBUSY : 0;
}

/* wifi_scan_once: start one hardware scan and wait for its completion callback */
int wifi_scan_once(unsigned int timeout_ms){
	struct ieee80211_local *local = director.ieee80211_local;
	struct ieee80211_ops *ops = director.ieee80211_ops;
	unsigned int elapsed_ms = 0;
	int ret;

	if (local == NULL || local->ieee80211_hw == NULL ||
	    local->ieee80211_vif == NULL || ops == NULL || ops->hw_scan == NULL ||
	    timeout_ms == 0)
		return -EINVAL;
	if (atomic_load(&director.scan_in_progress))
		return -EBUSY;

	ret = connection_leave_idle(local);
	if (ret)
		return ret;
	ret = start_hw_scan(local, ops->hw_scan, ops->cancel_hw_scan);
	if (ret)
		return ret;

	while (atomic_load(&director.scan_in_progress) && elapsed_ms < timeout_ms){
		unsigned int sleep_ms = timeout_ms - elapsed_ms;

		if (sleep_ms > 100)
			sleep_ms = 100;
		usleep(sleep_ms * 1000);
		elapsed_ms += sleep_ms;
	}
	if (atomic_load(&director.scan_in_progress)){
		return -ETIMEDOUT;
	}
	return test_bit(SCAN_ABORTED, &local->scanning) ? -ECANCELED : 0;
}

/* elf_reset_usb: reset the usb device with 'libusb_reset_device' */
int elf_reset_usb(void){
	int ret = 0;

	if(director.usb_dev != NULL){
		ret = libusb_reset_device(director.usb_dev);
	}else{
		ret = -EINVAL;
	}
	return ret;
}
