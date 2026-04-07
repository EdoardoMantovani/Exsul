void ieee80211_csa_finish(struct ieee80211_vif *vif, unsigned int link_id){
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_bss_conf *tx_bss_conf = NULL;
	struct ieee80211_link_data *link_data = NULL;

	local = director.ieee80211_local;
	sdata = local->sdata;
	if(sdata == NULL){
		goto end;
	}

	if (link_id >= IEEE80211_MLD_MAX_NUM_LINKS){
		return;
	}

	link_data = rcu_dereference(sdata->link[link_id]);
	if (!link_data) {
		return;
	}

	tx_bss_conf = rcu_dereference(link_data->conf->tx_bss_conf);
	if (tx_bss_conf == link_data->conf) {
		/* Trigger ieee80211_csa_finish() on the non-transmitting
		 * interfaces when channel switch is received on
		 * transmitting interface
		 */
		struct ieee80211_link_data *iter = NULL;

		for_each_sdata_link_rcu(local, iter) {
			if (iter->sdata == sdata || rcu_access_pointer(iter->conf->tx_bss_conf) != tx_bss_conf){
				continue;
			}
			wiphy_work_queue(local->wiphy, &iter->csa.finalize_work);
		}
	}

	wiphy_work_queue(local->wiphy, &link_data->csa.finalize_work);
	end:
}
