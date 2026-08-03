#!/usr/bin/perl

# Made by Edoardo Mantovani, 2026
# Exsul build script
# version 2.1.0 (3 Aug 2026)

use strict;
use warnings;
use Getopt::Long;
use Cwd qw(getcwd);
use File::Copy qw(move);
use List::Util qw(uniq);
use File::Path qw(make_path);
use Term::ANSIColor qw(color);
use File::Temp qw(tempfile tempdir);
use File::Basename qw(dirname basename);

my $ls      = `which ls`;
my $cd      = `which cd`;
my $cp      = `which cp`;
my $rm      = `which rm`;
my $cat	    = `which cat`;
my $sed     = `which sed`;
my $awk     = `which awk`;
my $grep    = `which grep`;
my $find    = `which find`;
my $make    = `which make`;
my $xxd     = `which xxd`;
my $echo    = `which echo`;
my $sort    = `which sort`;
my $mkdir   = `which mkdir`;
my $readelf = `which readelf`;

my $arch    = "arch";
my $source  = "src";
my $include = "inc";
my $stubs   = "stubs";
my $build   = "build";
my $bin     = "bin";

my $final_program = "exsul";

my $main      = "main.c";
my $win_layer = "layer.h";
my $proto_gen = "prototypes.h";
my $cb_table  = "cb_table.h";

my $functions = "functions";
my $variables = "variables";

