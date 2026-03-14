/* contains usb related stuff */

/* Hard limit */
#define USB_MAXENDPOINTS	30
/* this maximum is arbitrary */
#define USB_MAXINTERFACES	32
#define USB_MAXIADS		(USB_MAXINTERFACES/2)

#define URB_SHORT_NOT_OK	0x0001	/* report short reads as errors */
#define URB_ISO_ASAP		0x0002	/* iso-only; use the first unexpired
					 * slot in the schedule */
#define URB_NO_TRANSFER_DMA_MAP	0x0004	/* urb->transfer_dma valid on submit */
#define URB_ZERO_PACKET		0x0040	/* Finish bulk OUT with short packet */
#define URB_NO_INTERRUPT	0x0080	/* HINT: no non-error interrupt
					 * needed */
#define URB_FREE_BUFFER		0x0100	/* Free transfer buffer with the URB */

/* The following flags are used internally by usbcore and HCDs */
#define URB_DIR_IN		0x0200	/* Transfer from device to host */
#define URB_DIR_OUT		0
#define URB_DIR_MASK		URB_DIR_IN

#define URB_DMA_MAP_SINGLE	0x00010000	/* Non-scatter-gather mapping */
#define URB_DMA_MAP_PAGE	0x00020000	/* HCD-unsupported S-G */
#define URB_DMA_MAP_SG		0x00040000	/* HCD-supported S-G */
#define URB_MAP_LOCAL		0x00080000	/* HCD-local-memory mapping */
#define URB_SETUP_MAP_SINGLE	0x00100000	/* Setup packet DMA mapped */
#define URB_SETUP_MAP_LOCAL	0x00200000	/* HCD-local setup packet */
#define URB_DMA_SG_COMBINED	0x00400000	/* S-G entries were combined */
#define URB_ALIGNED_TEMP_BUFFER	0x00800000	/* Temp buffer was alloc'd */

#define usb_put_urb		usb_free_urb

enum usb_interface_condition {
	USB_INTERFACE_UNBOUND = 0,
	USB_INTERFACE_BINDING,
	USB_INTERFACE_BOUND,
	USB_INTERFACE_UNBINDING,
};

struct usb_device;

struct usb_device_id {
	/* which fields to match against? */
	__u16		match_flags;

	/* Used for product specific matches; range is inclusive */
	__u16		idVendor;
	__u16		idProduct;
	__u16		bcdDevice_lo;
	__u16		bcdDevice_hi;

	/* Used for device class matches */
	__u8		bDeviceClass;
	__u8		bDeviceSubClass;
	__u8		bDeviceProtocol;

	/* Used for interface class matches */
	__u8		bInterfaceClass;
	__u8		bInterfaceSubClass;
	__u8		bInterfaceProtocol;

	/* Used for vendor-specific interface matches */
	__u8		bInterfaceNumber;

	/* not matched against */
	unsigned long	driver_info __attribute__((aligned(sizeof(unsigned long))));
};

/**
 * struct usb_host_endpoint - host-side endpoint descriptor and queue
 * @desc: descriptor for this endpoint, wMaxPacketSize in native byteorder
 * @ss_ep_comp: SuperSpeed companion descriptor for this endpoint
 * @ssp_isoc_ep_comp: SuperSpeedPlus isoc companion descriptor for this endpoint
 * @urb_list: urbs queued to this endpoint; maintained by usbcore
 * @hcpriv: for use by HCD; typically holds hardware dma queue head (QH)
 *	with one or more transfer descriptors (TDs) per urb
 * @ep_dev: ep_device for sysfs info
 * @extra: descriptors following this endpoint in the configuration
 * @extralen: how many bytes of "extra" are valid
 * @enabled: URBs may be submitted to this endpoint
 * @streams: number of USB-3 streams allocated on the endpoint
 *
 * USB requests are always queued to a given endpoint, identified by a
 * descriptor within an active interface in a given USB configuration.
 */
struct usb_host_endpoint {
	struct usb_endpoint_descriptor		desc;
	struct usb_ss_ep_comp_descriptor	ss_ep_comp;
	struct usb_ssp_isoc_ep_comp_descriptor	ssp_isoc_ep_comp;
	struct list_head			urb_list;
	void					*hcpriv;
	struct ep_device			*ep_dev;	/* For sysfs info */

	unsigned char				*extra;   /* Extra descriptors */
	int extralen;
	int enabled;
	int streams;
};

