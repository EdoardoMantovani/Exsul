void cfg80211_scan_done(void *request, const void *info){
	atomic_store(&director.scan_in_progress, false);
	scan_cache_schedule_print();
}