my $function_proto_regex = q{^\h*[_A-Za-z]\w*(?:[\w\h\*]*\h+)+[_A-Za-z]\w*\h*\([^;{}]*\)\h*(?=\{)};
my $find_command_arg = q{};
my $cb_table_parse = q{'{print $6}'};
my $readelf_table_parse = q{'{print $3}'};
my $sym_name_parse = q{'{print $8}'};

my @built_objects = ();
my $merged_source = undef;
my $proto = undef;
my $current_stub = undef;
my $final_stub = undef;
my $stub_name = undef;
my $cb_header = undef;
my $fw_header = undef;
my $offsets = undef;
my $cb_counter = 0;
my $proto_header = undef;
my $iterator = undef;
my $sym_name = undef;
my $sym_size = undef;
my $word_processor = undef;
my $base_folder = undef;
my $offset_generation_file = undef;
my $virtual_compilation_folder = undef;

my $compiler = undef;
my $enable_firmware_builtin = undef;
my $enable_fast_compilation = undef;
my $enable_debugging = undef;
my $generate_offset_file = undef;
my $pahole_kernel = undef;
my $windows_support = undef;
my $windows_static = undef;
my $verbose = undef;
my $dbg = undef;
my $strip = undef;
my $enable_exported_symbols_tracing = undef;
my $clean = undef;
my $help = undef;
my @windows_runtime_dlls = ();

my $exsul_sz = 0;
my $modified = 0;
my $exsul_f_sz = 0;
my $process_step = 0;
my $struct_field_counter = 0;
my $struct_name_counter = 0;
my $total_struct_number = 0;
my $current_struct_field_num = 0;
# permits to define the maximun number of the symbols that can be supported by the kernel module loader, unsure if maintain or not.
my $sym_total_entries = 0;

my @sources = ();
my @crafted_sources = ();
my @crafted_source_lines = ();
my @struct_fields = ();
my @runtime_structs = ();
my %struct_idx_by_name = ();
my %field_idx_by_name = ();
my %struct_set = ();
my %used_fields_by_struct = ();

my @gcc_params = (
	"-DARCH_INDEPENDENT_BITOPS",
	"-DTCC_SCRIPTING",
	"-DCONFIG_PM",
	"-DENABLE_UNWIND_DEBUGGING",
	"-O2",
	"-w",
	"-g",
	"-funwind-tables",
	"-fasynchronous-unwind-tables",
	"-fno-omit-frame-pointer",
	"-march=native",
	"-mtune=native",
	"-mstackrealign",
	"-mno-avx512f",
);
my @gcc_stub_params = (
	"-O2",
	"-fno-builtin",
	"-mno-sse",
	"-mno-mmx",
	"-mno-sse2",
	"-mno-3dnow",
	"-mno-avx",
	"-mno-sse4a",
	"-mno-red-zone",
	"-fno-omit-frame-pointer",
);
my @gcc_final_params = (
	"-fno-pie",
	"-no-pie",
);
my @gcc_final_linkings = (
	"-lusb-1.0",
	"-lpthread",
	"-ldwarf",
	"-ltcc",
	"-lunwind",
	"-lunwind-x86_64",
	"-lcrypto"
);

my %struct_whitelist = (
	"file"			=> "file",
	"skb"			=> "sk_buff",
	"skb_info"		=> "skb_shared_info",
	"mod"			=> "module",
	"wiphy"			=> "wiphy",
	"sock"			=> "sock",
	"dev"			=> "device",
	"udev"			=> "usb_device",
        "rate_info"     	=> "rate_info",
	"hw"			=> "ieee80211_hw",
	"urb"			=> "urb",
	"fops"			=> "file_operations",
	"net_device"		=> "net_device",
	"intf"			=> "usb_interface",
	"ieee80211_ops" 	=> "ieee80211_ops",
	"driver"		=> "device_driver",
	"usb_driver"		=> "usb_driver",
	"hdev"			=> "hci_dev",
	"hci_conn"		=> "hci_conn",
	"cfg80211_scan"		=> "cfg80211_scan_request",
	"settings"		=> "cfg80211_ap_settings",
	"cfg80211_beacon"	=> "cfg80211_beacon_data",
	"link_conf"		=> "ieee80211_bss_conf",
	"ieee80211_scan"	=> "ieee80211_scan_request",
	"ieee80211_vif" 	=> "ieee80211_vif",
	"ieee80211_vif_cfg"	=> "ieee80211_vif_cfg",
	"connection_vif"	=> "ieee80211_vif",
	"connection_sta"	=> "ieee80211_sta",
	"connection_link_sta"	=> "ieee80211_link_sta",
	"ieee80211_ops"		=> "ieee80211_ops",
	"cfg80211_ops"		=> "cfg80211_ops",
	"net_device_ops"	=> "net_device_ops",
);
my %fixed_only_structs = (
	"ieee80211_sta"      => 1,
	"ieee80211_link_sta" => 1,
);
my %field_ctype = (
	file => {
		"f_flags"				=> "unsigned int",
	},
	module => {
		"mem"					=> "struct module_memory",
		"crcs"					=> "u32 *",
		"gpl_crcs"				=> "u32 *",
		"num_gpl_syms"				=> "unsigned int",
		"num_syms"				=> "unsigned int",
		"num_exentries"				=> "unsigned int",
		"args"					=> "char *",
		"ctors"					=> "void *",
		"exit"					=> "void *",
		"name"					=> "char",
		"build_id"				=> "char",
		"sig_ok"				=> "bool",
		"status"				=> "unsigned int",
		"aborted"				=> "bool",
		"kp"					=> "struct kernel_param *",
		"num_kp"				=> "unsigned int",
		"syms"					=> "struct kernel_symbol *",
		"gpl_syms"				=> "struct kernel_symbol *",
		"extable"				=> "struct exception_table_entry *",
		"noinstr_text_start"			=> "void *",
		"noinstr_text_size"			=> "unsigned int",
		"dyndbg_info"				=> "void *",
		"percpu"				=> "void *",
		"init"					=> "void *",
		"state"					=> "unsigned int",
		"kallsyms"				=> "struct mod_kallsyms *",
		"core_kallsyms"				=> "struct mod_kallsyms",
	},
	sk_buff => {
		"len"                   		=> "unsigned int",
		"data_len"              		=> "unsigned int",
		"mac_len"               		=> "u16",
		"hdr_len"               		=> "u16",
		"next"                 	 		=> "struct sk_buff *",
		"prev"                  		=> "struct sk_buff *",
		"tail"                  		=> "sk_buff_data_t",
		"end"                   		=> "sk_buff_data_t",
		"head"                  		=> "unsigned char *",
		"data"                  		=> "unsigned char *",
		"cb"					=> "char",
		"queue_mapping"				=> "u16",
		"pkt_type"				=> "u8",
		"unreadable"            		=> "u8",
		"cloned"				=> "u8",
		"csum"					=> "__wsum",
		"hash"					=> "u32",
		"protocol"				=> "be16",
		"inner_protocol"        		=> "be16",
		"inner_protocol_type"   		=> "u8",
		"inner_ipproto"         		=> "u8",
		"inner_mac_header"      		=> "u16",
		"inner_network_header"  		=> "u16",
		"inner_transport_header"		=> "u16",
		"transport_header"      		=> "u16",
		"network_header"        		=> "u16",
		"mac_header"            		=> "u16",
		"dev"                   		=> "void *",
		"queue_mapping"         		=> "unsigned int",
		"reserved_tailroom"     		=> "unsigned int",
		"destructor"            		=> "void *",
		"users"					=> "refcount_t",
	},
	sk_buff_head => {
		"next"					=> "struct sk_buff *",
		"prev"					=> "struct sk_buff *",
		"qlen"					=> "unsigned int",
	},
	skb_shared_info => {
		"flags"					=> "unsigned char",
		"frag_list"				=> "struct sk_buff *",
		"destructor_arg"			=> "void *",
	},
	urb => {
		"status"				=> "int",
		"interval"				=> "int",
		"use_count"				=> "atomic_int",
		"reject"				=> "atomic_int",
		"kref"					=> "struct kref",
		"urb_list"				=> "struct list_head",
		"anchor"				=> "struct usb_anchor *",
		"anchor_list"				=> "struct list_head",
		"dev"					=> "struct usb_device *",
		"ep"					=> "struct usb_host_endpoint *",
		"pipe"					=> "unsigned int",
		"transfer_flags"			=> "unsigned int",
		"transfer_buffer"			=> "void *",
		"anchor"				=> "struct usb_anchor *",
		"context"				=> "void *",
		"setup_packet"				=> "unsigned char *",
		"setup_dma"				=> "u64",
		"iso_frame_desc"			=> "struct usb_iso_packet_descriptor",
		"complete"				=> "void *", # should be a callback but use a void pointer for now
	},
	usb_device => {
		"ep0"					=> "struct usb_host_endpoint",
		"descriptor"				=> "struct usb_device_descriptor",
		"bos"					=> "struct usb_host_bos *",
		"config"				=> "struct usb_host_config *",
		"actconfig"				=> "struct usb_host_config *",
		"dev"					=> "void *",
		"kobj"					=> "void *",
		"ep_in"					=> "struct usb_host_endpoint *",
		"ep_out"				=> "struct usb_host_endpoint *",
		"tt"					=> "struct usb_tt *", # probably will be a "never used" struct
	},
	rate_info => {
		"bw"					=> "unsigned int",
		"nss"					=> "unsigned int",
		"mcs"					=> "unsigned int",
		"flags"					=> "unsigned int",
		"eht_gi"				=> "unsigned int",
		"he_gi"					=> "unsigned int",
		"legacy"				=> "unsigned int",
	},
	wiphy => {
		"dev"					=> "struct device",
		"flags"					=> "u32",
		"regulatory_flags"			=> "u32",
		"features"				=> "u32",
		"software_iftypes"			=> "u16",
		"n_addresses"				=> "u16",
		"interface_modes"			=> "u16",
		"max_acl_mac_addrs"			=> "u16",
		"ext_features"				=> "u8",
		"priv"					=> "void *",
		"max_num_csa_counters"			=> "u8",
		"max_ap_assoc_sta"			=> "u16",
		"bands"					=> "struct ieee80211_supported_band *",
		"max_num_pmkids"			=> "u8",
		"pmsr_capa"				=> "struct cfg80211_pmsr_capabilities *",
		"iftype_akm_suites"			=> "struct wiphy_iftype_akm_suites *",
		"retry_short"				=> "u8",
		"retry_long"				=> "u8",
		"frag_threshold"			=> "u32",
		"rts_threshold"				=> "u32",
		"coverage_class"			=> "u8",
		"max_scan_ssids"			=> "u8",
		"max_sched_scan_reqs"			=> "u8",
		"max_sched_scan_ssids"			=> "u8",
		"max_match_sets"			=> "u8",
		"max_scan_ie_len"			=> "u16",
		"max_sched_scan_ie_len"			=> "u16",
		"max_sched_scan_plans"			=> "u32",
		"max_sched_scan_plan_interval"		=> "u32",
		"max_sched_scan_plan_iterations"	=> "u32",
		"max_remain_on_channel_duration"	=> "u16",
		"available_antennas_tx"			=> "u32",
		"available_antennas_rx"			=> "u32",
		"probe_resp_offload"			=> "u32",
		"registered"				=> "bool",
	},
	net_device => {
		"name"					=> "char",
		"mem_end"				=> "unsigned long",
		"mem_start"				=> "unsigned long",
		"base_addr"				=> "unsigned long",
		"irq"					=> "int",
		"dev_list"				=> "struct list_head",
		"dev_addrs"				=> "struct netdev_hw_addr_list",
		"napi_list"				=> "struct list_head",
		"unreg_list"				=> "struct list_head",
		"ptype_all"				=> "struct list_head",
		"ptype_specific"			=> "struct list_head",
		"features"				=> "u64",
		"rx_handler"				=> "void *",
		"rx_handler_data"			=> "void *",
		"state"					=> "unsigned long",
		"real_num_tx_queues"			=> "unsigned int",
		"gso_max_size"				=> "unsigned int",
		"gso_ipv4_max_size"			=> "unsigned int",
		"gso_max_segs"				=> "u16",
		"num_tc"				=> "s16",
	},
	usb_interface => {
		"altsetting"				=> "struct usb_host_interface *",
		"cur_altsetting"			=> "struct usb_host_interface *",
		"intf_assoc"				=> "struct usb_interface_assoc_descriptor *",
		"dev"					=> "struct device",
		"usb_dev"				=> "struct device *",
		"reset_ws"				=> "struct work_struct",
	},
	ieee80211_hw => {
		"conf"					=> "struct ieee80211_conf",
		"wiphy"					=> "struct wiphy *",
		"rate_control_algorithm"		=> "const char *",
		"priv"					=> "void *",
		"flags"					=> "unsigned long",
		"vif_data_size"				=> "int",
		"sta_data_size"				=> "int",
		"chanctx_data_size"			=> "int",
		"txq_data_size"				=> "int",
		"extra_tx_headroom"			=> "unsigned int",
		"extra_beacon_tailroom" 		=> "unsigned int",
		"queues"				=> "u16",
		"max_listen_interval"			=> "u16",
		"max_signal"				=> "s8",
		"max_rates"				=> "u8",
		"max_report_rates"			=> "u8",
		"max_rate_tries"			=> "u8",
		"max_tx_fragments"			=> "u8",
	},
	device => {
		"kobj"					=> "struct kobject",
		"parent"				=> "struct device *",
		"init_name"				=> "const char *",
		"devres_head"				=> "struct list_head",
		"driver"				=> "struct device_driver *",
		"driver_data"				=> "void *",
		"platform_data"				=> "void *",
	},
	ieee80211_ops => {
		"add_chanctx"				=> "void *",
		"add_interface"				=> "void *",
		"add_nan_func"				=> "void *",
		"assign_vif_chanctx"			=> "void *",
		"bss_info_changed"			=> "void *",
		"change_chanctx"			=> "void *",
		"config"				=> "void *",
		"configure_filter"			=> "void *",
		"del_nan_func"				=> "void *",
		"hw_scan"				=> "void *",
		"link_info_changed"			=> "void *",
		"remain_on_channel"			=> "void *",
		"remove_chanctx"			=> "void *",
		"remove_interface"			=> "void *",
		"set_frag_threshold"			=> "void *",
		"set_key"				=> "void *",
		"set_mac_acl"				=> "void *",
		"set_rts_threshold"			=> "void *",
		"sta_add"				=> "void *",
		"sta_remove"				=> "void *",
		"sta_state"				=> "void *",
		"start"					=> "void *",
		"start_nan"				=> "void *",
		"stop"					=> "void *",
		"stop_nan"				=> "void *",
		"tdls_cancel_channel_switch" 		=> "void *",
		"tdls_channel_switch"			=> "void *",
		"tdls_recv_channel_switch" 		=> "void *",
		"tx"					=> "void *",
		"unassign_vif_chanctx"			=> "void *",
		"vif_cfg_changed"			=> "void *",
		"wake_tx_queue"				=> "void *",
	},
	cfg80211_ops => {
		"suspend"				=> "void *",
		"resume"				=> "void *",
		"set_wakeup"				=> "void *",
		"add_virtual_intf"			=> "void *",
		"del_virtual_intf"			=> "void *",
		"change_virtual_intf"			=> "void *",
		"start_ap"				=> "void *",
		"stop_ap"				=> "void *",
		"join_mesh"				=> "void *",
		"leave_mesh"				=> "void *",
		"set_monitor_channel"			=> "void *",
		"scan"					=> "void *",
		"abort_scan"				=> "void *",
		"auth"					=> "void *",
		"deauth"				=> "void *",
		"assoc"					=> "void *",
		"set_tx_power"				=> "void *",
		"get_tx_power"				=> "void *",
		"set_antenna"				=> "void *",
		"get_antenna"				=> "void *",
		"start_radar_detection"			=> "void *",
	},
	net_device_ops => {
		"ndo_init"				=> "void *",
		"ndo_uninit"				=> "void *",
		"ndo_open"				=> "void *",
		"ndo_stop"				=> "void *",
		"ndo_start_xmit"			=> "void *",
		"ndo_features_check"			=> "void *",
		"ndo_set_rx_mode"			=> "void *",
		"ndo_set_mac_address"			=> "void *",
		"ndo_validate_addr"			=> "void *",
		"ndo_do_ioctl"				=> "void *",
		"ndo_eth_ioctl"				=> "void *",
	},
	device_driver => {
		"name"					=> "const char *",
		"bus"					=> "struct bus_type *",
		"owner"					=> "struct module *",
		"mod_name"				=> "const char *",
		"suppress_bind_attrs"			=> "bool",
		"probe_type"				=> "enum probe_type",
		"of_match_table"			=> "struct of_device_id *",
		"acpi_match_table"			=> "struct acpi_device_id *",
		"probe"					=> "void *",
		"sync_state"				=> "void *",
		"remove"				=> "void *",
		"shutdown"				=> "void *",
		"suspend"				=> "void *",
		"resume"				=> "void *",
		"p"					=> "struct driver_private *",
	},
	usb_driver => {
		"name"					=> "const char *",
		"probe"					=> "void *",
		"disconnect"				=> "void *",
		"unlocked_ioctl"			=> "void *",
		"suspend"				=> "void *",
		"resume"				=> "void *",
		"reset_resume"				=> "void *",
		"pre_reset"				=> "void *",
		"post_reset"				=> "void *",
		"id_table"				=> "struct usb_device_id *",
		"drvwrap"				=> "struct usbdrv_wrap",
	},
	hci_dev => {
		"list"					=> "struct list_head",
		"name"					=> "const char *",
		"flags"					=> "unsigned long",
		"id"					=> "__u16",
		"bus"					=> "__u8",
		"bdaddr"				=> "bdaddr_t",
		"setup_addr"				=> "bdaddr_t",
		"public_addr"				=> "bdaddr_t",
		"random_addr"				=> "bdaddr_t",
		"static_addr"				=> "bdaddr_t",
		"adv_addr_type"				=> "__u8",
		"appearance"				=> "__u16",
		"dev_class"				=> "__u8",
		"major_class"				=> "__u8",
		"minor_class"				=> "__u8",
		"max_page"				=> "__u8",
		"le_features"				=> "__u8",
		"le_accept_list_size"			=> "__u8",
		"le_resolv_list_size"			=> "__u8",
		"le_num_of_adv_sets"			=> "__u8",
		"le_states"				=> "__u8",
		"mesh_ad_types"				=> "__u8",
		"mesh_send_ref"				=> "__u8",
		"commands"				=> "__u8",
		"hci_ver"				=> "__u8",
		"hci_rev"				=> "__u16",
		"lmp_ver"				=> "__u8",
		"manufacturer"				=> "__u16",
		"lmp_subver"				=> "__u16",
		"voice_setting"				=> "__u16",
		"num_iac"				=> "__u8",
		"stored_max_keys"			=> "__u16",
		"stored_num_keys"			=> "__u16",
		"io_capability"				=> "__u8",
		"inq_tx_power"				=> "__s8",
		"err_data_reporting"			=> "__u8",
		"page_scan_interval"			=> "__u16",
		"page_scan_window"			=> "__u16",
		"page_scan_type"			=> "__u8",
		"le_adv_channel_map"			=> "__u8",
		"le_adv_min_interval"			=> "__u16",
		"le_adv_max_interval"			=> "__u16",
		"le_scan_type"				=> "__u8",
		"le_scan_interval"			=> "__u16",
		"le_scan_window"			=> "__u16",
		"le_scan_int_suspend"			=> "__u16",
		"sniff_min_interval"			=> "__u16",
		"sniff_max_interval"			=> "__u16",
		"cmd_cnt"				=> "atomic_int",
		"le_tx_def_phys"			=> "__u8",
		"le_rx_def_phys"			=> "__u8",
		"cmd_sync_work_list"			=> "struct list_head",
		"rx_q"					=> "struct sk_buff_head",
		"raw_q"					=> "struct sk_buff_head",
		"cmd_q"					=> "struct sk_buff_head",
		"mesh_pending"				=> "struct list_head",
		"mgmt_pending_lock"			=> "struct mutex",
		"mgmt_pending"				=> "struct list_head",
		"reject_list"				=> "struct list_head",
		"accept_list"				=> "struct list_head",
		"uuids"					=> "struct list_head",
		"link_keys"				=> "struct list_head",
		"long_term_keys"			=> "struct list_head",
		"identity_resolving_keys"		=> "struct list_head",
		"remote_oob_data"			=> "struct list_head",
		"le_accept_list"			=> "struct list_head",
		"le_resolv_list"			=> "struct list_head",
		"le_conn_params"			=> "struct list_head",
		"pend_le_conns"				=> "struct list_head",
		"pend_le_reports"			=> "struct list_head",
		"blocked_keys"				=> "struct list_head",
		"local_codecs"				=> "struct list_head",
		"hw_info"				=> "const char *",
		"fw_info"				=> "const char *",
		"adv_instances"				=> "struct list_head",
		"monitored_devices"			=> "struct list_head",
		"open"					=> "void *",
		"close"					=> "void *",
		"flush"					=> "void *",
		"setup"					=> "void *",
		"shutdown"				=> "void *",
		"send"					=> "void *",
		"notify"				=> "void *",
		"hw_error"				=> "void *",
		"post_init"				=> "void *",
		"set_diag"				=> "void *",
		"set_bdaddr"				=> "void *",
		"reset"					=> "void *",
		"wakeup"				=> "void *",
		"set_quality_report"			=> "void *",
		"get_data_path_id"			=> "void *",
		"get_codec_config_data"			=> "void *",
		"classify_pkt_type"			=> "void *",
	},
	hci_conn => {
		"list"					=> "struct list_head",
	},
	cfg80211_scan_request => {
		"ssids"					=> "struct cfg80211_ssid *",
		"n_ssids"				=> "int",
		"n_channels"				=> "u32",
		"scan_width"				=> "int", # translate 'enum nl80211_bss_scan_width' to 'int'
		"ie"					=> "u8 *",
		"ie_len"				=> "size_t",
		"duration"				=> "u16",
		"duration_mandatory"			=> "bool",
		"flags"					=> "u32",
		"rates"					=> "u32",
		"wdev"					=> "struct wireless_dev *",
		"mac_addr"				=> "u8",
		"mac_addr_mask"				=> "u8",
		"bssid"					=> "u8",
		"wiphy"					=> "struct wiphy *",
		"scan_start"				=> "unsigned long",
		"info"					=> "struct cfg80211_scan_info",
		"notified"				=> "bool",
		"no_cck"				=> "bool",
		"scan_6ghz"				=> "bool",
		"n_6ghz_params"				=> "u32",
		"scan_6ghz_params"			=> "struct cfg80211_scan_6ghz_params *",
		"channels"				=> "struct ieee80211_channel *",
	},
	cfg80211_ap_settings => {
		"chandef"				=> "struct cfg80211_chan_def",
		"beacon"				=> "struct cfg80211_beacon_data",
		"beacon_interval"			=> "int",
		"dtim_period"				=> "int",
		"ssid"					=> "const u8 *",
		"ssid_len"				=> "int",
		"hidden_ssid"				=> "enum nl80211_hidden_ssid",
		"crypto"				=> "struct cfg80211_crypto_settings",
		"privacy"				=> "bool",
		"auth_type"				=> "enum nl80211_auth_type",
		"inactivity_timeout"			=> "int",
		"p2p_ctwindow"				=> "u8",
		"p2p_opp_ps"				=> "bool",
		"acl"					=> "struct cfg80211_acl_data *",
		"pbss"					=> "bool",
		"beacon_rate"				=> "struct cfg80211_bitrate_mask",
		"ht_cap"				=> "const struct ieee80211_ht_cap *",
		"vht_cap"				=> "const struct ieee80211_vht_cap *",
		"he_cap"				=> "const struct ieee80211_he_cap_elem *",
		"he_oper"				=> "const struct ieee80211_he_operation *",
		"eht_cap"				=> "const struct ieee80211_eht_cap_elem *",
		"eht_oper"				=> "const struct ieee80211_eht_operation *",
		"uhr_oper"				=> "const struct ieee80211_uhr_operation *",
		"ht_required"				=> "bool",
		"vht_required"				=> "bool",
		"he_required"				=> "bool", 
		"sae_h2e_required"			=> "bool",
		"twt_responder"				=> "bool",
		"flags"					=> "u32",
		"he_obss_pd"				=> "struct ieee80211_he_obss_pd",
		"fils_discovery"			=> "struct cfg80211_fils_discovery",
		"unsol_bcast_probe_resp"		=> "struct cfg80211_unsol_bcast_probe_resp",
		"mbssid_config"				=> "struct cfg80211_mbssid_config",
		"s1g_long_beacon_period"		=> "u8",
		"s1g_short_beacon"			=> "struct cfg80211_s1g_short_beacon",
	},
	cfg80211_beacon_data => {
		"link_id"				=> "int",
		"head"					=> "u8 *",
		"tail"					=> "u8 *",
		"beacon_ies"				=> "u8 *",
		"proberesp_ies"				=> "u8 *",
		"assocresp_ies"				=> "u8 *",
		"probe_resp"				=> "u8 *",
		"lci"					=> "u8 *",
		"civicloc"				=> "u8 *",
		"mbssid_ies"				=> "struct cfg80211_mbssid_elems *",
		"rnr_ies"				=> "struct cfg80211_rnr_elems *",
		"ftm_responder"				=> "s8",
		"head_len"				=> "size_t",
		"tail_len"				=> "size_t",
		"beacon_ies_len"			=> "size_t",
		"proberesp_ies_len"			=> "size_t",
		"assocresp_ies_len"			=> "size_t",
		"probe_resp_len"			=> "size_t",
		"lci_len"				=> "size_t",
		"civicloc_len"				=> "size_t",
		"he_bss_color_valid"			=> "bool",
	},
	ieee80211_bss_conf => {
		"vif"					=> "struct ieee80211_vif *",
		"bss"					=> "struct cfg80211_bss *",
		"bssid"					=> "u8 *",
		"link_id"				=> "unsigned int",
		"addr"					=> "u8",
		"htc_trig_based_pkt_ext"		=> "u8",
		"uora_exists"				=> "bool",
		"uora_ocw_range"			=> "u8",
		"frame_time_rts_th"			=> "u16",
		"he_support"				=> "bool",
		"twt_requester"				=> "bool",
		"twt_responder"				=> "bool",
		"twt_protected"				=> "bool",
		"twt_broadcast"				=> "bool",
		"use_cts_prot"				=> "bool",
		"use_short_preamble"			=> "bool",
		"use_short_slot"			=> "bool",
		"enable_beacon"				=> "bool",
		"dtim_period"				=> "u8",
		"beacon_int"				=> "u16",
		"assoc_capability"			=> "u16",
		"sync_tsf"				=> "u64",
		"sync_device_ts"			=> "u32",
		"sync_dtim_count"			=> "u8",
		"basic_rates"				=> "u32",
		"beacon_rate"				=> "struct ieee80211_rate *",
		"mcast_rate"				=> "int",
		"ht_operation_mode"			=> "u16",
		"cqm_rssi_thold"			=> "s32",
		"cqm_rssi_hyst"				=> "u32",
		"cqm_rssi_low"				=> "s32",
		"cqm_rssi_high"				=> "s32",
		"chanreq"				=> "struct ieee80211_chan_req",
		"mu_group"				=> "struct ieee80211_mu_group_data",
		"qos"					=> "bool",
		"hidden_ssid"				=> "bool",
		"txpower"				=> "int",
		"txpower_type"				=> "enum nl80211_tx_power_setting",
		"p2p_noa_attr"				=> "struct ieee80211_p2p_noa_attr",
		"allow_p2p_go_ps"			=> "bool",
		"max_idle_period"			=> "u16",
		"protected_keep_alive"			=> "bool",
		"ftm_responder"				=> "bool",
		"ftmr_params"				=> "struct ieee80211_ftm_responder_params *",
		"nontransmitted"			=> "bool",
		"tx_bss_conf"				=> "struct ieee80211_bss_conf *",
		"transmitter_bssid"			=> "u8",
		"bssid_index"				=> "u8",
		"bssid_indicator"			=> "u8",
		"ema_ap"				=> "bool",
		"profile_periodicity"			=> "u8",
		"he_obss_pd"				=> "struct ieee80211_he_obss_pd",
		"he_bss_color"				=> "struct cfg80211_he_bss_color",
		"fils_discovery"			=>"struct ieee80211_fils_discovery",
		"unsol_bcast_probe_resp_interval" 	=> "u32",
		"beacon_tx_rate"			=> "struct cfg80211_bitrate_mask",
		"power_type"				=> "enum ieee80211_ap_reg_power",
		"tpe"					=> "struct ieee80211_parsed_tpe",
		"pwr_reduction"				=> "u8",
		"eht_support"				=> "bool",
		"epcs_support"				=> "bool",
		"uhr_support"				=> "bool",
		"csa_active"				=> "bool",
		"mu_mimo_owner"				=> "bool",
		"chanctx_conf"				=> "struct ieee80211_chanctx_conf *",
		"color_change_active"			=> "bool",
		"color_change_color"			=> "u8",
		"ht_ldpc"				=> "bool",
		"vht_ldpc"				=> "bool",
		"he_ldpc"				=> "bool",
		"vht_su_beamformer"			=> "bool",
		"vht_su_beamformee"			=> "bool",
		"vht_mu_beamformer"			=> "bool",
		"vht_mu_beamformee"			=> "bool",
		"he_su_beamformer"			=> "bool",
		"he_su_beamformee"			=> "bool",
		"he_mu_beamformer"			=> "bool",
		"he_full_ul_mumimo"			=> "bool",
		"eht_su_beamformer"			=> "bool",
		"eht_su_beamformee"			=> "bool",
		"eht_mu_beamformer"			=> "bool",
		"eht_80mhz_full_bw_ul_mumimo"		=> "bool",
		"eht_disable_mcs15"			=> "bool",
		"bss_param_ch_cnt"			=> "u8",
		"bss_param_ch_cnt_link_id"		=> "u8",
		"s1g_long_beacon_period"		=> "u8",
	},
	ieee80211_scan_request => {
		"ies"					=> "struct ieee80211_scan_ies",
		"req"					=> "struct cfg80211_scan_request",
	},
	ieee80211_vif => {
		"type"					=> "enum nl80211_iftype",
		"cfg"					=> "struct ieee80211_vif_cfg",
		"bss_conf"				=> "struct ieee80211_bss_conf",
		"link_conf"				=> "struct ieee80211_bss_conf *",
		"reg_ttlm"				=> "struct ieee80211_neg_ttlm",
		"addr"					=> "u8",
		"addr_valid"				=> "bool",
		"p2p"					=> "bool",
		"cab_queue"				=> "u8",
		"hw_queue"				=> "u8",
		"driver_flags"				=> "u32",
		"offload_flags"				=> "u32",
		"valid_links"				=> "u16",
		"active_links"				=> "u16",
		"dormant_links"				=> "u16",
		"suspended_link"			=> "u16",
		"probe_req_reg"				=> "bool",
		"rx_mcast_action_reg"			=> "bool",
	},
	ieee80211_vif_cfg => {
		"assoc"					=> "bool",
		"aid"					=> "u16",
		"ibss_joined"				=> "bool",
		"ibss_creator"				=> "bool",
		"ssid"					=> "u8",
		"ssid_len"				=> "int",
		"idle"					=> "bool",
		"ap_addr"				=> "u8",
	},
	ieee80211_sta => {
		"addr"					=> "u8",
		"aid"					=> "u16",
		"wme"					=> "bool",
		"valid_links"				=> "u16",
		"deflink"				=> "struct ieee80211_link_sta",
		"link"					=> "struct ieee80211_link_sta *",
	},
	ieee80211_link_sta => {
		"sta"					=> "struct ieee80211_sta *",
		"addr"					=> "u8",
		"link_id"				=> "u8",
		"supp_rates"				=> "u32",
		"rx_nss"				=> "u8",
	},
);
my %inline_array_field = (
	module => {
		"mem"				=> 1,
		"name"				=> 1,
		"build_id"			=> 1,
	},
	wiphy => {
		"bands"				=> 1,
	},
	cfg80211_scan_request => {
		"rates"				=> 1,
		"mac_addr"			=> 1,
		"mac_addr_mask"			=> 1,
		"bssid"				=> 1,
		"channels"			=> 1,
	},
	skb_shared_info => {
		"frags"				=> 1,
	},
	sk_buff => {
		"cb"				=> 1,
	},
	ieee80211_sub_if_data => {
		"vif"				=> 1,
	},
	ieee80211_vif => {
		"addr"				=> 1,
		"hw_queue"			=> 1,
		"link_conf"			=> 1,
	},
	ieee80211_bss_conf => {
		"addr"				=> 1,
		"transmitter_bssid"		=> 1,
		"mcast_rate"			=> 1,
		
	},
	ieee80211_vif_cfg => {
		"ssid"				=> 1,
		"ap_addr"			=> 1,
	},
	ieee80211_sta => {
		"addr"				=> 1,
		"link"				=> 1,
	},
	ieee80211_link_sta => {
		"addr"				=> 1,
		"supp_rates"			=> 1,
	},
);

chomp($ls);
chomp($cd);
chomp($cp);
chomp($rm);
chomp($cat);
chomp($sed);
chomp($awk);
chomp($grep);
chomp($find);
chomp($make);
chomp($xxd);
chomp($echo);
chomp($sort);
chomp($mkdir);
chomp($readelf);

GetOptions(
	"compiler=s"      => \$compiler,
	"enable-fw"	  => \$enable_firmware_builtin,
	"fast"		  => \$enable_fast_compilation,
	"debug"		  => \$enable_debugging,
	"generate_offset" => \$generate_offset_file,
	"pahole_kernel=s" => \$pahole_kernel,
	"windows"         => \$windows_static,
	"verbose"	  => \$verbose,
	"dbg"		  => \$dbg,
	"strip"		  => \$strip,
	"trace"		  => \$enable_exported_symbols_tracing,
	"clean"           => \$clean,
	"help"            => \$help,
);

if($help){
	print("Build script for Exsul\n");
	print("Made by Edoardo Mantovani, 2026\n");
	print("-> options:\n");
	print("--compiler=[gcc|clang]\n");
	print("--enable-fw -> enable static firmware embedding in the main application, this permits to not use the Linux standard path for loading a firmware\n");
	print("--generate_offset -> generate a special json file which can be used for creating the offset backup plan\n");
	print("--windows -> enable windows compilation and require static Windows libraries for a single executable\n");
	print("--verbose -> show compilation command line\n");
	print("--dbg -> enable debugging strings in Exsul\n");
	print("--strip -> strip the final executable\n");
	print("--trace -> trace every exported function, used mainly for debugging!\n");
	print("--clean -> clear every compilation file\n");
	print("--help -> display this help text\n");
	exit(0);
}

if($windows_static){
	$windows_support = 1;
}

# used only if you want to generate the fallback offset solution.
if($generate_offset_file){
	$offset_generation_file = "required_offsets.json";
}

# enable the debugging strings in Exsul
if($dbg){
	push(@gcc_params, "-DENABLE_DBG_PRINT ");
}

# select the default before applying compiler-specific flags. Kernel modules
# may call userspace stubs with an 8-byte-aligned stack, so GCC must know the
# incoming stack boundary and realign it before calling libc.
if(!$compiler && !$windows_support){
	$compiler = "gcc";
}

# clang does not support GCC's incoming stack boundary option. GCC stubs must
# accept the kernel's 8-byte incoming alignment, while retaining the default
# 16-byte preferred alignment required when they call userspace libraries.
if($compiler){
	if(!($compiler eq "clang")){
		push(@gcc_params, "-mincoming-stack-boundary=3 ");
	}else{
		push(@gcc_params, "-Wno-everything ");
		push(@gcc_stub_params, "-Wno-implicit-function-declaration ");
	}
	# workaround for tcc, which cannot find the stdatomic.h header.
	if($compiler eq "tcc"){
		push(@gcc_params, "-I/usr/include/newlib/ ");
		# fix also 'Unable to determine type definition of intptr_t'
		push(@gcc_params, "-D__INTPTR_TYPE__ long int ");
	}
	# windows support
	if($compiler =~ "ming"){
		push(@gcc_params, "-I/usr/include/ ");
		push(@gcc_params, "-I/usr/include/x86_64-linux-gnu/ ");
	}
}

if($enable_debugging){

}

# gcc ming for Windows doesn't support -rdynamic, this is just a workaround for not producing any errors during the compilation.
if(!($windows_support) && $compiler && !($compiler =~ "ming")){
	push(@gcc_params, "-rdynamic");
}

# "--clean" code
if($clean){
	`$rm $build/*.o 2>/dev/null`;
	`$rm $bin/$final_program* 2>/dev/null`;
	`$rm -rf out_* 2>/dev/null`;
	# update: remove also crashdump files
	`$rm -f crashreport*`;
	`$rm -f sigabort_crash*`;
	`$rm -f offsets.conf`;
	`$rm *.elf 2>/dev/null`;

	if($offset_generation_file){
		`$rm $offset_generation_file 2>/dev/null`;
	}else{
		`$rm required_offsets.json 2>/dev/null`;
	}
	exit(0);
}

# same as using --compiler=mingw
if($windows_support){
	$compiler = "x86_64-w64-mingw32-gcc";
	$final_program = "exsul.exe";
	push(@gcc_params, "-I./win/include/ ");
	# set also a variant of readelf
	$readelf = "x86_64-w64-mingw32-nm";
	$readelf_table_parse = q{'{print $2}'};
	@gcc_final_linkings = ();
	push(@gcc_final_linkings, "-static");
	push(@gcc_final_linkings, "-static-libgcc");
	push(@gcc_final_linkings, "-static-libstdc++");

	if($windows_static){
		my @required_static_libs = (
			"win/libs/libdwarf.a",
			"win/libs/libusb-1.0.a",
			"win/libs/libtcc.a",
			"win/libs/libzstd.a",
		);
		my @missing_static_libs = grep { ! -e $_ } @required_static_libs;
		push(@gcc_final_linkings, @required_static_libs);
		push(@gcc_final_linkings, "-lbcrypt");
		push(@gcc_final_linkings, "-lwinpthread");
		push(@gcc_final_linkings, "-lsynchronization");
		push(@gcc_final_linkings, "-lsetupapi");
		push(@gcc_final_linkings, "-lole32");
		push(@gcc_final_linkings, "-luuid");
		push(@gcc_final_linkings, "-ladvapi32");
		push(@gcc_final_linkings, "-lws2_32");
	}else{
		push(@gcc_final_linkings, "-lbcrypt");
		push(@gcc_final_linkings, "-lwinpthread");
		push(@gcc_final_linkings, "-lsynchronization");
		# now add the Windows import/static libraries used by the DLL-backed build
		push(@gcc_final_linkings, "win/libs/libdwarf.dll.a");
		push(@gcc_final_linkings, "win/libs/libdwarfp.dll.a");
		push(@gcc_final_linkings, "win/libs/libusb-1.0.a");
		push(@gcc_final_linkings, "win/libs/libusb-1.0.dll.a");
		push(@gcc_final_linkings, "win/libs/libtcc.dll");
		push(@gcc_final_linkings, "win/libs/libzstd.a");
		push(@windows_runtime_dlls, "win/libs/libdwarf-2.dll");
		push(@windows_runtime_dlls, "win/libs/libtcc.dll");
	}
}

if($enable_fast_compilation){
	goto FASTJUMP;
}

# utilities function for getting struct/fields index
sub get_struct_index {
	my ($struct_name) = @_;
	return $struct_idx_by_name{$struct_name};
}

sub get_field_index {
	my ($struct_name, $field_name) = @_;
	return undef if !exists $field_idx_by_name{$struct_name};
	return $field_idx_by_name{$struct_name}{$field_name};
}

# new version of 'get_extended_ptr_variables' had a bug which didn't permit to detect "ieee80211_ops"
sub get_extended_ptr_variables {
	my ($line, $struct) = @_;
	my %seen = ();

	while ($line =~ /\b(?:const\s+)?struct\s+\Q$struct\E\b\s*\*+\s*([A-Za-z_]\w*)\b(?=\s*(?:[,;)=]))/g) {
		$seen{$1} = 1;
	}

	while ($line =~ /\b(?:const\s+)?struct\s+\Q$struct\E\b\s*\*+\s*[A-Za-z_]\w*\b((?:\s*,\s*\*+\s*[A-Za-z_]\w*\b)*)/g) {
		my $tail = $1;
		while ($tail =~ /,\s*\*+\s*([A-Za-z_]\w*)\b/g) {
			$seen{$1} = 1;
		}
	}

	return keys %seen;
}