struct usb_iso_packet_descriptor {
	unsigned int offset;
	unsigned int length;		/* expected length */
	unsigned int actual_length;
	int status;
};

/* host-side wrapper for one interface setting's parsed descriptors */
struct usb_host_interface {
	struct usb_interface_descriptor	desc;

	int extralen;
	unsigned char *extra;   /* Extra descriptors */

	/* array of desc.bNumEndpoints endpoints associated with this
	 * interface setting.  these will be in no particular order.
	 */
	struct usb_host_endpoint *endpoint;

	char *string;		/* iInterface string, if present */
};

/**
 * struct usb_interface_cache - long-term representation of a device interface
 * @num_altsetting: number of altsettings defined.
 * @ref: reference counter.
 * @altsetting: variable-length array of interface structures, one for
 *	each alternate setting that may be selected.  Each one includes a
 *	set of endpoint configurations.  They will be in no particular order.
 *
 * These structures persist for the lifetime of a usb_device, unlike
 * struct usb_interface (which persists only as long as its configuration
 * is installed).  The altsetting arrays can be accessed through these
 * structures at any time, permitting comparison of configurations and
 * providing support for the /sys/kernel/debug/usb/devices pseudo-file.
 */
struct usb_interface_cache {
	unsigned num_altsetting;	/* number of alternate settings */
	struct kref ref;		/* reference counter */

	/* variable-length array of alternate settings for this interface,
	 * stored in no particular order */
	struct usb_host_interface altsetting[];
};

struct usb_interface {
	/* array of alternate settings for this interface,
	 * stored in no particular order */
	struct usb_host_interface *altsetting;

	struct usb_host_interface *cur_altsetting;	/* the currently
					 * active alternate setting */
	unsigned num_altsetting;	/* number of alternate settings */

	/* If there is an interface association descriptor then it will list
	 * the associated interfaces */
	struct usb_interface_assoc_descriptor *intf_assoc;

	int minor;			/* minor number this interface is
					 * bound to */
	enum usb_interface_condition condition;		/* state of binding */
	unsigned sysfs_files_created:1;	/* the sysfs attributes exist */
	unsigned ep_devs_created:1;	/* endpoint "devices" exist */
	unsigned unregistering:1;	/* unregistration is in progress */
	unsigned needs_remote_wakeup:1;	/* driver requires remote wakeup */
	unsigned needs_altsetting0:1;	/* switch to altsetting 0 is pending */
	unsigned needs_binding:1;	/* needs delayed unbind/rebind */
	unsigned resetting_device:1;	/* true: bandwidth alloc after reset */
};

struct usb_anchor {
	struct list_head urb_list;
	wait_queue_head_t wait;
	u32 lock;
	atomic_int suspend_wakeups;
	unsigned int poisoned:1;
};

struct urb {
	/* the order of the first elements isn't important, luckily */
	struct kref kref;		/* reference count of the URB */
	int unlinked;			/* unlink error code */
	void *hcpriv;			/* private data for host controller */
	atomic_uint use_count;		/* concurrent submissions counter */
	atomic_uint reject;		/* submissions will fail */

	struct list_head urb_list;	/* list head for use by the urb's current owner */
	struct list_head anchor_list;	/* the URB may be anchored */
	struct usb_anchor *anchor;
	/* ... */
};

/**
 * struct usb_host_config - representation of a device's configuration
 * @desc: the device's configuration descriptor.
 * @string: pointer to the cached version of the iConfiguration string, if
 *	present for this configuration.
 * @intf_assoc: list of any interface association descriptors in this config
 * @interface: array of pointers to usb_interface structures, one for each
 *	interface in the configuration.  The number of interfaces is stored
 *	in desc.bNumInterfaces.  These pointers are valid only while the
 *	configuration is active.
 * @intf_cache: array of pointers to usb_interface_cache structures, one
 *	for each interface in the configuration.  These structures exist
 *	for the entire life of the device.
 * @extra: pointer to buffer containing all extra descriptors associated
 *	with this configuration (those preceding the first interface
 *	descriptor).
 * @extralen: length of the extra descriptors buffer.
 *
 * USB devices may have multiple configurations, but only one can be active
 * at any time.  Each encapsulates a different operational environment;
 * for example, a dual-speed device would have separate configurations for
 * full-speed and high-speed operation.  The number of configurations
 * available is stored in the device descriptor as bNumConfigurations.
 *
 * A configuration can contain multiple interfaces.  Each corresponds to
 * a different function of the USB device, and all are available whenever
 * the configuration is active.  The USB standard says that interfaces
 * are supposed to be numbered from 0 to desc.bNumInterfaces-1, but a lot
 * of devices get this wrong.  In addition, the interface array is not
 * guaranteed to be sorted in numerical order.  Use usb_ifnum_to_if() to
 * look up an interface entry based on its number.
 *
 * Device drivers should not attempt to activate configurations.  The choice
 * of which configuration to install is a policy decision based on such
 * considerations as available power, functionality provided, and the user's
 * desires (expressed through userspace tools).  However, drivers can call
 * usb_reset_configuration() to reinitialize the current configuration and
 * all its interfaces.
 */
