void ieee80211_unregister_hw(struct ieee80211_hw *hw){
	struct ieee80211_ops *ops = NULL;
	struct ieee80211_vif *vif = NULL;
	struct ieee80211_local *local = NULL;
	struct cfg80211_registered_device *rdev = NULL;
	void (*stop)(struct ieee80211_hw *hw, bool suspend) = NULL;
	void (*cancel_hw_scan)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	void (*remove_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;

	local = director.ieee80211_local;
	if(local == NULL || hw != local->ieee80211_hw){
		goto end;
	}
	peer_runtime_stop();
	ops = local->ieee80211_ops;
	wifi_connection_stop(local);
	if(local->ieee80211_ap_vif != NULL){
		mac80211_ap_stop(local);
	}
	if(ops != NULL){
		if(atomic_exchange(&director.scan_in_progress, false)){
			cancel_hw_scan = ops->cancel_hw_scan;
			if(cancel_hw_scan != NULL && local->ieee80211_vif != NULL){
				cancel_hw_scan(hw, local->ieee80211_vif);
			}
		}
		for(int i = 0; i < 3; i++){
			if(!(director.mac80211_interfaces & BIT(i))){
				continue;
			}
			director.mac80211_interfaces &= ~BIT(i);
			vif = i == 0 ? local->ieee80211_vif : i == 1 ? local->ieee80211_ap_vif : local->ieee80211_mon_vif;
			remove_interface = ops->remove_interface;
			if(vif != NULL && remove_interface != NULL){
				remove_interface(hw, vif);
			}
		}
		if(director.mac80211_started){
			director.mac80211_started = false;
			stop = ops->stop;
			if(stop != NULL){
				stop(hw, false);
			}
		}
	}
	if(local->workqueue != NULL){
		destroy_workqueue(local->workqueue);
		local->workqueue = NULL;
	}
	rdev = director.rdev;
	if(rdev != NULL){
		cancel_work_sync(&rdev->wiphy_work);
	}
	end:
}