# this is used for getting the type string associated with every field, the main advantage of that instead of assigning to every field the "unsigned int" type,
# we can cast them automatically, the form became: (*(type *)((char *)base + director.runtime_generated_structs->structs[X].fields[Y].field_offset))
sub get_type_from_struct_field {
	my ($struct, $field) = @_;
	if($verbose){
		if(exists $field_ctype{$struct} && exists $field_ctype{$struct}{$field}){
			print("- struct $struct field $field\n");
		}else{
			print("- not exist $struct field $field\n");
		}
	}
	# fast fix: in case of 'ieee80211_ops' always return a "void *" [deprecated]
	if($struct eq "ieee80211_ops"){
		if($verbose){
			print("[ieee80211_ops] fix for field $field\n");
		}
		return "void *";
	}

	return $field_ctype{$struct}{$field} if exists $field_ctype{$struct} && exists $field_ctype{$struct}{$field};
	return "unsigned int";
}

# detect if the field is an array, this is used for things like "module->mem[type]" situations.
sub field_is_inline_array {
	my ($struct, $field) = @_;
	return 0 if !exists $inline_array_field{$struct};
	return exists $inline_array_field{$struct}{$field};
}

# build the final expression "(*(type *)((char *)base + director.runtime_generated_structs->structs[$current_struct_idx].fields[$current_field_idx].field_offset))"
sub build_simple_field_access_expr {
	my ($var, $st, $field) = @_;

	my $current_struct_idx = get_struct_index($st);
	my $current_field_idx  = get_field_index($st, $field);

	if (!defined $current_struct_idx || !defined $current_field_idx) {
		print("[!] skipping $var->$field (struct $st)\n");
		return undef;
	}

	my $current_field_type = get_type_from_struct_field($st, $field);
	my $field_offset_expr  = "director.runtime_generated_structs->structs[$current_struct_idx].fields[$current_field_idx].field_offset";

	if($verbose){
		print("$st->$field has type $current_field_type\n");
	}

	return "(*($current_field_type*)((char *)$var + $field_offset_expr))";
}

