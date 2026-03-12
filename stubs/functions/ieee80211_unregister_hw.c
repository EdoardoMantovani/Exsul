void ieee80211_unregister_hw(struct ieee80211_hw *hw){
	struct ieee80211_local *local = NULL;

	local = &director.ieee80211_local;
	#if 0

	/*
	 * At this point, interface list manipulations are fine
	 * because the driver cannot be handing us frames any
	 * more and the tasklet is killed.
	 */
	ieee80211_remove_interfaces(local);

	ieee80211_txq_teardown_flows(local);

	wiphy_delayed_work_cancel(local->hw.wiphy, &local->roc_work);
	wiphy_work_cancel(local->hw.wiphy, &local->reconfig_filter);
	wiphy_work_cancel(local->hw.wiphy, &local->sched_scan_stopped_work);
	wiphy_work_cancel(local->hw.wiphy, &local->radar_detected_work);
	wiphy_unlock(local->hw.wiphy);

	cancel_work_sync(&local->restart_work);

	ieee80211_clear_tx_pending(local);
	rate_control_deinitialize(local);

	if (skb_queue_len(&local->skb_queue) ||
	    skb_queue_len(&local->skb_queue_unreliable))
		wiphy_warn(local->hw.wiphy, "skb_queue not empty\n");
	skb_queue_purge(&local->skb_queue);
	skb_queue_purge(&local->skb_queue_unreliable);

	wiphy_unregister(local->hw.wiphy);
	destroy_workqueue(local->workqueue);
	ieee80211_led_exit(local);
	kfree(local->int_scan_req);
	#endif
}
