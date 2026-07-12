struct global_conf {
	u8 show_struct_offset;
	u8 strict_missing_syms;
};

struct fast_struct_size {
	u16 file;
	u16 wiphy;
	u16 cfg80211_ops;
	u16 cfg80211_scan_request;
	u16 cfg80211_ap_settings;
	u16 ieee80211_scan_request;
	u16 ieee80211_sta;
        u16 ieee80211_hw;
        u16 ieee80211_ops;
	u16 ieee80211_vif;
	u16 ieee80211_bss_conf;
        u16 module;
        u16 net_device;
        u16 rate_info;
        u16 sk_buff;
        u16 sk_buff_head;
        u16 skb_shared_info;
        u16 sock;
        u16 urb;
        u16 usb_device;
        u16 usb_driver;
        u16 usb_interface;
        u16 device;
        u16 hci_dev;
};

struct exsul_scan_result {
	bool valid;
	bool from_probe_resp;
	u8 bssid[ETH_ALEN];
	u8 ssid[EXSUL_MAX_SCAN_SSID_LEN];
	u8 ssid_len;
	u8 channel;
	u16 beacon_interval;
	u16 capability;
	u32 center_freq;
	s8 signal;
	u32 seen;
	u32 last_seen_generation;
};

#define MAX_TCC_RUNTIME_CONTEXTS 4

struct module_controller {
        char module_name[32];
	/* current module major, minor and patch version */
	u8 kernel_major_version;
	u8 kernel_minor_version;
	u8 kernel_patch;
	/* mantain the count of the loaded modules in the current address space */
	int modules_loaded;
	/* mantain a track of the current memory used for everything, in bytes */
	int total_memory_used;
	int fd;
	u8 *trampoline_area;
        int trampoline_used;

	/* emulates jiffies */
	u64 jiffies;

	/* special flag which permits to search for the vid/pid of the 'DISK' version of the usb dongles */
	bool enable_usb_disk_search;

        int libusb_ret;

	/* socket file handler used for the standalone ELF ipc */
	int socket_fd;

	/* socket flags */
	int socket_flags;

	/* ipc client */
	int client_flags;

	/* global configuration struct, the field can be set at runtime via the 'set' command */
	struct global_conf global_conf;
        struct module *mod;
        struct load_info *info;
        struct internal_cb_table *internal_cb_table;
	struct runtime_struct *runtime_generated_structs;

	/* lock used for the hashtable add/removal of an item */
	pthread_mutex_t rhashtable_lock;

        /* libusb data, use only 1 device handler for an entire application */
        libusb_device_handle *usb_dev;
	/* pointers to the 'usb_device' structs allocated for the 'probe' callback */
	void *usb_device_ptr;

	/* backup args used for running the 'xxx_driver_init' function */
	struct usb_interface *usb_interface;
	struct usb_device_id *usb_device_id;

	struct file_layer file_table[MAX_FILE_IDX];

        /* data gathered when the usb device is attached to the host system */
        u8                                usb_interface_number;
        u8                                usb_endpoint_num;
        struct libusb_endpoint_descriptor usb_ep[USB_MAX_ENDPOINT_NUM];
        struct libusb_device_descriptor   usb_dev_desc;
	pthread_mutex_t			  urb_anchor_mutex;

	pthread_mutex_t			  skb_queue_lock;
	bool				  connection_tx_trace;
	atomic_bool			  rx_dump_enabled;
	atomic_uint			  rx_dump_sequence;

	/* threads associated with libusb's async urb transfer, permits to implement the 'usb_send_urb' kernel function */
	pthread_t			  libusb_event_thread;
	atomic_bool			  libusb_thread_stop;

	/* locks for usb i/o operations */
	pthread_mutex_t usb_io_lock;
	pthread_cond_t  usb_io_idle;
	int usb_io_depth;

	/* official init callback present in 'struct module' */
	int (LINUX_ABI *init_cb)(void);
	/* init driver function */
	int (LINUX_ABI *driver_init_function)(void);
	/* backup copy of the 'usb_device_id' table */
	void *usb_table_ptr;
	int usb_table_size;

