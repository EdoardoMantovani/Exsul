/* contains usb related stuff */

/* macro for translating a pipe (used only in the linux kernel) into an endpoint (used in libusb) */
#define usb_pipeendpoint(pipe)  (((pipe) >> 15) & 0xf)

/* macro used for finding out if the pipe is IN, otherwise is OUT */
#define usb_pipein(pipe)        ((pipe) & USB_DIR_IN)

/* macros used for getting the endpoint/pipe type */
#define usb_pipetype(pipe)      (((pipe) >> 30) & 3)
#define usb_pipeisoc(pipe)      (usb_pipetype((pipe)) == PIPE_ISOCHRONOUS)
#define usb_pipeint(pipe)       (usb_pipetype((pipe)) == PIPE_INTERRUPT)
#define usb_pipecontrol(pipe)   (usb_pipetype((pipe)) == PIPE_CONTROL)
#define usb_pipebulk(pipe)      (usb_pipetype((pipe)) == PIPE_BULK)

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
	u64		driver_info __attribute__((aligned(sizeof(u64))));
};

/* USB_DT_EUSB2_ISOC_ENDPOINT_COMP: eUSB2 Isoch Endpoint Companion descriptor */
struct usb_eusb2_isoc_ep_comp_descriptor {
	__u8	bLength;
	__u8	bDescriptorType;
	__le16	wMaxPacketSize;
	__le32	dwBytesPerInterval;
} __attribute__ ((packed));

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
	struct usb_eusb2_isoc_ep_comp_descriptor eusb2_isoc_ep_comp;
	struct list_head			urb_list;
	void					*hcpriv;
	struct ep_device			*ep_dev;	/* For sysfs info */

	unsigned char				*extra;   /* Extra descriptors */
	int					extralen;
	int					enabled;
	int					streams;
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

/* used in 'struct usb_driver', never changes across kernel versions, the last field 'for_devices' is almost useless and deprecated in the latest kernel v7.0 */
struct usbdrv_wrap {
	struct device_driver driver;
	int for_devices;
};

struct usb_driver {
	const char *name;
	int (LINUX_ABI *probe) (struct usb_interface *intf, const struct usb_device_id *id);
	void (LINUX_ABI *disconnect) (struct usb_interface *intf);
};

typedef void (LINUX_ABI *usb_complete_t)(struct urb *);

static int usb_anchor_check_wakeup(struct usb_anchor *anchor){
	return anchor->suspend_wakeups == 0 && list_empty(&anchor->urb_list);
}

/* convert_usb_pipe_to_ep: convert a kernel pipe into an usb endpoint for using it with libusb */
static unsigned char convert_usb_pipe_to_ep(unsigned int pipe){
	unsigned char ep_num = 0;
	unsigned char epaddr = 0;

	ep_num = usb_pipeendpoint(pipe);
	epaddr = ep_num | (usb_pipein(pipe) ? USB_DIR_IN : 0);
	return epaddr;
}

static inline struct usb_host_endpoint *usb_pipe_endpoint(struct usb_device *dev, unsigned int pipe){
	struct usb_host_endpoint **eps = NULL;
	eps = usb_pipein(pipe) ? dev->ep_in : dev->ep_out;
	return eps[usb_pipeendpoint(pipe)];
}

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

/* model the primitives for locking during usb I/O phases */
static inline void usb_lock(void) {
	pthread_mutex_lock(&director.usb_io_lock);
	director.usb_io_depth++;
}

static inline void usb_unlock(void) {
	director.usb_io_depth--;
	if (director.usb_io_depth == 0){
		pthread_cond_broadcast(&director.usb_io_idle);
	}
	pthread_mutex_unlock(&director.usb_io_lock);
}

/* libusb_urb_callback: main libusb callback used as a thin layer between the userspace and the kernelspace */
static void libusb_urb_callback(struct libusb_transfer *transfer){
	struct urb *urb = NULL;
	usb_complete_t usb_completion_cb = NULL;

	/* get the urb pointer from the 'transfer' data */
	urb = (struct urb *)transfer->user_data;

	/* convert the libusb error into the equivalent Linux kernel error */
	switch (transfer->status) {
		case LIBUSB_TRANSFER_COMPLETED:
			urb->status = 0;
		break;
		case LIBUSB_TRANSFER_ERROR:
			urb->status = -EIO;
		break;
		case LIBUSB_TRANSFER_TIMED_OUT:
			urb->status = -ETIMEDOUT;
		break;
		case LIBUSB_TRANSFER_CANCELLED:
			urb->status = -ENOENT;
		break;
		case LIBUSB_TRANSFER_STALL:
			urb->status = -EPIPE;
		break;
		case LIBUSB_TRANSFER_NO_DEVICE:
			urb->status = -ENODEV;
		break;
		case LIBUSB_TRANSFER_OVERFLOW:
			urb->status = -EOVERFLOW;
		break;
		default:
			urb->status = -EIO;
		break;
	}

	urb->actual_length = transfer->actual_length;

	urb->hcpriv = NULL;
	atomic_fetch_sub_explicit(&urb->use_count, 1, memory_order_release);

	/* Linux removes a completed URB from its anchor before invoking the driver. */
	usb_unanchor_urb(urb);

	usb_completion_cb = (usb_complete_t)(urb->complete);
	if (usb_completion_cb != NULL){
		usb_completion_cb(urb);
	}

	libusb_free_transfer(transfer);
	/* Drop the reference acquired by usb_submit_urb(). */
	usb_free_urb(urb);
}

/* libusb_thread: handle async events */
static void *libusb_thread(void *unused){
	setup_percpu();
	while (!atomic_load_explicit(&director.libusb_thread_stop, memory_order_acquire)){
		libusb_handle_events(NULL);
	}
	return NULL;
}
