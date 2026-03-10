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
struct runtime_struct_layout structs[11];
};
OFFSET_TABLE_SECTION static struct runtime_struct runtime_generated_structs = {
	.structs = {
			[0] = {
				.struct_size = 0,
				.struct_name = "device",
				.struct_field_num = 1,
				.fields = {
					[0] = {
						.field_name = "kobj",
						.field_offset = 0,
					},
				},
			},
			[1] = {
				.struct_size = 0,
				.struct_name = "ieee80211_hw",
				.struct_field_num = 22,
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
						.field_name = "assign_vif_chanctx",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "bss_info_changed",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "change_chanctx",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "config",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "configure_filter",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "hw_scan",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "link_info_changed",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "remain_on_channel",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "remove_chanctx",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "remove_interface",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "set_key",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "sta_add",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "sta_remove",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "sta_state",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "start",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "stop",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "tx",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "unassign_vif_chanctx",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "vif_cfg_changed",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "wake_tx_queue",
						.field_offset = 0,
					},
				},
			},
			[2] = {
				.struct_size = 0,
				.struct_name = "module",
				.struct_field_num = 34,
				.fields = {
					[0] = {
						.field_name = "core_kallsyms",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "core_typeoffs",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "crcs",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "ctors",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "dyndbg_info",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "extable",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "gpl_crcs",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "gpl_syms",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "hdr",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "index",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "init",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "init_typeoffs",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "kallsyms",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "kp",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "len",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "mem",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "mod",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "mod_kallsyms_init_off",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "name",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "noinstr_text_size",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "noinstr_text_start",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "num_ctors",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "num_exentries",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "num_gpl_syms",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "num_kp",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "num_syms",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "percpu",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "sechdrs",
						.field_offset = 0,
					},
					[28] = {
						.field_name = "secstrings",
						.field_offset = 0,
					},
					[29] = {
						.field_name = "state",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "stroffs",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "strtab",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "symoffs",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "syms",
						.field_offset = 0,
					},
				},
			},
			[3] = {
				.struct_size = 0,
				.struct_name = "rate_info",
				.struct_field_num = 7,
				.fields = {
					[0] = {
						.field_name = "bw",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "eht_gi",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "flags",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "he_gi",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "legacy",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "mcs",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "nss",
						.field_offset = 0,
					},
				},
			},
			[4] = {
				.struct_size = 0,
				.struct_name = "sk_buff",
				.struct_field_num = 48,
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
						.field_name = "end",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "extensions",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "fclone",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "hash",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "hdr_len",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "head",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "head_frag",
						.field_offset = 0,
					},
					[19] = {
						.field_name = "inner_ipproto",
						.field_offset = 0,
					},
					[20] = {
						.field_name = "inner_mac_header",
						.field_offset = 0,
					},
					[21] = {
						.field_name = "inner_network_header",
						.field_offset = 0,
					},
					[22] = {
						.field_name = "inner_protocol",
						.field_offset = 0,
					},
					[23] = {
						.field_name = "inner_protocol_type",
						.field_offset = 0,
					},
					[24] = {
						.field_name = "inner_transport_header",
						.field_offset = 0,
					},
					[25] = {
						.field_name = "ip_summed",
						.field_offset = 0,
					},
					[26] = {
						.field_name = "len",
						.field_offset = 0,
					},
					[27] = {
						.field_name = "mac_header",
						.field_offset = 0,
					},
					[28] = {
						.field_name = "mac_len",
						.field_offset = 0,
					},
					[29] = {
						.field_name = "network_header",
						.field_offset = 0,
					},
					[30] = {
						.field_name = "next",
						.field_offset = 0,
					},
					[31] = {
						.field_name = "nohdr",
						.field_offset = 0,
					},
					[32] = {
						.field_name = "peeked",
						.field_offset = 0,
					},
					[33] = {
						.field_name = "pfmemalloc",
						.field_offset = 0,
					},
					[34] = {
						.field_name = "pkt_type",
						.field_offset = 0,
					},
					[35] = {
						.field_name = "prev",
						.field_offset = 0,
					},
					[36] = {
						.field_name = "protocol",
						.field_offset = 0,
					},
					[37] = {
						.field_name = "queue_mapping",
						.field_offset = 0,
					},
					[38] = {
						.field_name = "reserved_tailroom",
						.field_offset = 0,
					},
					[39] = {
						.field_name = "sk",
						.field_offset = 0,
					},
					[40] = {
						.field_name = "slow_gro",
						.field_offset = 0,
					},
					[41] = {
						.field_name = "tail",
						.field_offset = 0,
					},
					[42] = {
						.field_name = "transport_header",
						.field_offset = 0,
					},
					[43] = {
						.field_name = "truesize",
						.field_offset = 0,
					},
					[44] = {
						.field_name = "tstamp",
						.field_offset = 0,
					},
					[45] = {
						.field_name = "unreadable",
						.field_offset = 0,
					},
					[46] = {
						.field_name = "users",
						.field_offset = 0,
					},
					[47] = {
						.field_name = "x",
						.field_offset = 0,
					},
				},
			},
			[5] = {
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
			[6] = {
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
			[7] = {
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
			[8] = {
				.struct_size = 0,
				.struct_name = "urb",
				.struct_field_num = 3,
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
						.field_name = "kref",
						.field_offset = 0,
					},
				},
			},
			[9] = {
				.struct_size = 0,
				.struct_name = "usb_device",
				.struct_field_num = 2,
				.fields = {
					[0] = {
						.field_name = "actconfig",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "dev",
						.field_offset = 0,
					},
				},
			},
			[10] = {
				.struct_size = 0,
				.struct_name = "wiphy",
				.struct_field_num = 10,
				.fields = {
					[0] = {
						.field_name = "bss_param_support",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "bss_priv_size",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "ext_features",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "features",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "flags",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "priv",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "regd",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "regulatory_flags",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "retry_long",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "retry_short",
						.field_offset = 0,
					},
				},
			},
		},
};