struct usb_host_config {
	struct usb_config_descriptor	desc;

	char *string;		/* iConfiguration string, if present */

	/* List of any Interface Association Descriptors in this
	 * configuration. */
	struct usb_interface_assoc_descriptor *intf_assoc[USB_MAXIADS];

	/* the interfaces associated with this configuration,
	 * stored in no particular order */
	struct usb_interface *interface[USB_MAXINTERFACES];

	/* Interface information available even when this is not the
	 * active configuration */
	struct usb_interface_cache *intf_cache[USB_MAXINTERFACES];

	unsigned char *extra;   /* Extra descriptors */
	int extralen;
};

struct usb_driver {
	const char *name;
	int (*probe) (struct usb_interface *intf, const struct usb_device_id *id);
	void (*disconnect) (struct usb_interface *intf);
};

static bool match_endpoint(struct usb_endpoint_descriptor *epd,
		struct usb_endpoint_descriptor **bulk_in,
		struct usb_endpoint_descriptor **bulk_out,
		struct usb_endpoint_descriptor **int_in,
		struct usb_endpoint_descriptor **int_out){
	switch (usb_endpoint_type(epd)) {
	case USB_ENDPOINT_XFER_BULK:
		if (usb_endpoint_dir_in(epd)) {
			if (bulk_in && !*bulk_in) {
				*bulk_in = epd;
				break;
			}
		} else {
			if (bulk_out && !*bulk_out) {
				*bulk_out = epd;
				break;
			}
		}

		return false;
	case USB_ENDPOINT_XFER_INT:
		if (usb_endpoint_dir_in(epd)) {
			if (int_in && !*int_in) {
				*int_in = epd;
				break;
			}
		} else {
			if (int_out && !*int_out) {
				*int_out = epd;
				break;
			}
		}

		return false;
	default:
		return false;
	}

	return (!bulk_in || *bulk_in) && (!bulk_out || *bulk_out) && (!int_in || *int_in) && (!int_out || *int_out);
}

static void urb_destroy(struct kref *kref){
	struct urb *urb = NULL;
	void *__mptr = (void *)(kref);
	/* for avoiding problems, unroll the 'container_of' macro */
	urb = ((struct urb *)(__mptr - 0));

	if (urb->transfer_flags & URB_FREE_BUFFER){
		kfree(urb->transfer_buffer);
	}

	kfree(urb);
}

/**
 * usb_find_endpoint() - Given an endpoint address, search for the endpoint's
 * usb_host_endpoint structure in an interface's current altsetting.
 * @intf: the interface whose current altsetting should be searched
 * @ep_addr: the endpoint address (number and direction) to find
 *
 * Search the altsetting's list of endpoints for one with the specified address.
 *
 * Return: Pointer to the usb_host_endpoint if found, %NULL otherwise.
 */
static const struct usb_host_endpoint *usb_find_endpoint(struct usb_interface *intf, unsigned int ep_addr){
	int n = 0;
	struct usb_host_endpoint *ep = NULL;

	n = intf->cur_altsetting->desc.bNumEndpoints;
	ep = intf->cur_altsetting->endpoint;
	for (; n > 0; (--n, ++ep)) {
		if (ep->desc.bEndpointAddress == ep_addr){
			return ep;
		}
	}
	return NULL;
}