# build the expression (*(type *)((char *)base + runtime_struct.specific_field))) [only idea I have for now on this]
sub build_field_access_expr {
	my ($var, $st, $field, $index_expr) = @_;

	my $current_struct_idx = get_struct_index($st);
	my $current_field_idx  = get_field_index($st, $field);

	if (!defined $current_struct_idx || !defined $current_field_idx) {
		print("[!] skipping unresolved access $var->$field (struct $st)\n");
		return undef;
	}

	my $current_field_type = get_type_from_struct_field($st, $field);
	my $field_offset_expr  = "director.runtime_generated_structs->structs[$current_struct_idx].fields[$current_field_idx].field_offset";

	if($verbose){
		print("$st->$field has type $current_field_type\n");
	}

	if (field_is_inline_array($st, $field)) {
		my $base_expr = "(($current_field_type *)((char *)$var + $field_offset_expr))";
		if (defined $index_expr && length($index_expr) > 0) {
			return "$base_expr\[$index_expr\]";
		}
		return $base_expr;
	}

	my $base_expr = "(*($current_field_type*)((char *)$var + $field_offset_expr))";
	if (defined $index_expr && length($index_expr) > 0) {
		return "$base_expr\[$index_expr\]";
	}
	return $base_expr;
}

# build a very similar output as above BUT use this for the sizeof(struct <not compiled struct>)
sub build_undef_struct_expr {
	my $st = @_;
	my $current_struct_idx = 0;
	my $base_expr = undef;

	$current_struct_idx = get_struct_index($st);
	$base_expr = "director.runtime_generated_structs->structs[$current_struct_idx].struct_size";
	return $base_expr;
}

