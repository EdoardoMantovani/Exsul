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
	struct runtime_struct_layout structs[7];
};
OFFSET_TABLE_SECTION static struct runtime_struct runtime_generated_structs = {
	.structs = {
			[0] = {
				.struct_size = 0,
				.struct_name =  "sk_buff",
				.struct_field_num = 9,
				.fields = {
					[0] = {
						.field_name = "len",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "data",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "data_len",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "next",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "prev",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "head",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "tail",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "end",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "users",
						.field_offset = 0,
					},
					[10] = {
						.field_name = NULL,
						.field_offset = 0,
					},
				},
        		},
			[1] = {
				.struct_size = 0,
				.struct_name =  "module",
				.struct_field_num = 19,
				.fields = {
					[0] = {
						.field_name = "mem",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "name",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "kp",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "syms",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "crcs",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "gpl_syms",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "gpl_crcs",
						.field_offset = 0,
					},
					[7] = {
						.field_name = "ctors",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "noinstr_text_start",
						.field_offset = 0,
					},
					[9] = {
						.field_name = "noinstr_text_size",
						.field_offset = 0,
					},
					[10] = {
						.field_name = "extable",
						.field_offset = 0,
					},
					[11] = {
						.field_name = "num_exentries",
						.field_offset = 0,
					},
					[12] = {
						.field_name = "dyndbg_info",
						.field_offset = 0,
					},
					[13] = {
						.field_name = "percpu",
						.field_offset = 0,
					},
					[14] = {
						.field_name = "kallsyms",
						.field_offset = 0,
					},
					[15] = {
						.field_name = "core_kallsyms",
						.field_offset = 0,
					},
					[16] = {
						.field_name = "state",
						.field_offset = 0,
					},
					[17] = {
						.field_name = "num_ctors",
						.field_offset = 0,
					},
					[18] = {
						.field_name = "init",
						.field_offset = 0,
					},
					[20] = {
						.field_name = NULL,
						.field_offset = 0,
					},
				},
        		},
			[2] = {
				.struct_size = 0,
				.struct_name =  "wiphy",
				.struct_field_num = 0,
        		},
			[3] = {
				.struct_size = 0,
				.struct_name =  "rate_info",
				.struct_field_num = 0,
        		},
			[4] = {
				.struct_size = 0,
				.struct_name =  "urb",
				.struct_field_num = 3,
				.fields = {
					[0] = {
						.field_name = "kref",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "anchor_list",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "anchor",
						.field_offset = 0,
					},
					[4] = {
						.field_name = NULL,
						.field_offset = 0,
					},
				},
        		},
			[5] = {
				.struct_size = 0,
				.struct_name =  "usb_device",
				.struct_field_num = 1,
				.fields = {
					[0] = {
						.field_name = "actconfig",
						.field_offset = 0,
					},
					[2] = {
						.field_name = NULL,
						.field_offset = 0,
					},
				},
        		},
			[6] = {
				.struct_size = 0,
				.struct_name =  "ieee80211_hw",
				.struct_field_num = 0,
        		},
		},
};
