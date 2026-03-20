struct struct_field {
	unsigned char  field_name[24];
	unsigned short field_offset;
};

struct runtime_struct_layout {
	short struct_size;
	unsigned char *struct_name;
	unsigned short struct_field_num;
	struct struct_field fields[64];
};

struct runtime_struct {
	int compilation_time_generated_structs;
	struct runtime_struct_layout structs[15];
};

OFFSET_TABLE_SECTION static struct runtime_struct runtime_generated_structs = {
	.structs = {
			[0] = {
				.struct_size = 0,
				.struct_name = "device",
				.struct_field_num = 7,
				.fields = {
					[0] = {
						.field_name = "cfg",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "ethtool",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "init_name",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "kobj",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "parent",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "rb_left",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "rb_right",
						.field_offset = 0,
					},
				},
			},
			[1] = {
				.struct_size = 0,
				.struct_name = "ieee80211_hw",
				.struct_field_num = 23,
				.fields = {
					[0] = {
						.field_name = "conf",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "flags",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "max_listen_interval",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "max_mtu",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "max_nan_de_entries",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "max_rates",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "max_report_rates",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "max_rx_aggregation_subframes",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "max_signal",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "max_tx_aggregation_subframes",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "netdev_features",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "offchannel_tx_hw_queue",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "priv",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "queues",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "radiotap_mcs_details",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "radiotap_vht_details",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "rate_control_algorithm",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "tx_sk_pacing_shift",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "txq_data_size",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "uapsd_max_sp_len",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "uapsd_queues",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "weight_multiplier",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "wiphy",
						.field_offset = 0,
					},
				},
			},
			[2] = {
				.struct_size = 0,
				.struct_name = "ieee80211_ops",
				.struct_field_num = 23,
				.fields = {
					[0] = {
						.field_name = "add_nan_func",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "bss_info_changed",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "config",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "del_nan_func",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "hw_scan",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "link_info_changed",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "remain_on_channel",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "set_frag_threshold",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "set_key",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "set_mac_acl",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "set_rts_threshold",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "sta_add",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "sta_remove",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "sta_state",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "start",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "start_nan",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "stop",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "stop_nan",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "tdls_cancel_channel_switch",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "tdls_channel_switch",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "tdls_recv_channel_switch",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "tx",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "vif_cfg_changed",
						.field_offset = 0,
					},
				},
			},
			[3] = {
				.struct_size = 0,
				.struct_name = "module",
				.struct_field_num = 21,
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
						.field_name = "kallsyms",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "kp",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "mem",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "name",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "noinstr_text_size",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "noinstr_text_start",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "num_ctors",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "num_exentries",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "num_gpl_syms",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "num_kp",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "num_syms",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "percpu",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "state",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "syms",
						.field_offset = 0,
					},
				},
			},
			[4] = {
				.struct_size = 0,
				.struct_name = "net_device",
				.struct_field_num = 37,
				.fields = {
					[0] = {
						.field_name = "cfg",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "cfg_pending",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "close_list",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "dev_addr",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "dev_addrs",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "dev_refcnt",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "ethtool",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "features",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "group",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "gso_max_segs",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "gso_max_size",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "hw_features",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "ieee80211_ptr",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "max_mtu",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "mc",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "min_mtu",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "name",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "name_assign_type",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "needed_headroom",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "needed_tailroom",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "net_notifier_list",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "num_napi_configs",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "num_rx_queues",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "num_tx_queues",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "pcpu_stat_type",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "perm_addr",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "priv_flags",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "priv_len",
						.field_offset = 0,
					},
					[28] = {
						.field_name = "ptype_all",
						.field_offset = 0,
					},
					[29] = {
						.field_name = "ptype_specific",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "real_num_rx_queues",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "real_num_tx_queues",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "reg_state",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "state",
						.field_offset = 0,
					},
					[34] = {
						.field_name = "tx_queue_len",
						.field_offset = 0,
					},
					[35] = {
						.field_name = "uc",
						.field_offset = 0,
					},
					[36] = {
						.field_name = "unreg_list",
						.field_offset = 0,
					},
				},
			},
			[5] = {
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
			[6] = {
				.struct_size = 0,
				.struct_name = "sk_buff",
				.struct_field_num = 53,
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
						.field_name = "mac_header",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "mac_len",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "network_header",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "next",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "nohdr",
						.field_offset = 0,
					},
					[34] = {
						.field_name = "peeked",
						.field_offset = 0,
					},
					[35] = {
						.field_name = "pfmemalloc",
						.field_offset = 0,
					},
					[36] = {
						.field_name = "pkt_type",
						.field_offset = 0,
					},
					[37] = {
						.field_name = "prev",
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
						.field_name = "unreadable",
						.field_offset = 0,
					},
					[49] = {
						.field_name = "users",
						.field_offset = 0,
					},
					[50] = {
						.field_name = "wifi_acked",
						.field_offset = 0,
					},
					[51] = {
						.field_name = "wifi_acked_valid",
						.field_offset = 0,
					},
					[52] = {
						.field_name = "x",
						.field_offset = 0,
					},
				},
			},
			[7] = {
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
			[8] = {
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
			[9] = {
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
			[10] = {
				.struct_size = 0,
				.struct_name = "urb",
				.struct_field_num = 11,
				.fields = {
					[0] = {
						.field_name = "anchor",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "anchor_list",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "ep",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "kref",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "reject",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "transfer_buffer",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "transfer_dma",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "transfer_flags",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "urb_list",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "use_count",
						.field_offset = 0,
					},
				},
			},
			[11] = {
				.struct_size = 0,
				.struct_name = "usb_device",
				.struct_field_num = 3,
				.fields = {
					[0] = {
						.field_name = "actconfig",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "speed",
						.field_offset = 0,
					},
				},
			},
			[12] = {
				.struct_size = 0,
				.struct_name = "usb_driver",
				.struct_field_num = 2,
				.fields = {
					[0] = {
						.field_name = "name",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "probe",
						.field_offset = 0,
					},
				},
			},
			[13] = {
				.struct_size = 0,
				.struct_name = "usb_interface",
				.struct_field_num = 4,
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
				},
			},
			[14] = {
				.struct_size = 0,
				.struct_name = "wiphy",
				.struct_field_num = 42,
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
						.field_name = "ema_max_profile_periodicity",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "ext_features",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "extra_tx_headroom",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "features",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "flags",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "frag_threshold",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "iface_combinations",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "interface_modes",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "max_acl_mac_addrs",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "max_num_csa_counters",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "max_remain_on_channel_duration",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "max_scan_ie_len",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "max_scan_ssids",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "max_sched_scan_plan_interval",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "max_sched_scan_plans",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "mbssid_max_interfaces",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "mtx",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "n_addresses",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "n_cipher_suites",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "n_iface_combinations",
						.field_offset = 0,
					},
					[28] = {
						.field_name = "n_radio",
						.field_offset = 0,
					},
					[29] = {
						.field_name = "nan_supported_bands",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "pmsr_capa",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "priv",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "radio",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "regd",
						.field_offset = 0,
					},
					[34] = {
						.field_name = "regulatory_flags",
						.field_offset = 0,
					},
					[35] = {
						.field_name = "retry_long",
						.field_offset = 0,
					},
					[36] = {
						.field_name = "retry_short",
						.field_offset = 0,
					},
					[37] = {
						.field_name = "rts_threshold",
						.field_offset = 0,
					},
					[38] = {
						.field_name = "signal_type",
						.field_offset = 0,
					},
					[39] = {
						.field_name = "software_iftypes",
						.field_offset = 0,
					},
					[40] = {
						.field_name = "support_mbssid",
						.field_offset = 0,
					},
					[41] = {
						.field_name = "support_only_he_mbssid",
						.field_offset = 0,
					},
				},
			},
		},
};