sub build_fixed_alias_map {
	my %fixed_alias_to_struct = ();
	while (my ($var, $st) = each %struct_whitelist) {
		$fixed_alias_to_struct{$var} = $st;
	}
	return %fixed_alias_to_struct;
}

sub harvest_declared_ptr_aliases {
	my ($line) = @_;
	my %found = ();

	foreach my $struct (keys %struct_set) {
		next if $fixed_only_structs{$struct};
		my @vars = get_extended_ptr_variables($line, $struct);
		foreach my $v (@vars) {
			$found{$v} = $struct;
		}
	}

	return %found;
}

# scrape from the source codes every access to an undefined struct which will be converted later in this code.
sub collect_used_fields {
	my (@srcs) = @_;
	my %fixed_alias = build_fixed_alias_map();
	my %used = ();

	foreach my $src (@srcs) {
		open(my $fh, '<', $src) or die $!;
		my %local_alias = %fixed_alias;
		my %alias_depth = ();
		my $brace_depth = 0;

		while (my $line = <$fh>) {
			my %new_aliases = harvest_declared_ptr_aliases($line);
			if (%new_aliases) {
				for my $k (keys %new_aliases) {
					$local_alias{$k} = $new_aliases{$k};
					$alias_depth{$k} = $brace_depth;
				}
			}

			while ($line =~ /\b([A-Za-z_]\w*)->([A-Za-z_]\w*)/g) {
				my ($var, $field) = ($1, $2);
				my $st = $local_alias{$var};
				next if !defined $st;
				if($verbose){
					print("[*] $src:$st->$field\n");
				}
				$used{$st}{$field} = 1;
			}

			my $opens  = () = $line =~ /\{/g;
			my $closes = () = $line =~ /\}/g;
			$brace_depth += $opens - $closes;
			if ($closes > 0) {
				for my $k (keys %alias_depth) {
					if ($alias_depth{$k} > $brace_depth) {
						delete $local_alias{$k};
						delete $alias_depth{$k};
					}
				}
			}
		}

		close($fh) or die $!;
	}

	return %used;
}

