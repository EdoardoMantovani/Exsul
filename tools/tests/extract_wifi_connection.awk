# Extract the production functions, without the unrelated kernel ABI adapter.
# The test supplies driver/skb fixtures; it never maintains a copy of the FSM.
BEGIN {
	n = split("connection_get_vif connection_get connection_now_ms connection_runtime_init connection_set_state connection_queue_event connection_tx_status_observe connection_get_be16 connection_get_be64 connection_put_be16 connection_put_be64 connection_hmac_sha1 connection_derive_ptk connection_aes_unwrap connection_send_frame wifi_connection_send_auth connection_send_assoc connection_retry_worker connection_start_retry_worker connection_build_key_eapol connection_find_gtk connection_handle_m1 connection_handle_m3 connection_handle_eapol connection_finish_assoc connection_rx_process connection_rx_observe", names, " ");
	for (i = 1; i <= n; i++) wanted[names[i]] = 1;
	wanted["connection_tx_status_process"] = 1;
	wanted["connection_stop_locked"] = 1;
	wanted["connection_free_retired"] = 1;
	wanted["wifi_connection_stop"] = 1;
}
/^#define CONNECTION_/ { print; next }
/^(enum connection_(state|tx_kind)|struct connection_(fsm|event|retry_job)) \{/ { emit = 1 }
/^static const u8 connection_(rsn|wmm)_ie\[\]/ { emit = 1 }
/^static / && !/;$/ {
	name = $0;
	sub(/\(.*/, "", name);
	sub(/^.*[ *]/, "", name);
	if (name in wanted) { emit = 1; found[name] = 1 }
}
emit { print }
emit && /^}/ { emit = 0; print "" }
END {
	for (name in wanted)
		if (!(name in found)) {
			print "#error Missing production function " name;
			exit 1;
		}
}
