void ieee80211_rx_list(struct ieee80211_hw *hw, struct ieee80211_sta *pubsta, struct sk_buff *skb, struct list_head *list){
	struct wiphy *wiphy = NULL;
	struct ieee80211_rate *rate = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_supported_band *sband = NULL;
	struct ieee80211_rx_status *status = NULL;
	struct ieee80211_hdr *hdr = NULL;

	if (skb == NULL || IS_ERR(skb)){
		goto end;
	}
	status = IEEE80211_SKB_RXCB(skb);
	if (!status || IS_ERR(status)){
		goto drop;
	}
	hdr = (struct ieee80211_hdr *)skb->data;

	local = director.ieee80211_local;
	if (status->band >= NUM_NL80211_BANDS){
		goto drop;
	}

	wiphy = local->wiphy;
	sband = wiphy->bands[status->band];
	if (sband == NULL){
		goto drop;
	}

	/*
	 * If we're suspending, it is possible although not too likely
	 * that we'd be receiving frames after having already partially
	 * quiesced the stack. We can't process such frames then since
	 * that might, for example, cause stations to be added or other
	 * driver callbacks be invoked.
	 */
	if (unlikely(local->quiescing || local->suspended)){
		goto drop;
	}

	/* We might be during a HW reconfig, prevent Rx for the same reason */
	if (unlikely(local->in_reconfig)){
		goto drop;
	}

	if (!local->started){
		goto drop;
	}

	if (likely(!(status->flag & RX_FLAG_FAILED_PLCP_CRC) && !(status->flag & RX_FLAG_NO_PSDU && status->zero_length_psdu_type == IEEE80211_RADIOTAP_ZERO_LEN_PSDU_NOT_CAPTURED))) {
		/*
		 * Validate the rate, unless there was a PLCP error which may
		 * have an invalid rate or the PSDU was not capture and may be
		 * missing rate information.
		 */

		switch (status->encoding) {
		case RX_ENC_HT:
			/*
			 * rate_idx is MCS index, which can be [0-76]
			 * as documented on:
			 *
			 * https://wireless.wiki.kernel.org/en/developers/Documentation/ieee80211/802.11n
			 *
			 * Anything else would be some sort of driver or
			 * hardware error. The driver should catch hardware
			 * errors.
			 */
			if (status->rate_idx > 76){
				goto drop;
			}
			break;
		case RX_ENC_VHT:
			if (status->rate_idx > 11 || !status->nss || status->nss > 8){
				goto drop;
			}
			break;
		case RX_ENC_HE:
			if (status->rate_idx > 11 || !status->nss || status->nss > 8){
				goto drop;
			}
			break;
		case RX_ENC_EHT:
			if (status->rate_idx > 15 || !status->nss || status->nss > 8 || status->eht.gi > NL80211_RATE_INFO_EHT_GI_3_2){
				goto drop;
			}
			break;
		default:
			fallthrough;
		case RX_ENC_LEGACY:
			if (status->rate_idx >= sband->n_bitrates){
				goto drop;
			}
			rate = &sband->bitrates[status->rate_idx];
		}
	}

	if (status->link_id >= IEEE80211_LINK_UNSPECIFIED){
		goto drop;
	}
	status->rx_flags = 0;

	/*
	 * Frames with failed FCS/PLCP checksum are not returned,
	 * all other frames are returned without radiotap header
	 * if it was previously present.
	 * Also, frames with less than 16 bytes are dropped.
	 */
	if (!(status->flag & RX_FLAG_8023)){
		skb = ieee80211_rx_monitor(local, skb, rate);
	}
	if (skb) {
		if ((status->flag & RX_FLAG_8023) || ieee80211_is_data_present(hdr->frame_control)){
			ieee80211_tpt_led_trig_rx(local, skb->len);
		}
		if (status->flag & RX_FLAG_8023){
			__ieee80211_rx_handle_8023(hw, pubsta, skb, list);
		}else{
			__ieee80211_rx_handle_packet(hw, pubsta, skb, list);
		}
	}

	return;
	drop:
	kfree_skb(skb);
	end:
}