# generate the struct_offset header, it takes the structs defined in the hash above and searches through ./inc, ./src and ./stubs every possible field access, then generates
# a big global struct which contains every offset encoded as a struct field which will be accessed at runtime.
sub emit_runtime_struct_header {
	my ($offsets_path, $offset_json_path) = @_;

	open(OFFSETS, '>', $offsets_path) or die $!;
	print OFFSETS "struct struct_field {\n";
	print OFFSETS "	unsigned char  field_name[24];\n";
	print OFFSETS "	unsigned short field_offset;\n";
	print OFFSETS "};\n\n";
	print OFFSETS "struct runtime_struct_layout {\n";
	print OFFSETS "	short struct_size;\n";
	print OFFSETS "	unsigned char *struct_name;\n";
	print OFFSETS "	unsigned short struct_field_num;\n";
	print OFFSETS "	struct struct_field fields[128];\n";
	print OFFSETS "};\n\n";
	print OFFSETS "struct runtime_struct {\n";
	print OFFSETS "	int compilation_time_generated_structs;\n";
	print OFFSETS "	struct runtime_struct_layout structs[$total_struct_number + 1];\n";
	print OFFSETS "};\n\n";
	print OFFSETS "OFFSET_TABLE_SECTION static struct runtime_struct runtime_generated_structs = {\n";
	print OFFSETS "	.structs = {\n";

	if($generate_offset_file){
		open(OFFSET_GEN, '>', $offset_json_path) or die $!;
		print OFFSET_GEN "{\n";
		print OFFSET_GEN "\"structs\": [\n";
	}

	my @tracked_structs = sort keys %used_fields_by_struct;
	$struct_name_counter = 0;
	%struct_idx_by_name = ();
	%field_idx_by_name = ();
	@runtime_structs = ();

	for (my $i = 0; $i < scalar(@tracked_structs); $i++) {
		my $struct_name = $tracked_structs[$i];
		my @fields = sort keys %{ $used_fields_by_struct{$struct_name} };
		$current_struct_field_num = scalar(@fields);
		$struct_idx_by_name{$struct_name} = $struct_name_counter;

		my $current_struct = {
			name    => $struct_name,
			fields  => [],
			by_name => {},
		};

		print OFFSETS "\t\t\t[$struct_name_counter] = {\n";
		print OFFSETS "\t\t\t\t.struct_size = 0,\n";
		print OFFSETS "\t\t\t\t.struct_name = \"$struct_name\",\n";
		print OFFSETS "\t\t\t\t.struct_field_num = $current_struct_field_num,\n";

		if (@fields) {
			print OFFSETS "\t\t\t\t.fields = {\n";
			$struct_field_counter = 0;

			foreach my $field (@fields) {
				$field_idx_by_name{$struct_name}{$field} = $struct_field_counter;
				$current_struct->{by_name}{$field} = $struct_field_counter;
				push(@{ $current_struct->{fields} }, { offset => 0, field_name => $field });

				print OFFSETS "\t\t\t\t\t[$struct_field_counter] = {\n";
				print OFFSETS "\t\t\t\t\t\t.field_name = \"$field\",\n";
				print OFFSETS "\t\t\t\t\t\t.field_offset = 0,\n";
				print OFFSETS "\t\t\t\t\t},\n";
				$struct_field_counter++;
			}

			print OFFSETS "\t\t\t\t},\n";
		}

		print OFFSETS "\t\t\t},\n";
		push(@runtime_structs, $current_struct);

		if($generate_offset_file){
			print OFFSET_GEN "\t\t{\n";
			print OFFSET_GEN "\t\t\t\"name\":\"$struct_name\",\n";
			print OFFSET_GEN "\t\t\t\"fields\": [";
			for (my $j = 0; $j < scalar(@fields); $j++) {
				print OFFSET_GEN "\"$fields[$j]\"";
				print OFFSET_GEN ", " if $j < $#fields;
			}
			print OFFSET_GEN "]\n";
			print OFFSET_GEN "\t\t}";
			print OFFSET_GEN "," if $i < $#tracked_structs;
			print OFFSET_GEN "\n";
		}

		$struct_name_counter++;
	}

	print OFFSETS "\t\t},\n";
	print OFFSETS "};\n";
	close(OFFSETS) or die $!;

	if($generate_offset_file){
		print OFFSET_GEN "\t]\n";
		print OFFSET_GEN "}\n";
		close(OFFSET_GEN) or die $!;
	}
}

