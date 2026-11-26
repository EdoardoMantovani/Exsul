struct fast_struct_size {
	u16 file;
	u16 inode;
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
	u16 usb_bus;
        u16 device;
        u16 hci_dev;
	u16 cfg80211_chan_def;
	u16 ieee80211_chanctx_conf;
	u16 pci_dev;
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
	/* Preserve the AP's advertised security/rates for connection negotiation. */
	u16 ies_len;
	u8 ies[1024];
};

struct peer_runtime;
struct mesh_peer;
struct peer_retired_interface;

struct module_controller {
        char module_name[32];
	/* current module kernel version and Ubuntu ABI */
	u8 kernel_major_version;
	u8 kernel_minor_version;
	u8 kernel_patch;
	u16 kernel_abi_version;
	/* necessary for PCIe related task, it is the output of sysconf(_SC_PAGESIZE) */
	u16 page_size;
	/* mantain the count of the loaded modules in the current address space */
	int modules_loaded;
	/* mantain a track of the current memory used for everything, in bytes */
	int total_memory_used;
	int fd;
	/* pcap file descriptor, enabled with the 'ops rx on' command */
	int pcap_fd;
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
	volatile sig_atomic_t elf_shutdown_requested;
	int elf_signal_pipe[2];
	volatile u32 pe_standalone_mode;

        struct module *mod;
        struct load_info *info;
        struct internal_cb_table *internal_cb_table;
	unsigned int native_cb_entries;
	struct runtime_struct *runtime_generated_structs;

	/* lock used for the hashtable add/removal of an item */
	pthread_mutex_t rhashtable_lock;

        /* libusb data, use only 1 device handler for an entire application */
        libusb_context *usb_ctx;
        libusb_device_handle *usb_dev;
	/* pointers to the 'usb_device' structs allocated for the 'probe' callback */
	void *usb_device_ptr;

	/* backup args used for running the 'xxx_driver_init' function */
	struct usb_interface *usb_interface;
	struct usb_device_id *usb_device_id;

        /* data gathered when the usb device is attached to the host system */
	u8 usb_multi_state;
        u8 usb_interface_number;
	u32 usb_claimed_interfaces;
        u8 usb_endpoint_num;
        struct libusb_endpoint_descriptor usb_ep[USB_MAX_ENDPOINT_NUM];
        struct libusb_device_descriptor usb_dev_desc;
	pthread_mutex_t urb_anchor_mutex;
	pthread_mutex_t usb_rx_observer_lock;
	pthread_cond_t usb_rx_observer_idle;
	int (*usb_rx_observer)(void *data, int len);
	unsigned int usb_rx_observer_refs;
	bool usb_rx_observer_initialized;

	pthread_mutex_t skb_queue_lock;
	bool connection_tx_trace;
	atomic_bool rx_dump_enabled;
	atomic_uint rx_dump_sequence;

	/* threads associated with libusb's async urb transfer, permits to implement the 'usb_send_urb' kernel function */
	pthread_t libusb_event_thread;
	atomic_bool libusb_thread_stop;
	atomic_bool usb_device_lost;
	struct usb_driver *bound_usb_driver;
	atomic_bool finalizing;
	bool runtime_initialized;
	bool mac80211_started;
	u8 mac80211_interfaces; /* successful add_interface calls: station/AP/monitor */

	pthread_mutex_t	usb_transfer_pool_lock;
	struct libusb_transfer *usb_transfer_pool[USB_TRANSFER_POOL_SIZE];
	struct libusb_transfer *usb_active_transfers[USB_ACTIVE_TRANSFER_MAX];
	u16 usb_transfer_pool_count;
	u16 usb_active_transfer_count;
	atomic_uint usb_pending_transfers;
	pthread_mutex_t usb_completion_lock;
	pthread_cond_t usb_completion_cond;
	bool usb_completion_initialized;

	/* locks for usb i/o operations */
	pthread_mutex_t usb_io_lock;
	pthread_cond_t usb_io_idle;
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
	/* main tcc context */
	TCCState *tcc_ctx;
	/* buffer for tcc jit "script" */
	unsigned char *tcc_memarea;
	/* mutex used for running the tcc jit "script" */
	pthread_mutex_t tcc_mutex;
	/* support multiple tcc instances */
	TCCState *runtime_tcc_ctx[MAX_TCC_RUNTIME_CONTEXTS];
	u8 runtime_tcc_ctx_count;
	#endif

	/* special flags used for indicating that the offsets have already been given by a configuration file, this permits to skip the dwarf parsing part during the lkm loading */
	u8 offset_init_by_conf;
        /* provide the support for loading multiple lkm, permitting thus to reuse the functions from every lkm loaded */
        u8 loaded_lkm_num;
	/* track every memory region mapped in the actual memory, it doesn't matter much the original module they belongs to */
	struct allocation_list *module_allocation_list;

	#ifdef EXSUL_PCI_SUPPORT
	struct exsul_vfio *vfio;
	struct pci_driver *bound_pci_driver;
	struct pci_dev *pci_device;
	struct pci_device_id *pci_device_id;
	#endif