static void *get_symbol_data(unsigned char *symbol_name, int *symbol_size){
        int n = 0;
        void *ptr = NULL;
        void *strtab = NULL;
        Elf(Sym) *sym = NULL;
        Elf(Shdr) *symsect = NULL;
        struct load_info *info = NULL;
        unsigned char *cur_name = NULL;

        info = director.info;

        symsect = &info->sechdrs[info->index.sym];

	sym = (void *)symsect->sh_addr;
        strtab = (void *)info->sechdrs[info->index.str].sh_addr;
        n = symsect->sh_size / sizeof(Elf(Sym));

        for (unsigned int i = 0; i < n; i++) {
                cur_name = &strtab[sym[i].st_name];

                if (cur_name == NULL || cur_name[0] == '\0'){
                        continue;
                }

                if (strstr(cur_name, (char *)symbol_name) != NULL) {
                        *symbol_size = sym[i].st_size;
                        ptr = malloc(sym[i].st_size);
                        memset(ptr, 0, sym[i].st_size);
                        memcpy(ptr, sym[i].st_value, sym[i].st_size);
                }
        }
        return ptr;
}

static int parse_module_usb_table(void){
	int ret = 0;
	int usb_table_size = 0;
	struct usb_device_id *p = NULL;

	p = (struct usb_device_id *)get_symbol_data(USB_TABLE_SYM_NAME, &usb_table_size);
	if(p == NULL || usb_table_size == 0){
		ret = -ENOMEM;
		return ret;
	}

	ret = (usb_table_size % sizeof(struct usb_device_id));
	if(ret != 0){
		printf("[!] usb device table symbol isn't aligned with the size of \'usb_device_id\', aborting...\n");
		free(p);
		ret = -EINVAL;
		return ret;
	}

	printf("[*] usb devices detected for the current kernel module:\n");
	for(u8 j = 0; ; j++){
		if(p == NULL || p->idProduct == 0x00 || p->idVendor == 0x00){
			break;
		}
		printf("[*] usb pid: 0x%08x vid: 0x%08x\n", p->idProduct, p->idVendor);
		/* now add the pid/vid tuple into our main struct, this will be used later for the libusb device handling */
		for(u8 k = 0; k < MAX_USB_DEV_LIST; k++){
			if(director.usb_dev_list[k].is_used == false){
				director.usb_dev_list[k].usb_vid = p->idVendor;
				director.usb_dev_list[k].usb_pid = p->idProduct;
				director.usb_dev_list[k].is_used = true;
				break;
			}
		}
		p++;
	}

	return ret;
}

/* main function for detecting the attached usb dongle, requires a full 'usb_dev_list' table */
static int detect_usb_device(void){
	int ret = 0;
	for (u8 j = 0; j < MAX_USB_DEV_LIST; j++) {
		director.usb_dev = libusb_open_device_with_vid_pid(NULL, director.usb_dev_list[j].usb_vid, director.usb_dev_list[j].usb_pid);
		if (director.usb_dev != NULL) {
			printf("[*] new device found:\n-pid: 0x%04x\n-vid: 0x%04x\n",
                            director.usb_dev_list[j].usb_pid,
			    director.usb_dev_list[j].usb_vid);
			ret = libusb_claim_interface(director.usb_dev, director.usb_interface_number);
			if (ret < 0) {
				printf("[!] failed to claim interface!\n");
				printf("[!] error: %d\n", ret);
				goto end;
			}
			break;
                }

		if ((j + 1) == MAX_USB_DEV_LIST) {
			/* sleep and reset the counter */
			usleep(1000);
			j = 0;
		}
	}
	end:
	return ret;
}

