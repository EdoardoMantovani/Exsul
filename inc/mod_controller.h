struct module_controller {
        char module_name[16];
	int fd;
        unsigned char *trampoline_area;
        int trampoline_used;
        int libusb_ret;
        struct module *mod;
        struct load_info *info;
        struct internal_cb_table *internal_cb_table;
	struct runtime_struct *runtime_generated_structs;

	/* lock used for the hashtable add/removal of an item */
	pthread_mutex_t rhashtable_lock;

        /* libusb data */
        libusb_device_handle *usb_dev;

	struct file_layer file_table[MAX_FILE_IDX];

        /* data gathered when the usb device is attached to the host system */
        u8                                usb_interface_number;
        u8                                usb_endpoint_num;
        struct libusb_endpoint_descriptor usb_ep[USB_MAX_ENDPOINT_NUM];
        struct libusb_device_descriptor   usb_dev_desc;

        /* provide the support for loading multiple lkm, permitting thus to reuse the functions from every lkm loaded */
        u8 loaded_lkm_num;
        void *lkm_current_area[MAX_ALLOCATED_LKM_NUMBER];

        struct rfkill *rfkill;
	struct ieee80211_local *ieee80211_local;
        struct cfg80211_registered_device *rdev;

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

	/* structures used for seq/procfs/sysfs virtual files */
	struct seq_list seq_list;
	struct seq_file *current_seq_file;
};