	#ifdef ENABLE_FUZZING
	/* set to true when in fuzzing mode, hooks the USB bus I/O functions for redirecting/parsing the device interface */
	u8 fuzzing_mode_enabled;
	/* Generic, owned feedback queue populated by a driver-specific observer. */
	struct fuzz_urb_list *fuzz_urb_list;
	struct fuzz_urb_list *fuzz_urb_tail;
	size_t fuzz_urb_count;
	pthread_mutex_t fuzz_lock;
	pthread_cond_t fuzz_cond;
	u8 fuzz_runtime_initialized;
	unsigned int fuzz_callback_refs;
	/* this callback is driver specific and it is used for appending a frame to the urb fuzz queue, in general, it must
	 * parse the data and submit normalized feedback with fuzz_urb_append(). */
	int (*fuzz_accept_valid_urb)(void *data, int len);
	#endif

	/* kernel based structs */
	struct wiphy *wiphy;
	struct wireless_dev *wdev;
	struct net_device *net_device;
        struct rfkill *rfkill;
	struct ieee80211_local *ieee80211_local;
        struct cfg80211_registered_device *rdev;
	struct cfg80211_wowlan *cfg80211_wowlan;
	struct exsul_scan_result scan_results[EXSUL_MAX_SCAN_RESULTS];
	u32 scan_result_count;
	pthread_mutex_t scan_cache_lock;
	pthread_mutex_t wiphy_work_queue_mutex;
	atomic_uint scan_generation;
	atomic_uint scan_result_epoch;
	_Atomic(u64) scan_complete_deadline;
	_Atomic(u64) scan_watchdog_deadline;
	_Atomic(u64) scan_print_deadline;
	void *active_scan_req;
	atomic_bool scan_in_progress;
	pthread_t wifi_scan_thread;
	atomic_bool wifi_scan_thread_started;
	atomic_bool wifi_scan_thread_stop;
	_Atomic(void *) connection_fsm;
	void *connection_retired;
	pthread_mutex_t connection_lock;
	pthread_mutex_t connection_control_lock;
	pthread_cond_t connection_wake;
	u16 connection_tx_cookie;
	bool connection_runtime_initialized;

	/* mesh/P2P state */
	struct peer_runtime *peer_runtime;
	struct mesh_peer *mesh_retired_peers;
	struct peer_retired_interface *peer_retired_interfaces;
	pthread_mutex_t peer_control_lock;
	pthread_mutex_t peer_state_lock;
	pthread_mutex_t peer_rx_lock;
	bool peer_sync_initialized;

	pthread_mutex_t rtnl_mutex;
	/* store here the backup for every 'ops' struct, it will be called later via the 'ops' command */
	struct ieee80211_ops *ieee80211_ops;
	struct cfg80211_ops *cfg80211_ops;

	/* access point mode configuration */
	u8 *ap_ssid;
	u8 ap_ssid_len;
	u8 ap_ssid_storage[33];
	u8 ap_channel;
	bool ap_started;

	/* array of pointers which emulates the functions 'devm_kxxx', when the driver will quit, every element of the array will be freed */
	void *kmem_array[MAX_KMEM_ARRAY_SIZE];

	struct usb_dev_tuple usb_dev_list[MAX_USB_DEV_LIST];

	u8 debugfs_buffer[MAX_DEBUGFS_SIZE];
	/* create 2 buffers: one for emulating the userspace-driver data input and the other for emulating the driver-userspace communication */
	u8 userspace_to_driver_buffer[MAX_USERSPACE_TO_DRIVER_IO_SIZE];
	u8 driver_to_userspace_buffer[MAX_DRIVER_TO_USERSPACE_IO_SIZE];

	/* dwarf data, used for handling the libdwarf context */
	#ifndef EXSUL_NO_DWARF
	Dwarf_Debug dbg;
	Dwarf_Error err;
	#endif

	struct runtime_single_fields runtime_single_fields;
	struct list_head wakeup_sources;

	/* global workqueues */
	struct workqueue_struct system_wq;
	struct workqueue_struct system_percpu_wq;
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

	/* tasklets related data */
	u8 tasklet_numbers;
	atomic_int tasklet_sync_state;
	bool tasklet_initialized;
	bool tasklet_stopped;
	pthread_mutex_t tasklet_lock;
        pthread_cond_t tasklet_wake;
	struct tasklet_head tasklet_normal_priority;
	struct tasklet_head tasklet_high_priority;
	struct tasklet_runtime_state *tasklet_runtime_states;
	pthread_t *tasklet_workers;
	unsigned int number_tasklet_workers;

	/* timing synchronization primitives used in 'timer_xxx' functions */
	pthread_mutex_t timer_lock;
	pthread_cond_t timer_cond;
	struct hlist_head timer_head;
	pthread_once_t timer_once;
	pthread_t timer_thread;
	bool timer_stopping;
	struct timespec	timer_base;
	/* pointer to the timer whose callback is currently executing, NULL otherwise */
	struct timer_list *timer_executing;

	/* hci specific data */
	struct hci_dev *hci_dev;
	struct list_head hci_dev_list;
	struct list_head hci_cb_list;
	char hci_name[16];
	int hci_setup_status;
	bool hci_stopping;
	bool hci_setup_complete;
	bool hci_raw_initialized;
	bool hci_state_initialized;
	pthread_mutex_t hci_raw_lock;
	pthread_cond_t hci_raw_cond;
	pthread_mutex_t hci_state_lock;
	pthread_cond_t hci_state_cond;
	pthread_mutex_t bit_wait_lock;
	pthread_cond_t bit_wait_cond;
};