static int detect_usb_endpoints(void){
	int ret = 0;
	libusb_device *usb_dev = NULL;
	struct libusb_config_descriptor *cfg = NULL;

	usb_dev = libusb_get_device(director.usb_dev);

	ret = libusb_get_active_config_descriptor(usb_dev, &cfg);

	if (ret == LIBUSB_ERROR_NOT_FOUND) {
		ret = libusb_get_config_descriptor(usb_dev, 0, &cfg);
	}
	#if 0
	if (ret) {
		printf("[%s] failed to get the usb configuration descriptor!\n", __FUNCTION__);
		return ret;
	}

	for (u8 i = 0; i < cfg->bNumInterfaces; i++) {
                const struct libusb_interface *itf = &cfg->interface[i];

                for (int a = 0; a < itf->num_altsetting; a++) {
                        const struct libusb_interface_descriptor *id =
                            &itf->altsetting[a];

                        u8 out = 0, in = 0;
                        for (u8 e = 0; e < id->bNumEndpoints; e++) {
                                const struct libusb_endpoint_descriptor *ep =
                                    &id->endpoint[e];
                                u8 type = ep->bmAttributes &
                                          LIBUSB_TRANSFER_TYPE_MASK;
                                u8 dir = ep->bEndpointAddress &
                                         LIBUSB_ENDPOINT_DIR_MASK;

                                if (type == LIBUSB_TRANSFER_TYPE_BULK) {
                                        if (dir == LIBUSB_ENDPOINT_OUT &&
                                            out == 0)
                                                out = ep->bEndpointAddress;
                                        if (dir == LIBUSB_ENDPOINT_IN &&
                                            in == 0)
                                                in = ep->bEndpointAddress;
                                }
                        }

                        if (!out) {
                                continue;
                        }

                        int score = (id->bInterfaceClass == 0x08) ? 2 : 1;

                        if (score > best_score) {
                                best_score = score;
                                best_iface = id->bInterfaceNumber;
                                best_alt   = id->bAlternateSetting;
                                best_out   = out;
                                best_in    = in;
                        }
                }
        }

        libusb_free_config_descriptor(cfg);

        if (!best_out) {
                return LIBUSB_ERROR_NOT_FOUND;
        }
        dev->iface_num   = best_iface;
        dev->alt_setting = best_alt;
        dev->bulk_out    = best_out;
        dev->bulk_in     = best_in;
	#endif
	return ret;
}

/* due to the fact that the usb driver entry point functions usually employs the structs defined in /usr/include/ch9/xxx.h,
 * we need to create a translation layer between libusb and those structs, the function below have this purpose.
 */
static int convert_ss_ep_companion(struct usb_ss_ep_comp_descriptor *ep_dst, struct libusb_ss_endpoint_companion_descriptor *ep_src){
	int ret = 0;
	if(ep_dst == NULL || ep_src == NULL){
		ret = -EINVAL;
		return ret;
	}

	ep_dst->bLength           = ep_src->bLength;
	ep_dst->bDescriptorType   = ep_src->bDescriptorType;
	ep_dst->bMaxBurst         = ep_src->bMaxBurst;
	ep_dst->bmAttributes      = ep_src->bmAttributes;
	ep_dst->wBytesPerInterval = ep_src->wBytesPerInterval;
	return ret;
}

static int convert_usb_endpoint_desc(struct usb_endpoint_descriptor *ep_dst, struct libusb_endpoint_descriptor *ep_src){
	int ret = 0;
	if(ep_dst == NULL || ep_src == NULL){
		ret = -EINVAL;
		return ret;
	}

	ep_dst->bLength          = ep_src->bLength;
	ep_dst->bDescriptorType  = ep_src->bDescriptorType;
	ep_dst->bEndpointAddress = ep_src->bEndpointAddress;
	ep_dst->bmAttributes     = ep_src->bmAttributes;
	ep_dst->wMaxPacketSize   = ep_src->wMaxPacketSize;
	ep_dst->bInterval        = ep_src->bInterval;
	ep_dst->bRefresh         = ep_src->bRefresh;
	ep_dst->bSynchAddress    = ep_src->bSynchAddress;
	return ret;
}

static int convert_usb_interface_desc(struct usb_interface_descriptor *ep_dst, struct libusb_interface_descriptor *ep_src){
	int ret =  0;
	if(ep_dst == NULL || ep_src == NULL){
		ret = -EINVAL;
		return ret;
	}

	ep_dst->bLength            = ep_src->bLength;
	ep_dst->bDescriptorType    = ep_src->bDescriptorType;
	ep_dst->bInterfaceNumber   = ep_src->bInterfaceNumber;
	ep_dst->bAlternateSetting  = ep_src->bAlternateSetting;
	ep_dst->bNumEndpoints      = ep_src->bNumEndpoints;
	ep_dst->bInterfaceClass    = ep_src->bInterfaceClass;
	ep_dst->bInterfaceSubClass = ep_src->bInterfaceSubClass;
	ep_dst->bInterfaceProtocol = ep_src->bInterfaceProtocol;
	ep_dst->iInterface         = ep_src->iInterface;
	return ret;
}

static int get_altsetting_endpoints_enabled(struct usb_host_interface *alt){
	int ret = 0;

	if (alt == NULL){
		ret = -EINVAL;
		return ret;
	}

	if(alt->endpoint == NULL){
		ret = -EINVAL;
		return ret;
	}

	for (int i = 0; i < alt->desc.bNumEndpoints; i++){
		alt->endpoint[i].enabled = 1;
	}
	return ret;
}