	u8 **envp;
	/* base pointer of the region header mapped in memory, used for creating the userspace independent executable */
	u8 segments_in_memory;
	void *segment_header_in_memory;

	/* tcc scripting data */
	#ifdef TCC_SCRIPTING
	TCCState *tcc_ctx;
	unsigned char *tcc_memarea;
	pthread_mutex_t tcc_mutex;
	TCCState *runtime_tcc_ctx[MAX_TCC_RUNTIME_CONTEXTS];
	u8 runtime_tcc_ctx_count;
	#endif

	/* special flags used for indicating that the offsets have already been given by a configuration file, this permits to skip the dwarf parsing part during the lkm loading */
	u8 offset_init_by_conf;
        /* provide the support for loading multiple lkm, permitting thus to reuse the functions from every lkm loaded */
        u8 loaded_lkm_num;
	/* track every memory region mapped in the actual memory, it doesn't matter much the original module they belongs to */
	struct allocation_list *module_allocation_list;

	/* kernel based structs */
	struct wiphy *wiphy;
	struct wireless_dev *wdev;
	struct net_device *net_device;
        struct rfkill *rfkill;
	struct ieee80211_local *ieee80211_local;
        struct cfg80211_registered_device *rdev;
	struct exsul_scan_result scan_results[EXSUL_MAX_SCAN_RESULTS];
	u32 scan_result_count;
	pthread_mutex_t scan_cache_lock;
	atomic_uint scan_generation;
	atomic_uint scan_result_epoch;
	void *active_scan_req;
	atomic_bool scan_in_progress;
	void *connection_fsm;

	pthread_mutex_t rtnl_mutex;
	/* store here the backup for every 'ops' struct, it will be called later via the 'ops' command */
	struct ieee80211_ops *ieee80211_ops;
	struct cfg80211_ops  *cfg80211_ops;

	/* access point mode configuration */
	u8 *ap_ssid;
	u8  ap_ssid_len;

	/* array of pointers which emulates the functions 'devm_kxxx', when the driver will quit, every element of the array will be freed */
	void *kmem_array[MAX_KMEM_ARRAY_SIZE];

	struct usb_dev_tuple usb_dev_list[MAX_USB_DEV_LIST];

	u8 debugfs_buffer[MAX_DEBUGFS_SIZE];
	/* create 2 buffers: one for emulating the userspace-driver data input and the other for emulating the driver-userspace communication */
	u8 userspace_to_driver_buffer[MAX_USERSPACE_TO_DRIVER_IO_SIZE];
	u8 driver_to_userspace_buffer[MAX_DRIVER_TO_USERSPACE_IO_SIZE];

	/* dwarf data, used for handling the libdwarf context */
	Dwarf_Debug dbg;
	Dwarf_Error err;

	struct runtime_single_fields runtime_single_fields;

	struct list_head wakeup_sources;

	/* global workqueues */
	struct workqueue_struct system_wq;
	struct workqueue_struct system_freezable_wq;
	struct workqueue_struct system_dfl_wq;

	/* structures used for seq/procfs/sysfs virtual files */
	struct seq_list seq_list;
	struct seq_file *current_seq_file;

	/* firmware builtin data */
	#ifdef USE_BUILTIN_FIRMWARE
	struct firmware_builtin_space *fw_builtin;
	#endif
	struct firmware *firmware_ptr;

	/* list of the allocated memory, used for freeing everything at the end */
	struct allocation_list *allocation_list;

	/* store some important struct sizes, done once foreach module loaded */
	struct fast_struct_size struct_sizes;

	/* timing synchronization primitives used in 'timer_xxx' functions */
	pthread_mutex_t   	timer_lock;
	pthread_cond_t    	timer_cond;
	struct hlist_head 	timer_head;
	pthread_once_t  	timer_once;
	pthread_t 		timer_thread;
	struct timespec 	timer_base;
	/* pointer to the timer whose callback is currently executing, NULL otherwise */
	struct timer_list	*timer_executing;

	/* hci specific data */
	struct list_head hci_dev_list;
	struct list_head hci_cb_list;
};