# main function used for rewriting the file with the struct conversion
sub rewrite_source_file {
	my ($in_path) = @_;
	my %fixed_alias = build_fixed_alias_map();

	open(my $in, '<', $in_path) or die $!;

	my $dir = dirname($in_path);
	my ($out, $tmp_path) = tempfile(".tmp_XXXX", DIR => $dir, UNLINK => 0);
	my %local_alias = %fixed_alias;
	my %alias_depth = ();
	my $brace_depth = 0;
	# fix: allow to use the array form for the regex in every file under './out_XXXX'
	my $allow_array_mode = ($in_path);

	while(my $line = <$in>){
		my %new_aliases = harvest_declared_ptr_aliases($line);
		if (%new_aliases) {
			for my $k (keys %new_aliases) {
				$local_alias{$k} = $new_aliases{$k};
				$alias_depth{$k} = $brace_depth;
			}
		}

		if (%local_alias) {
			# resolve the sizeof(struct <not defined>) by converting it
                        foreach(keys %local_alias){
                                if($line =~ "sizeof(struct $_)"){
                                        print("$line\n");
                                }
                        }
			# maps the keys of %local alias as 'key1|key2|key3'..
        		my $var_re = join '|', map { quotemeta($_) } sort { length($b) <=> length($a) } keys %local_alias;
			if ($allow_array_mode) {
				$line =~ s{
					\b($var_re)->([A-Za-z_]\w*)
					(
						\s*\[
							([^\[\]\n]+)
						\]
					)?
				}{
					my ($var, $field, $raw_index, $index_expr) = ($1, $2, $3, $4);
					my $st = $local_alias{$var};

					if($verbose){
						print("var $var field $field st $st\n");
					}

					if (!(defined $st)) {
						defined $raw_index ? "$var->$field$raw_index" : "$var->$field";
					}else{
						my $expr = build_field_access_expr($var, $st, $field, $index_expr);
						if (defined $expr) {
							$expr;
						}else{
							defined $raw_index ? "$var->$field$raw_index" : "$var->$field";
						}
					}
				}gex;
			}else{
				$line =~ s{\b($var_re)->([A-Za-z_]\w*)(?!\s*\[)}{
					my ($var, $field) = ($1, $2);
					my $st = $local_alias{$var};

					if($verbose){
						print("var $var field $field st $st\n");
					}

					if (!(defined $st)) {
						"$var->$field";
					}else{
						my $expr = build_simple_field_access_expr($var, $st, $field);
						if (defined $expr) {
							$expr;
						}else{
							"$var->$field";
						}
					}
				}gex;
			}
		}

		print $out $line;

		my $opens  = () = $line =~ /\{/g;
		my $closes = () = $line =~ /\}/g;
		$brace_depth += $opens - $closes;
		if ($closes > 0) {
			for my $k (keys %alias_depth) {
				if ($alias_depth{$k} > $brace_depth) {
					delete $local_alias{$k};
					delete $alias_depth{$k};
				}
			}
		}
	}

	close($out) or die $!;
	close($in)  or die $!;
	move($tmp_path, $in_path) or die $!;
}

# I noticed that instead of compiling every file used as kernel->userspace wrappers, is way more practical to merge the 450+ files into a big
# one and compile it alone
sub merge_rewritten_function_stubs {
	my ($root, $out_path) = @_;

	my $functions_dir = "./$stubs/$functions";
	my $variables_dir = "./$stubs/$variables";
	my $layer_path    = "$root/$include/$win_layer";
	my $utils_path    = "$root/$include/utils.h";

	my $out_dir = dirname($out_path);
	make_path($out_dir) unless -d $out_dir;

	open(my $out, '>', $out_path) or die $!;

	print $out "/* Copyright (C) Edoardo Mantovani - All Rights Reserved\n";
	print $out "* Unauthorized copying of this file, via any medium is strictly prohibited\n";
	print $out "* Proprietary and confidential\n";
	print $out "*/\n\n";

	foreach my $header ($layer_path, $utils_path) {
		open(my $fh, '<', $header) or die $!;
		while (my $line = <$fh>) {
			print $out $line;
		}
		print $out "\n\n";
		close($fh) or die $!;
	}

	my @function_files = ();
	my @win_excluded_stubs = qw(snprintf.c sscanf.c vsnprintf.c sprintf.c);
	foreach (`$find $functions_dir -type f -name "*.c" | $sort`) {
		my $file = $_;
		chomp($file);
		if($windows_support){
			my $bn = basename($file);
			next if grep { $bn eq $_ } @win_excluded_stubs;
		}
		push(@function_files, $file);
	}

	foreach(`$find $variables_dir -type f -name "*.c" | $sort`) {
		my $file = $_;
		chomp($file);
		push(@function_files, $file);
	}

	foreach my $file (@function_files) {
		print $out "\n/* from $file */\n";
		open(my $fh, '<', $file) or die $!;
		while (my $line = <$fh>) {
			print $out $line;
		}
		print $out "\n";
		close($fh) or die $!;
	}

	close($out) or die $!;
}

# print logo
print(color("green"));
print("███████╗██╗  ██╗███████╗██╗   ██╗██╗     
██╔════╝╚██╗██╔╝██╔════╝██║   ██║██║     
█████╗   ╚███╔╝ ███████╗██║   ██║██║     
██╔══╝   ██╔██╗ ╚════██║██║   ██║██║     
███████╗██╔╝ ██╗███████║╚██████╔╝███████╗
╚══════╝╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝\n");
print(color("reset"));

# create the build directory
`$mkdir build 2>/dev/null`;
`$mkdir bin 2>/dev/null`;

# generate the inat-table.c file at build time
`$awk -f tools/generate_inat/gen-insn-attr-x86.awk tools/generate_inat/x86-opcode-map.txt > $arch/x86_64/inat-tables.c`;
print("[*] generated $arch/x86_64/inat-tables.c...\n");

# fix: generate early prototypes.h for avoiding any error related to the compilation of 'merged_functions.h'
$proto_header = "$include/$proto_gen";
{
	open(my $early_fh, '>', $proto_header) or die $!;
	print $early_fh "/* Copyright (C) Edoardo Mantovani - All Rights Reserved\n";
	print $early_fh "* Unauthorized copying of this file, via any medium is strictly prohibited\n";
	print $early_fh "* Proprietary and confidential\n";
	print $early_fh "*/\n\n";
	print $early_fh "/* automatically generated file, don't edit! */\n\n";
	foreach(`$find $stubs/$functions/ -name "*.c" | $sort`){
		my $cs = $_; chomp($cs);
		my $p = `$grep -oP '$function_proto_regex' $cs`;
		chomp($p);
		if(length($p) == 0){
			$p = `$cat $cs | head -n 1`;
			chomp($p);
			$p =~ s/{//;
		}
		print $early_fh "$p;\n" if length($p);
	}
	foreach(`$find $stubs/$variables/ -name "*.c" | $sort`){
		my $cs = $_; chomp($cs);
		my $p = `$cat $cs | head -n 1`;
		chomp($p);
		$p =~ s/=//;
		$p =~ s/{//;
		print $early_fh "extern $p;\n" if length($p);
	}
	close($early_fh) or die $!;
}

# generate the tempdir used for storing the files with the converted struct fields access
$virtual_compilation_folder = tempdir("out_XXXX", CLEANUP => 0);
make_path("$virtual_compilation_folder/$build");

foreach(qw(inc src arch)){
	print("[*] transferring $_ to the runtime struct converter...\n");
	`$cp -R $_ $virtual_compilation_folder/$_`;
}

$merged_source = "$virtual_compilation_folder/$build/merged_functions.c";
merge_rewritten_function_stubs($virtual_compilation_folder, $merged_source);

foreach(`$find $virtual_compilation_folder -type f`){
	$iterator = $_;
	chomp($iterator);
	# dirty workaround due to mingw headers because "snprintf" is already defined..
	if($windows_support){
		if(!($iterator eq "snprintf")){
			push(@sources, $iterator);
		}
	}else{
		push(@sources, $iterator);
	}
}

%struct_set = map { $_ => 1 } values %struct_whitelist;
$struct_set{"sk_buff_head"}    = 1;

print("[*] starting to modify the struct fields!\n");
# first pass: gather field usage using local alias inference
%used_fields_by_struct = collect_used_fields(@sources);
$total_struct_number = scalar(keys %used_fields_by_struct);

$offsets = "$include/struct_offset.h";
print("[*] generating the struct_offset.h header!\n");
emit_runtime_struct_header($offsets, $offset_generation_file);
`$cp $offsets $virtual_compilation_folder/$include/`;

# second iteration:
# 1) process header files under "./inc/"
# 2) process stubs
# 3) process "src/main.c"
@sources = ();
foreach(`$find $virtual_compilation_folder`){
	$iterator = $_;
	chomp($iterator);
	if($iterator =~ /\.(c|h)\z/){
		if($verbose){
			print("[*] adding $iterator to the compilation list!\n");
		}
		# dirty workaround due to mingw headers because "snprintf" is already defined..
		if($windows_support){
			if(!($iterator eq "snprintf")){
				push(@sources, $iterator);
			}
		}else{
			push(@sources, $iterator);
		}
	}
}

# apply the real modification now, foreach file added, convert its structs field access
foreach my $src (uniq(@sources)){
	print("[*] converting $src...\n");
	rewrite_source_file($src);
}

print("[*] conversion finished!\n");

$final_stub = "merged_functions.o";
if($verbose){
	print("$compiler @gcc_params -I./$virtual_compilation_folder/$include @gcc_stub_params -include $virtual_compilation_folder/$include/$win_layer -c $merged_source -o $build/$final_stub\n");
}

if($enable_exported_symbols_tracing){
	my $coccinelle = `which spatch`;
	my $coccinelle_file = undef;
	my $coccinelle_file_name = "Exsul_export_tracing.cocci";

	if(!$coccinelle){
		print("[!] coccinelle not installed!\n");
	}else{
		chomp($coccinelle);
		# create the coccinelle patching file
		open($coccinelle_file, '>', $coccinelle_file_name);
		print $coccinelle_file "@@\n";
		print $coccinelle_file "identifier fn !~ \"^(memcpy|memset)\$\";\n";
		print $coccinelle_file "@@\n\n";
		print $coccinelle_file "fn(...) {\n";
		print $coccinelle_file "+ printf(\"-> %s\", __FUNCTION__);\n";
		print $coccinelle_file "...\n";
		print $coccinelle_file "}\n";
		close($coccinelle_file);
		`$coccinelle --sp-file $coccinelle_file_name --in-place $merged_source`;
		`$rm $coccinelle_file`;
	}
}

system("$compiler @gcc_params -I./$virtual_compilation_folder/$include @gcc_stub_params -include $virtual_compilation_folder/$include/$win_layer -c $merged_source -o $build/$final_stub");
push(@built_objects, "$build/$final_stub");
print("[*] compiling $final_stub\n");

my $final_obj_line = join(' ', @built_objects);

print("[*] compiling $final_program\n");
if($verbose){
	print("$compiler @gcc_params @gcc_final_params -I./$virtual_compilation_folder/$arch -I./$virtual_compilation_folder/$include $source/$main -o $bin/$final_program $final_obj_line @gcc_final_linkings");
	print("\n");
}

# update: as the last step, if '--enable-fw' is enabled, creates an additional header which contains the byte arrays obtained via xxd of the firmwares
$fw_header = "$include/fw.h";

if($enable_firmware_builtin){
	push(@gcc_final_params, "-include $fw_header ");
	push(@gcc_final_params, "-DUSE_BUILTIN_FIRMWARE ");
}

print("[*] creating the global callback table...\n");

# set the symbols total entries to a specific value (quite high, must support at least 4096 different symbols,
# 450+ from my implementation + every possible other symbol present in complex wifi drivers, at least 1k
$sym_total_entries = 4096;

$cb_header = "$include/$cb_table";
open(FH, '>', $cb_header) or die $!;
print FH "/* Copyright (C) Edoardo Mantovani - All Rights Reserved\n";
print FH " * Unauthorized copying of this file, via any medium is strictly prohibited\n";
print FH " * Proprietary and confidential\n";
print FH " */\n\n";
print FH "/* automatically generated file, don't edit! */\n\n";
print FH "struct internal_cb_table sym_table[$sym_total_entries] = {\n";

$proto_header = "$include/$proto_gen";
open(PROTO, '>', $proto_header) or die $!;
print PROTO "/* Copyright (C) Edoardo Mantovani - All Rights Reserved\n";
print PROTO " * Unauthorized copying of this file, via any medium is strictly prohibited\n";
print PROTO " * Proprietary and confidential\n";
print PROTO " */\n\n";
print PROTO "/* automatically generated file, don't edit! */\n\n";

$find_command_arg = q{-name "*.c"};
foreach(`$find $stubs/$functions/ $find_command_arg`){
	$current_stub = $_;
	chomp($current_stub);
	# dirty workaround: 'timer_delete' is a different function defined in 'time.h'
	if($current_stub eq "timer_delete"){
		next;
	}
	$final_stub = $current_stub;
	$final_stub =~ s/\..*$/.o/;
	$stub_name = $final_stub;
	$stub_name =~ s/\..*$//;
	$iterator = (((split("/", $stub_name))[-1]));
	# get the symbol size
	if($windows_support){
		$sym_size = `$readelf -S --size-sort --radix=x build/merged_functions.o 2>/dev/null | $grep \" $iterator\$\" 2>/dev/null | $awk $readelf_table_parse 2>/dev/null`;
		if($iterator eq "snprintf" || $iterator eq "sscanf"){
			$sym_size = "00";
		}
		$sym_size = length($sym_size) ? "0x" . $sym_size : 0;
	}else{
		$sym_size = `$readelf -sW $build/merged_functions.o 2>/dev/null | $grep \" $iterator\$\" 2>/dev/null | $awk $readelf_table_parse 2>/dev/null`;	
	}
	chomp($sym_size);
	if(!$sym_size){
		$sym_size = 0;
	}

	print FH "    [$cb_counter] = { .function_name = \"$iterator\", .function_ptr = $iterator, .function_size = $sym_size },\n";
	$cb_counter++;
	$proto = `$grep -oP '$function_proto_regex' $current_stub`;
	chomp($proto);
	if(length($proto) == 0){
		$proto = `cat $current_stub | head -n 1`;
		chomp($proto);
		$proto =~ s/{//;
	}
	print PROTO "$proto;\n";
}

foreach(`$find $stubs/$variables/ $find_command_arg`){
	$current_stub = $_;
	chomp($current_stub);
	$final_stub = $current_stub;
	$final_stub =~ s/\..*$/.o/;
	$stub_name = $final_stub;
	$stub_name =~ s/\..*$//;
	$iterator = (((split("/", $stub_name))[-1]));
	print FH "    [$cb_counter] = { .function_name = \"$iterator\", .function_ptr = &$iterator },\n";
	$cb_counter++;
	$proto = `cat $current_stub | head -n 1`;
	chomp($proto);
	$proto =~ s/=//;
	$proto =~ s/{//;
	print PROTO "extern $proto;\n";
}

print FH "};";

`$cp $cb_header $virtual_compilation_folder/$include`;
`$cp $proto_header $virtual_compilation_folder/$include`;

FASTJUMP:
# this is a fastpath: if we have already compiled everything and we are re-running the Build.pl script, just assign again $virtual_compilation_folder and jump from this point.
if($enable_fast_compilation){
	foreach(`$ls | $grep out`){
		$iterator = $_;
		chomp($iterator);
		$virtual_compilation_folder = $iterator;
		last;
	}
	print("-- $virtual_compilation_folder/build/merged_functions.o\n");
	$iterator .= "/build/merged_functions.o";
	# check if exist 'merged_functions.c' or 'merged_functions.o'
	if(-e $iterator){
		$final_obj_line = $iterator;
	}
}
print("[*] compiling main.c...\n");
system("$compiler @gcc_params -c -I./$virtual_compilation_folder/$arch -I./$virtual_compilation_folder/$include $virtual_compilation_folder/$source/$main -o $bin/$final_program");
# now reads the defined symbols under /bin/exsul
close(FH);
my $d = q{-i '$ d'};
`$sed $d $cb_header`;
open(FH, '>>', $cb_header) or die $!;

my @cmd = `$readelf -sW $bin/$final_program | $grep -iv "UND" | $grep "FUNC"`;

foreach my $iterator (@cmd){
	chomp($iterator);

	next if $iterator =~ /^\s*$/;
	next if $iterator =~ /^\s*Symbol table/;
	next if $iterator =~ /^\s*Num:/;

	$sym_name = `$echo $iterator | $awk $sym_name_parse`;
	$sym_size = `$echo $iterator | $awk $readelf_table_parse`;
	chomp($sym_name);
	chomp($sym_size);
	if($verbose){
		print("[*] adding new symbol $sym_name\n");
	}
	# fix: clear the input that needs to be inserted in the 'cb_table.h' header, avoid '.L__FUNCTION__xxx' and in general every symbol which starts with '.'
	next if !length($sym_name);
	next if $sym_name =~ /^\./;
	next if $sym_name =~ /__FUNCTION__/;
	next if $sym_name !~ /^[_A-Za-z]\w*$/;

	print FH "    [$cb_counter] = { .function_name = \"$sym_name\", .function_ptr = $sym_name, .function_size = $sym_size },\n";
	$cb_counter++;
}
print FH "    [$cb_counter] = { .function_name = NULL, .function_ptr = NULL },\n";
print FH "};\n\n";
close(FH) or die $!;
close(PROTO) or die $!;
print("[*] global callback table generated...\n");
# now delete 'exsul'
`$rm $bin/$final_program`;
# copy the new 'cb_table.h' in the out directory
`$cp $include/$cb_table $virtual_compilation_folder/$include/$cb_table`;
# now run the real compilation command
system("$compiler @gcc_params @gcc_final_params -I./$virtual_compilation_folder/$arch -I./$virtual_compilation_folder/$include $virtual_compilation_folder/$source/$main -o $bin/$final_program $final_obj_line @gcc_final_linkings");
if($windows_support){
	foreach my $dll (@windows_runtime_dlls){
		if(-e $dll){
			`$cp $dll $bin/`;
		}
	}
}
print("[*] compilation finished, output build available under ./bin!\n");
# if the 'strip' option is enabled, provide to strip the final executable by automatically choosing the 'strip' executable at runtime
if($strip){
	$exsul_sz = -s "$bin/$final_program";
	if($windows_support){
		$strip = "x86_64-w64-mingw32-strip";
	}else{
		$strip = "strip";
	}
	`$strip $bin/$final_program`;
	$exsul_f_sz = -s "$bin/$final_program";
	print("[*] stripped $bin/$final_program, final size: $exsul_f_sz bytes, deleted " . ($exsul_sz - $exsul_f_sz) . " bytes \n");
}
undef(@struct_fields);
undef(@gcc_params);
undef(@gcc_final_params);
undef(@gcc_final_linkings);
