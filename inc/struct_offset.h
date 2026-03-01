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
	struct runtime_struct_layout structs[8];
};
OFFSET_TABLE_SECTION static struct runtime_struct runtime_generated_structs = {
	.structs = {
			[0] = {
				.struct_size = 0,
				.struct_name =  "usb_device",
				.struct_field_num = 0,
        		},
			[1] = {
				.struct_size = 0,
				.struct_name =  "ieee80211_hw",
				.struct_field_num = 0,
        		},
			[2] = {
				.struct_size = 0,
				.struct_name =  "sk_buff",
				.struct_field_num = 7,
				.fields = {
					[0] = {
						.field_name = "next",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "len",
						.field_offset = 0,
					},
					[2] = {
						.field_name = "head",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "data",
						.field_offset = 0,
					},
					[4] = {
						.field_name = "tail",
						.field_offset = 0,
					},
					[5] = {
						.field_name = "end",
						.field_offset = 0,
					},
					[6] = {
						.field_name = "dev",
						.field_offset = 0,
					},
					[8] = {
						.field_name = "NULL",
						.field_offset = 0,
					},
				},
        		},
			[3] = {
				.struct_size = 0,
				.struct_name =  "sk_buff",
				.struct_field_num = 2,
				.fields = {
					[0] = {
						.field_name = "next",
						.field_offset = 0,
					},
					[1] = {
						.field_name = "prev",
						.field_offset = 0,
					},
					[3] = {
						.field_name = "NULL",
						.field_offset = 0,
					},
				},
        		},
			[4] = {
				.struct_size = 0,
				.struct_name =  "module",
				.struct_field_num = 0,
        		},
			[5] = {
				.struct_size = 0,
				.struct_name =  "urb",
				.struct_field_num = 0,
        		},
			[6] = {
				.struct_size = 0,
				.struct_name =  "rate_info",
				.struct_field_num = 0,
        		},
			[7] = {
				.struct_size = 0,
				.struct_name =  "wiphy",
				.struct_field_num = 0,
        		},
		},
};
