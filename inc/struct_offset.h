struct struct_field {
	unsigned char  field_name[64];
	unsigned short field_offset;
};

struct runtime_struct_layout {
	short struct_size;
	unsigned char *struct_name;
	unsigned short struct_field_num;
	struct struct_field fields[128];
};

struct runtime_struct {
	int compilation_time_generated_structs;
	struct runtime_struct_layout structs[40 + 1];
};

OFFSET_TABLE_SECTION static struct runtime_struct runtime_generated_structs = {
	.structs = {
			[0] = {
				.struct_size = 0,
				.struct_name = "cfg80211_ap_settings",
				.struct_field_num = 9,
				.fields = {
					[0] = {
						.field_name = "auth_type",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "beacon",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "beacon_interval",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "chandef",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "dtim_period",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "hidden_ssid",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "privacy",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "ssid",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "ssid_len",
						.field_offset = 0,
					},
				},
			},
			[1] = {
				.struct_size = 0,
				.struct_name = "cfg80211_beacon_data",
				.struct_field_num = 4,
				.fields = {
					[0] = {
						.field_name = "head",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "head_len",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "tail",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "tail_len",
						.field_offset = 0,
					},
				},
			},
			[2] = {
				.struct_size = 0,
				.struct_name = "cfg80211_chan_def",
				.struct_field_num = 5,
				.fields = {
					[0] = {
						.field_name = "center_freq1",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "center_freq2",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "chan",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "freq1_offset",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "width",
						.field_offset = 0,
					},
				},
			},
			[3] = {
				.struct_size = 0,
				.struct_name = "cfg80211_ops",
				.struct_field_num = 6,
				.fields = {
					[0] = {
						.field_name = "add_virtual_intf",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "change_virtual_intf",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "del_virtual_intf",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "scan",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "start_ap",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "stop_ap",
						.field_offset = 0,
					},
				},
			},
			[4] = {
				.struct_size = 0,
				.struct_name = "cfg80211_scan_request",
				.struct_field_num = 22,
				.fields = {
					[0] = {
						.field_name = "bssid",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "channels",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "duration",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "duration_mandatory",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "first_part",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "flags",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "ie",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "ie_len",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "mac_addr",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "mac_addr_mask",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "n_6ghz_params",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "n_channels",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "n_ssids",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "no_cck",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "rates",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "scan_6ghz",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "scan_6ghz_params",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "scan_start",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "ssids",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "tsf_report_link_id",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "wdev",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "wiphy",
						.field_offset = 0,
					},
				},
			},
			[5] = {
				.struct_size = 0,
				.struct_name = "device",
				.struct_field_num = 6,
				.fields = {
					[0] = {
						.field_name = "coherent_dma_mask",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "dma_mask",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "driver",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "driver_override",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "init_name",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "kobj",
						.field_offset = 0,
					},
				},
			},
			[6] = {
				.struct_size = 0,
				.struct_name = "device_driver",
				.struct_field_num = 1,
				.fields = {
					[0] = {
						.field_name = "name",
						.field_offset = 0,
					},
				},
			},
			[7] = {
				.struct_size = 0,
				.struct_name = "discovery_state",
				.struct_field_num = 9,
				.fields = {
					[0] = {
						.field_name = "all",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "report_invalid_rssi",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "resolve",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "result_filtering",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "rssi",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "state",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "unknown",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "uuid_count",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "uuids",
						.field_offset = 0,
					},
				},
			},
			[8] = {
				.struct_size = 0,
				.struct_name = "file",
				.struct_field_num = 3,
				.fields = {
					[0] = {
						.field_name = "f_flags",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "f_inode",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "f_pos",
						.field_offset = 0,
					},
				},
			},
			[9] = {
				.struct_size = 0,
				.struct_name = "hci_conn",
				.struct_field_num = 2,
				.fields = {
					[0] = {
						.field_name = "handle",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "type",
						.field_offset = 0,
					},
				},
			},
			[10] = {
				.struct_size = 0,
				.struct_name = "hci_dev",
				.struct_field_num = 126,
				.fields = {
					[0] = {
						.field_name = "accept_list",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "adv_instance_cnt",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "adv_instance_expire",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "adv_instance_timeout",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "adv_instances",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "adv_tx_power",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "advmon_allowlist_duration",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "advmon_no_filter_duration",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "auth_payload_timeout",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "blocked_keys",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "classify_pkt_type",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "close",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "cmd_q",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "cmd_sync_cancel_work",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "cmd_sync_work",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "cmd_sync_work_list",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "cmd_sync_work_lock",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "cmd_timer",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "cmd_work",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "conn_flags",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "conn_hash",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "conn_info_max_age",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "conn_info_min_age",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "cur_adv_instance",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "def_le_autoconnect_timeout",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "def_multi_adv_rotation_duration",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "def_page_scan_int",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "def_page_scan_type",
						.field_offset = 0,
					},
					[28] = {
						.field_name = "def_page_scan_window",
						.field_offset = 0,
					},
					[29] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "dev_flags",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "discov_interleaved_timeout",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "discovery",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "dump",
						.field_offset = 0,
					},
					[34] = {
						.field_name = "enable_advmon_interleave_scan",
						.field_offset = 0,
					},
					[35] = {
						.field_name = "error_reset",
						.field_offset = 0,
					},
					[36] = {
						.field_name = "esco_type",
						.field_offset = 0,
					},
					[37] = {
						.field_name = "flags",
						.field_offset = 0,
					},
					[38] = {
						.field_name = "fw_info",
						.field_offset = 0,
					},
					[39] = {
						.field_name = "hci_drv",
						.field_offset = 0,
					},
					[40] = {
						.field_name = "hw_info",
						.field_offset = 0,
					},
					[41] = {
						.field_name = "identity_resolving_keys",
						.field_offset = 0,
					},
					[42] = {
						.field_name = "inq_tx_power",
						.field_offset = 0,
					},
					[43] = {
						.field_name = "interleave_scan",
						.field_offset = 0,
					},
					[44] = {
						.field_name = "io_capability",
						.field_offset = 0,
					},
					[45] = {
						.field_name = "le_accept_list",
						.field_offset = 0,
					},
					[46] = {
						.field_name = "le_adv_channel_map",
						.field_offset = 0,
					},
					[47] = {
						.field_name = "le_adv_max_interval",
						.field_offset = 0,
					},
					[48] = {
						.field_name = "le_adv_min_interval",
						.field_offset = 0,
					},
					[49] = {
						.field_name = "le_conn_latency",
						.field_offset = 0,
					},
					[50] = {
						.field_name = "le_conn_max_interval",
						.field_offset = 0,
					},
					[51] = {
						.field_name = "le_conn_min_interval",
						.field_offset = 0,
					},
					[52] = {
						.field_name = "le_conn_params",
						.field_offset = 0,
					},
					[53] = {
						.field_name = "le_def_tx_len",
						.field_offset = 0,
					},
					[54] = {
						.field_name = "le_def_tx_time",
						.field_offset = 0,
					},
					[55] = {
						.field_name = "le_max_key_size",
						.field_offset = 0,
					},
					[56] = {
						.field_name = "le_max_rx_len",
						.field_offset = 0,
					},
					[57] = {
						.field_name = "le_max_rx_time",
						.field_offset = 0,
					},
					[58] = {
						.field_name = "le_max_tx_len",
						.field_offset = 0,
					},
					[59] = {
						.field_name = "le_max_tx_time",
						.field_offset = 0,
					},
					[60] = {
						.field_name = "le_min_key_size",
						.field_offset = 0,
					},
					[61] = {
						.field_name = "le_num_of_adv_sets",
						.field_offset = 0,
					},
					[62] = {
						.field_name = "le_resolv_list",
						.field_offset = 0,
					},
					[63] = {
						.field_name = "le_rx_def_phys",
						.field_offset = 0,
					},
					[64] = {
						.field_name = "le_scan_disable",
						.field_offset = 0,
					},
					[65] = {
						.field_name = "le_scan_int_adv_monitor",
						.field_offset = 0,
					},
					[66] = {
						.field_name = "le_scan_int_connect",
						.field_offset = 0,
					},
					[67] = {
						.field_name = "le_scan_int_discovery",
						.field_offset = 0,
					},
					[68] = {
						.field_name = "le_scan_int_suspend",
						.field_offset = 0,
					},
					[69] = {
						.field_name = "le_scan_interval",
						.field_offset = 0,
					},
					[70] = {
						.field_name = "le_scan_window",
						.field_offset = 0,
					},
					[71] = {
						.field_name = "le_scan_window_adv_monitor",
						.field_offset = 0,
					},
					[72] = {
						.field_name = "le_scan_window_connect",
						.field_offset = 0,
					},
					[73] = {
						.field_name = "le_scan_window_discovery",
						.field_offset = 0,
					},
					[74] = {
						.field_name = "le_scan_window_suspend",
						.field_offset = 0,
					},
					[75] = {
						.field_name = "le_supv_timeout",
						.field_offset = 0,
					},
					[76] = {
						.field_name = "le_tx_def_phys",
						.field_offset = 0,
					},
					[77] = {
						.field_name = "link_keys",
						.field_offset = 0,
					},
					[78] = {
						.field_name = "link_mode",
						.field_offset = 0,
					},
					[79] = {
						.field_name = "list",
						.field_offset = 0,
					},
					[80] = {
						.field_name = "local_codecs",
						.field_offset = 0,
					},
					[81] = {
						.field_name = "lock",
						.field_offset = 0,
					},
					[82] = {
						.field_name = "long_term_keys",
						.field_offset = 0,
					},
					[83] = {
						.field_name = "manufacturer",
						.field_offset = 0,
					},
					[84] = {
						.field_name = "max_le_tx_power",
						.field_offset = 0,
					},
					[85] = {
						.field_name = "mesh_pending",
						.field_offset = 0,
					},
					[86] = {
						.field_name = "mgmt_pending",
						.field_offset = 0,
					},
					[87] = {
						.field_name = "mgmt_pending_lock",
						.field_offset = 0,
					},
					[88] = {
						.field_name = "min_enc_key_size",
						.field_offset = 0,
					},
					[89] = {
						.field_name = "min_le_tx_power",
						.field_offset = 0,
					},
					[90] = {
						.field_name = "monitored_devices",
						.field_offset = 0,
					},
					[91] = {
						.field_name = "name",
						.field_offset = 0,
					},
					[92] = {
						.field_name = "ncmd_timer",
						.field_offset = 0,
					},
					[93] = {
						.field_name = "num_iac",
						.field_offset = 0,
					},
					[94] = {
						.field_name = "open",
						.field_offset = 0,
					},
					[95] = {
						.field_name = "pend_le_conns",
						.field_offset = 0,
					},
					[96] = {
						.field_name = "pend_le_reports",
						.field_offset = 0,
					},
					[97] = {
						.field_name = "pkt_type",
						.field_offset = 0,
					},
					[98] = {
						.field_name = "power_off",
						.field_offset = 0,
					},
					[99] = {
						.field_name = "power_on",
						.field_offset = 0,
					},
					[100] = {
						.field_name = "promisc",
						.field_offset = 0,
					},
					[101] = {
						.field_name = "quirk_flags",
						.field_offset = 0,
					},
					[102] = {
						.field_name = "raw_q",
						.field_offset = 0,
					},
					[103] = {
						.field_name = "reenable_adv_work",
						.field_offset = 0,
					},
					[104] = {
						.field_name = "reject_list",
						.field_offset = 0,
					},
					[105] = {
						.field_name = "remote_oob_data",
						.field_offset = 0,
					},
					[106] = {
						.field_name = "req_lock",
						.field_offset = 0,
					},
					[107] = {
						.field_name = "req_result",
						.field_offset = 0,
					},
					[108] = {
						.field_name = "req_rsp",
						.field_offset = 0,
					},
					[109] = {
						.field_name = "req_skb",
						.field_offset = 0,
					},
					[110] = {
						.field_name = "req_status",
						.field_offset = 0,
					},
					[111] = {
						.field_name = "req_wait_q",
						.field_offset = 0,
					},
					[112] = {
						.field_name = "req_workqueue",
						.field_offset = 0,
					},
					[113] = {
						.field_name = "rpa_timeout",
						.field_offset = 0,
					},
					[114] = {
						.field_name = "rx_q",
						.field_offset = 0,
					},
					[115] = {
						.field_name = "rx_work",
						.field_offset = 0,
					},
					[116] = {
						.field_name = "send",
						.field_offset = 0,
					},
					[117] = {
						.field_name = "sent_cmd",
						.field_offset = 0,
					},
					[118] = {
						.field_name = "setup",
						.field_offset = 0,
					},
					[119] = {
						.field_name = "sniff_max_interval",
						.field_offset = 0,
					},
					[120] = {
						.field_name = "sniff_min_interval",
						.field_offset = 0,
					},
					[121] = {
						.field_name = "tx_work",
						.field_offset = 0,
					},
					[122] = {
						.field_name = "unregister_lock",
						.field_offset = 0,
					},
					[123] = {
						.field_name = "uuids",
						.field_offset = 0,
					},
					[124] = {
						.field_name = "wakeup",
						.field_offset = 0,
					},
					[125] = {
						.field_name = "workqueue",
						.field_offset = 0,
					},
				},
			},
			[11] = {
				.struct_size = 0,
				.struct_name = "ieee80211_bss_conf",
				.struct_field_num = 43,
				.fields = {
					[0] = {
						.field_name = "addr",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "allow_p2p_go_ps",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "arp_addr_cnt",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "assoc_capability",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "basic_rates",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "beacon_int",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "beacon_rate",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "bssid",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "chanctx_conf",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "chanreq",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "cqm_rssi_high",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "cqm_rssi_hyst",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "cqm_rssi_low",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "cqm_rssi_thold",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "dtim_period",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "eht_support",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "enable_beacon",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "frame_time_rts_th",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "ftm_responder",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "ftm_responder_params",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "he_support",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "hidden_ssid",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "ht_operation_mode",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "idle",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "link_id",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "mcast_rate",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "multicast_to_unicast",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "ps",
						.field_offset = 0,
					},
					[28] = {
						.field_name = "qos",
						.field_offset = 0,
					},
					[29] = {
						.field_name = "sync_device_ts",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "sync_dtim_count",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "sync_tsf",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "twt_broadcast",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "twt_protected",
						.field_offset = 0,
					},
					[34] = {
						.field_name = "twt_requester",
						.field_offset = 0,
					},
					[35] = {
						.field_name = "twt_responder",
						.field_offset = 0,
					},
					[36] = {
						.field_name = "tx_bss_conf",
						.field_offset = 0,
					},
					[37] = {
						.field_name = "txpower",
						.field_offset = 0,
					},
					[38] = {
						.field_name = "txpower_type",
						.field_offset = 0,
					},
					[39] = {
						.field_name = "use_cts_prot",
						.field_offset = 0,
					},
					[40] = {
						.field_name = "use_short_preamble",
						.field_offset = 0,
					},
					[41] = {
						.field_name = "use_short_slot",
						.field_offset = 0,
					},
					[42] = {
						.field_name = "vif",
						.field_offset = 0,
					},
				},
			},
			[12] = {
				.struct_size = 0,
				.struct_name = "ieee80211_chanctx_conf",
				.struct_field_num = 8,
				.fields = {
					[0] = {
						.field_name = "ap",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "conf",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "def",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "driver_present",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "min_def",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "radio_idx",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "rx_chains_dynamic",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "rx_chains_static",
						.field_offset = 0,
					},
				},
			},
			[13] = {
				.struct_size = 0,
				.struct_name = "ieee80211_hw",
				.struct_field_num = 28,
				.fields = {
					[0] = {
						.field_name = "chanctx_data_size",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "conf",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "extra_beacon_tailroom",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "extra_tx_headroom",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "flags",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "max_listen_interval",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "max_mtu",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "max_nan_de_entries",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "max_rates",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "max_report_rates",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "max_rx_aggregation_subframes",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "max_signal",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "max_tx_aggregation_subframes",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "netdev_features",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "offchannel_tx_hw_queue",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "priv",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "queues",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "radiotap_mcs_details",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "radiotap_vht_details",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "rate_control_algorithm",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "sta_data_size",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "tx_sk_pacing_shift",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "txq_data_size",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "uapsd_max_sp_len",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "uapsd_queues",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "vif_data_size",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "weight_multiplier",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "wiphy",
						.field_offset = 0,
					},
				},
			},
			[14] = {
				.struct_size = 0,
				.struct_name = "ieee80211_link_sta",
				.struct_field_num = 5,
				.fields = {
					[0] = {
						.field_name = "addr",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "link_id",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "rx_nss",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "sta",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "supp_rates",
						.field_offset = 0,
					},
				},
			},
			[15] = {
				.struct_size = 0,
				.struct_name = "ieee80211_ops",
				.struct_field_num = 42,
				.fields = {
					[0] = {
						.field_name = "add_chanctx",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "add_interface",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "add_nan_func",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "assign_vif_chanctx",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "bss_info_changed",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "cancel_hw_scan",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "change_chanctx",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "conf_tx",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "config",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "configure_filter",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "del_nan_func",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "flush",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "get_tsf",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "hw_scan",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "link_info_changed",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "offset_tsf",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "prepare_multicast",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "reconfig_complete",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "remain_on_channel",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "remove_chanctx",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "remove_interface",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "reset_tsf",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "set_default_unicast_key",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "set_key",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "set_mac_acl",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "set_rts_threshold",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "set_tsf",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "sta_state",
						.field_offset = 0,
					},
					[28] = {
						.field_name = "start",
						.field_offset = 0,
					},
					[29] = {
						.field_name = "start_ap",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "start_nan",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "stop",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "stop_ap",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "stop_nan",
						.field_offset = 0,
					},
					[34] = {
						.field_name = "suspend",
						.field_offset = 0,
					},
					[35] = {
						.field_name = "sw_scan_start",
						.field_offset = 0,
					},
					[36] = {
						.field_name = "tdls_cancel_channel_switch",
						.field_offset = 0,
					},
					[37] = {
						.field_name = "tdls_channel_switch",
						.field_offset = 0,
					},
					[38] = {
						.field_name = "tdls_recv_channel_switch",
						.field_offset = 0,
					},
					[39] = {
						.field_name = "tx",
						.field_offset = 0,
					},
					[40] = {
						.field_name = "unassign_vif_chanctx",
						.field_offset = 0,
					},
					[41] = {
						.field_name = "vif_cfg_changed",
						.field_offset = 0,
					},
				},
			},
			[16] = {
				.struct_size = 0,
				.struct_name = "ieee80211_scan_request",
				.struct_field_num = 2,
				.fields = {
					[0] = {
						.field_name = "ies",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "req",
						.field_offset = 0,
					},
				},
			},
			[17] = {
				.struct_size = 0,
				.struct_name = "ieee80211_sta",
				.struct_field_num = 5,
				.fields = {
					[0] = {
						.field_name = "addr",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "aid",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "deflink",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "link",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "wme",
						.field_offset = 0,
					},
				},
			},
			[18] = {
				.struct_size = 0,
				.struct_name = "ieee80211_vif",
				.struct_field_num = 12,
				.fields = {
					[0] = {
						.field_name = "active_links",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "addr",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "addr_valid",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "bss_conf",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "cab_queue",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "cfg",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "hw_queue",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "link_conf",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "p2p",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "type",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "valid_links",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "wdev",
						.field_offset = 0,
					},
				},
			},
			[19] = {
				.struct_size = 0,
				.struct_name = "ieee80211_vif_cfg",
				.struct_field_num = 7,
				.fields = {
					[0] = {
						.field_name = "aid",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "ap_addr",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "arp_addr_cnt",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "assoc",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "idle",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "ssid",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "ssid_len",
						.field_offset = 0,
					},
				},
			},
			[20] = {
				.struct_size = 0,
				.struct_name = "inode",
				.struct_field_num = 1,
				.fields = {
					[0] = {
						.field_name = "i_size",
						.field_offset = 0,
					},
				},
			},
			[21] = {
				.struct_size = 0,
				.struct_name = "module",
				.struct_field_num = 22,
				.fields = {
					[0] = {
						.field_name = "core_kallsyms",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "crcs",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "ctors",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "dyndbg_info",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "extable",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "gpl_crcs",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "gpl_syms",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "init",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "kallsyms",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "kp",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "mem",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "name",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "noinstr_text_size",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "noinstr_text_start",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "num_ctors",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "num_exentries",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "num_gpl_syms",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "num_kp",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "num_syms",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "percpu",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "state",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "syms",
						.field_offset = 0,
					},
				},
			},
			[22] = {
				.struct_size = 0,
				.struct_name = "net_device",
				.struct_field_num = 26,
				.fields = {
					[0] = {
						.field_name = "_tx",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "close_list",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "dev_addr",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "dev_addrs",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "dev_refcnt",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "ethtool_ops",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "group",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "gso_max_segs",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "gso_max_size",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "ieee80211_ptr",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "mc",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "name",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "name_assign_type",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "net_notifier_list",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "netdev_ops",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "num_tx_queues",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "priv_flags",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "priv_len",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "ptype_all",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "ptype_specific",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "real_num_tx_queues",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "reg_state",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "state",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "tx_queue_len",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "uc",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "unreg_list",
						.field_offset = 0,
					},
				},
			},
			[23] = {
				.struct_size = 0,
				.struct_name = "net_device_ops",
				.struct_field_num = 7,
				.fields = {
					[0] = {
						.field_name = "ndo_init",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "ndo_open",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "ndo_set_mac_address",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "ndo_set_rx_mode",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "ndo_stop",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "ndo_uninit",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "ndo_validate_addr",
						.field_offset = 0,
					},
				},
			},
			[24] = {
				.struct_size = 0,
				.struct_name = "netdev_queue",
				.struct_field_num = 1,
				.fields = {
					[0] = {
						.field_name = "state",
						.field_offset = 0,
					},
				},
			},
			[25] = {
				.struct_size = 0,
				.struct_name = "pci_bus",
				.struct_field_num = 1,
				.fields = {
					[0] = {
						.field_name = "ops",
						.field_offset = 0,
					},
				},
			},
			[26] = {
				.struct_size = 0,
				.struct_name = "pci_dev",
				.struct_field_num = 21,
				.fields = {
					[0] = {
						.field_name = "bus",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "cfg_size",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "class",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "devfn",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "device",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "driver",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "irq",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "is_busmaster",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "ops",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "pcie_cap",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "pcie_flags_reg",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "pm_cap",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "pme_support",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "rebar_cap",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "resource",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "revision",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "rom_base_reg",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "subsystem_device",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "subsystem_vendor",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "vendor",
						.field_offset = 0,
					},
				},
			},
			[27] = {
				.struct_size = 0,
				.struct_name = "pci_driver",
				.struct_field_num = 3,
				.fields = {
					[0] = {
						.field_name = "id_table",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "probe",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "remove",
						.field_offset = 0,
					},
				},
			},
			[28] = {
				.struct_size = 0,
				.struct_name = "pci_ops",
				.struct_field_num = 3,
				.fields = {
					[0] = {
						.field_name = "map_bus",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "read",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "write",
						.field_offset = 0,
					},
				},
			},
			[29] = {
				.struct_size = 0,
				.struct_name = "rate_info",
				.struct_field_num = 14,
				.fields = {
					[0] = {
						.field_name = "bitrate",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "bw",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "count",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "eht_gi",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "eht_ru_alloc",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "flags",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "he_dcm",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "he_gi",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "he_ru_alloc",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "idx",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "legacy",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "mcs",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "n_bonded_ch",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "nss",
						.field_offset = 0,
					},
				},
			},
			[30] = {
				.struct_size = 0,
				.struct_name = "sk_buff",
				.struct_field_num = 54,
				.fields = {
					[0] = {
						.field_name = "_skb_refdst",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "active_extensions",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "alloc_cpu",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "cb",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "cloned",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "csum",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "csum_offset",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "csum_start",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "data",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "data_len",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "destructor",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "dst",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "dst_pending_confirm",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "end",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "extensions",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "fclone",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "hash",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "hdr_len",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "head",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "head_frag",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "inner_ipproto",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "inner_mac_header",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "inner_network_header",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "inner_protocol",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "inner_protocol_type",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "inner_transport_header",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "ip_summed",
						.field_offset = 0,
					},
					[28] = {
						.field_name = "len",
						.field_offset = 0,
					},
					[29] = {
						.field_name = "list",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "mac_header",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "mac_len",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "network_header",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "next",
						.field_offset = 0,
					},
					[34] = {
						.field_name = "nohdr",
						.field_offset = 0,
					},
					[35] = {
						.field_name = "peeked",
						.field_offset = 0,
					},
					[36] = {
						.field_name = "pfmemalloc",
						.field_offset = 0,
					},
					[37] = {
						.field_name = "pkt_type",
						.field_offset = 0,
					},
					[38] = {
						.field_name = "priority",
						.field_offset = 0,
					},
					[39] = {
						.field_name = "protocol",
						.field_offset = 0,
					},
					[40] = {
						.field_name = "queue_mapping",
						.field_offset = 0,
					},
					[41] = {
						.field_name = "reserved_tailroom",
						.field_offset = 0,
					},
					[42] = {
						.field_name = "sk",
						.field_offset = 0,
					},
					[43] = {
						.field_name = "slow_gro",
						.field_offset = 0,
					},
					[44] = {
						.field_name = "tail",
						.field_offset = 0,
					},
					[45] = {
						.field_name = "transport_header",
						.field_offset = 0,
					},
					[46] = {
						.field_name = "truesize",
						.field_offset = 0,
					},
					[47] = {
						.field_name = "tstamp",
						.field_offset = 0,
					},
					[48] = {
						.field_name = "tstamp_type",
						.field_offset = 0,
					},
					[49] = {
						.field_name = "unreadable",
						.field_offset = 0,
					},
					[50] = {
						.field_name = "users",
						.field_offset = 0,
					},
					[51] = {
						.field_name = "wifi_acked",
						.field_offset = 0,
					},
					[52] = {
						.field_name = "wifi_acked_valid",
						.field_offset = 0,
					},
					[53] = {
						.field_name = "x",
						.field_offset = 0,
					},
				},
			},
			[31] = {
				.struct_size = 0,
				.struct_name = "sk_buff_head",
				.struct_field_num = 3,
				.fields = {
					[0] = {
						.field_name = "next",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "prev",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "qlen",
						.field_offset = 0,
					},
				},
			},
			[32] = {
				.struct_size = 0,
				.struct_name = "skb_shared_info",
				.struct_field_num = 3,
				.fields = {
					[0] = {
						.field_name = "dataref",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "frags",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "nr_frags",
						.field_offset = 0,
					},
				},
			},
			[33] = {
				.struct_size = 0,
				.struct_name = "sock",
				.struct_field_num = 2,
				.fields = {
					[0] = {
						.field_name = "sk_refcnt",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "sk_wmem_alloc",
						.field_offset = 0,
					},
				},
			},
			[34] = {
				.struct_size = 0,
				.struct_name = "urb",
				.struct_field_num = 17,
				.fields = {
					[0] = {
						.field_name = "actual_length",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "anchor",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "anchor_list",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "complete",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "ep",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "hcpriv",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "kref",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "pipe",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "reject",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "status",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "transfer_buffer",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "transfer_buffer_length",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "transfer_dma",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "transfer_flags",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "urb_list",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "use_count",
						.field_offset = 0,
					},
				},
			},
			[35] = {
				.struct_size = 0,
				.struct_name = "usb_device",
				.struct_field_num = 8,
				.fields = {
					[0] = {
						.field_name = "actconfig",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "bus",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "config",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "descriptor",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "ep_in",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "ep_out",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "speed",
						.field_offset = 0,
					},
				},
			},
			[36] = {
				.struct_size = 0,
				.struct_name = "usb_driver",
				.struct_field_num = 4,
				.fields = {
					[0] = {
						.field_name = "disconnect",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "id_table",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "name",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "probe",
						.field_offset = 0,
					},
				},
			},
			[37] = {
				.struct_size = 0,
				.struct_name = "usb_interface",
				.struct_field_num = 5,
				.fields = {
					[0] = {
						.field_name = "altsetting",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "cur_altsetting",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "num_altsetting",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "reset_ws",
						.field_offset = 0,
					},
				},
			},
			[38] = {
				.struct_size = 0,
				.struct_name = "wiphy",
				.struct_field_num = 34,
				.fields = {
					[0] = {
						.field_name = "addresses",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "ap_sme_capa",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "bands",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "bss_param_support",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "bss_priv_size",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "cipher_suites",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "coverage_class",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "ext_features",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "extra_tx_headroom",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "features",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "flags",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "frag_threshold",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "interface_modes",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "max_acl_mac_addrs",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "max_num_csa_counters",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "max_remain_on_channel_duration",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "max_scan_ie_len",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "max_scan_ssids",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "max_sched_scan_plan_interval",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "max_sched_scan_plans",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "mtx",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "n_addresses",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "n_cipher_suites",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "nan_supported_bands",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "pmsr_capa",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "reg_notifier",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "regd",
						.field_offset = 0,
					},
					[28] = {
						.field_name = "regulatory_flags",
						.field_offset = 0,
					},
					[29] = {
						.field_name = "retry_long",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "retry_short",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "rts_threshold",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "signal_type",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "software_iftypes",
						.field_offset = 0,
					},
				},
			},
			[39] = {
				.struct_size = 0,
				.struct_name = "wireless_dev",
				.struct_field_num = 1,
				.fields = {
					[0] = {
						.field_name = "registered",
						.field_offset = 0,
					},
				},
			},
		},
};
