/* Made by Edoardo Mantovani, 2026
 * this header contains usb-related stuff */

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
#define usb_sndctrlpipe(dev, endpoint)	\
	((PIPE_CONTROL << 30) | __create_pipe(dev, endpoint))
#define usb_rcvctrlpipe(dev, endpoint)	\
	((PIPE_CONTROL << 30) | __create_pipe(dev, endpoint) | USB_DIR_IN)
#define usb_sndisocpipe(dev, endpoint)	\
	((PIPE_ISOCHRONOUS << 30) | __create_pipe(dev, endpoint))
#define usb_rcvisocpipe(dev, endpoint)	\
	((PIPE_ISOCHRONOUS << 30) | __create_pipe(dev, endpoint) | USB_DIR_IN)
#define usb_sndbulkpipe(dev, endpoint)	\
	((PIPE_BULK << 30) | __create_pipe(dev, endpoint))
#define usb_rcvbulkpipe(dev, endpoint)	\
	((PIPE_BULK << 30) | __create_pipe(dev, endpoint) | USB_DIR_IN)
#define usb_sndintpipe(dev, endpoint)	\
	((PIPE_INTERRUPT << 30) | __create_pipe(dev, endpoint))
#define usb_rcvintpipe(dev, endpoint)	\
	((PIPE_INTERRUPT << 30) | __create_pipe(dev, endpoint) | USB_DIR_IN)

#define interface_to_usbdev(intf) ((struct usb_device *)director.usb_device_ptr)

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
	USB_INTERFACE_UNBOUND,
	USB_INTERFACE_BINDING,
	USB_INTERFACE_BOUND,
	USB_INTERFACE_UNBINDING,
};

enum usb_wireless_status {
	USB_WIRELESS_STATUS_NA,
	USB_WIRELESS_STATUS_DISCONNECTED,
	USB_WIRELESS_STATUS_CONNECTED,
};

struct usb_device;

#define USB_DEVICE_ID_MATCH_VENDOR		0x0001
#define USB_DEVICE_ID_MATCH_PRODUCT		0x0002
#define USB_DEVICE_ID_MATCH_DEV_LO		0x0004
#define USB_DEVICE_ID_MATCH_DEV_HI		0x0008
#define USB_DEVICE_ID_MATCH_DEV_CLASS		0x0010
#define USB_DEVICE_ID_MATCH_DEV_SUBCLASS	0x0020
#define USB_DEVICE_ID_MATCH_DEV_PROTOCOL	0x0040
#define USB_DEVICE_ID_MATCH_INT_CLASS		0x0080
#define USB_DEVICE_ID_MATCH_INT_SUBCLASS	0x0100
#define USB_DEVICE_ID_MATCH_INT_PROTOCOL	0x0200
#define USB_DEVICE_ID_MATCH_INT_NUMBER		0x0400

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

/*
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
	struct list_head urb_list;
	void *hcpriv;
	struct ep_device *ep_dev;	/* For sysfs info */

	unsigned char *extra;   /* Extra descriptors */
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

/*
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

	/* variable-length array of alternate settings for this interface, stored in no particular order */
	struct usb_host_interface altsetting[];
};

struct usb_interface {
	/* array of alternate settings for this interface, stored in no particular order */
	struct usb_host_interface *altsetting;

	struct usb_host_interface *cur_altsetting;	/* the currently active alternate setting */
	unsigned num_altsetting;	/* number of alternate settings */

	/* If there is an interface association descriptor then it will list the associated interfaces */
	struct usb_interface_assoc_descriptor *intf_assoc;

	int minor;			/* minor number this interface is bound to */
	enum usb_interface_condition condition;	/* state of binding */
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

struct usb_submit {
	struct urb *urb;
	unsigned char *owned_buffer;
	bool isoc;
	bool control;
	bool pooled_transfer;
};

/*
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

	/* List of any Interface Association Descriptors in this configuration. */
	struct usb_interface_assoc_descriptor *intf_assoc[USB_MAXIADS];

	/* the interfaces associated with this configuration, stored in no particular order */
	struct usb_interface *interface[USB_MAXINTERFACES];

	/* Interface information available even when this is not the active configuration */
	struct usb_interface_cache *intf_cache[USB_MAXINTERFACES];

	unsigned char *extra;   /* Extra descriptors */
	int extralen;
};

/* used in 'struct usb_driver', never changes across kernel versions, the last field 'for_devices' is almost useless and deprecated in the latest kernel v7.0 */
struct usbdrv_wrap {
	struct device_driver driver;
	int for_devices;
};

typedef void (LINUX_ABI *usb_complete_t)(struct urb *);

static int usb_anchor_check_wakeup(struct usb_anchor *anchor){
	int ret = 0;

	if(anchor->suspend_wakeups == 0 && list_empty(&anchor->urb_list)){
		ret = true;
	}
	return ret;
}

static bool usb_id_terminator(const struct usb_device_id *id){
	int ret = 0;

	if(id == NULL){
		return true;
	}
	ret = id->match_flags == 0 && id->idVendor == 0 &&
	      id->idProduct == 0 && id->bcdDevice_lo == 0 &&
	      id->bcdDevice_hi == 0 && id->bDeviceClass == 0 &&
	      id->bDeviceSubClass == 0 && id->bDeviceProtocol == 0 &&
	      id->bInterfaceClass == 0 && id->bInterfaceSubClass == 0 &&
	      id->bInterfaceProtocol == 0 && id->bInterfaceNumber == 0 &&
	      id->driver_info == 0;
	return ret;
}

static bool __usb_match_id(const struct usb_device_descriptor *descriptor, const struct usb_interface_descriptor *interface_descriptor, const struct usb_device_id *id){
	int ret = 0;
	u16 bcd = 0;
	u16 vendor = 0;
	u16 product = 0;

	if(descriptor == NULL || id == NULL || usb_id_terminator(id)){
		return false;
	}
	bcd = le16_to_cpu(descriptor->bcdDevice);
	vendor = le16_to_cpu(descriptor->idVendor);
	product = le16_to_cpu(descriptor->idProduct);
	if((id->match_flags & USB_DEVICE_ID_MATCH_VENDOR) && id->idVendor != vendor){
		return false;
	}
	if((id->match_flags & USB_DEVICE_ID_MATCH_PRODUCT) && id->idProduct != product){
		return false;
	}
	if((id->match_flags & USB_DEVICE_ID_MATCH_DEV_LO) && bcd < id->bcdDevice_lo){
		return false;
	}
	if((id->match_flags & USB_DEVICE_ID_MATCH_DEV_HI) && bcd > id->bcdDevice_hi){
		return false;
	}
	if((id->match_flags & USB_DEVICE_ID_MATCH_DEV_CLASS) && id->bDeviceClass != descriptor->bDeviceClass){
		return false;
	}
	if((id->match_flags & USB_DEVICE_ID_MATCH_DEV_SUBCLASS) && id->bDeviceSubClass != descriptor->bDeviceSubClass){
		return false;
	}
	if((id->match_flags & USB_DEVICE_ID_MATCH_DEV_PROTOCOL) && id->bDeviceProtocol != descriptor->bDeviceProtocol){
		return false;
	}

	if(id->match_flags & (USB_DEVICE_ID_MATCH_INT_CLASS | USB_DEVICE_ID_MATCH_INT_SUBCLASS | USB_DEVICE_ID_MATCH_INT_PROTOCOL | USB_DEVICE_ID_MATCH_INT_NUMBER)){
		if(interface_descriptor == NULL){
			return false;
		}
		if((id->match_flags & USB_DEVICE_ID_MATCH_INT_CLASS) && id->bInterfaceClass != interface_descriptor->bInterfaceClass){
			return false;
		}
		if((id->match_flags & USB_DEVICE_ID_MATCH_INT_SUBCLASS) && id->bInterfaceSubClass != interface_descriptor->bInterfaceSubClass){
			return false;
		}
		if((id->match_flags & USB_DEVICE_ID_MATCH_INT_PROTOCOL) && id->bInterfaceProtocol != interface_descriptor->bInterfaceProtocol){
			return false;
		}
		if((id->match_flags & USB_DEVICE_ID_MATCH_INT_NUMBER) && id->bInterfaceNumber != interface_descriptor->bInterfaceNumber){
			return false;
		}
	}
	ret = true;
	return ret;
}

/* usb_match_device: usb kernel function used in 'usb_device_match_id', returns 0 if no match, 1 if match */
static int usb_match_device(struct usb_device *dev, const struct usb_device_id *id){
	int ret = 0;

	if ((id->match_flags & USB_DEVICE_ID_MATCH_VENDOR) && id->idVendor != le16_to_cpu(dev->descriptor.idVendor)){
		return ret;
	}
	if ((id->match_flags & USB_DEVICE_ID_MATCH_PRODUCT) && id->idProduct != le16_to_cpu(dev->descriptor.idProduct)){
		return ret;
	}
	/* No need to test id->bcdDevice_lo != 0, since 0 is never
	   greater than any unsigned number. */
	if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_LO) && (id->bcdDevice_lo > le16_to_cpu(dev->descriptor.bcdDevice))){
		return ret;
	}
	if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_HI) && (id->bcdDevice_hi < le16_to_cpu(dev->descriptor.bcdDevice))){
		return ret;
	}
	if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_CLASS) && (id->bDeviceClass != dev->descriptor.bDeviceClass)){
		return ret;
	}
	if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_SUBCLASS) && (id->bDeviceSubClass != dev->descriptor.bDeviceSubClass)){
		return ret;
	}
	if ((id->match_flags & USB_DEVICE_ID_MATCH_DEV_PROTOCOL) && (id->bDeviceProtocol != dev->descriptor.bDeviceProtocol)){
		return ret;
	}
	ret = true;
	return ret;
}

/* convert_usb_pipe_to_ep: convert a kernel pipe into an usb endpoint for using it with libusb */
static unsigned char convert_usb_pipe_to_ep(unsigned int pipe){
	unsigned char ep_num = 0;
	unsigned char epaddr = 0;

	ep_num = usb_pipeendpoint(pipe);
	epaddr = ep_num | (usb_pipein(pipe) ? USB_DIR_IN : 0);
	return epaddr;
}

static inline unsigned int __create_pipe(struct usb_device *usb_device, unsigned int endpoint){
	int ret = 0;

	ret = endpoint << 15;
	return ret;
}

/* usb_pipe_endpoint: given he usb pipe, retuns the endpoint */
static inline struct usb_host_endpoint *usb_pipe_endpoint(struct usb_device *dev, unsigned int pipe){
	struct usb_host_endpoint **eps = NULL;

	eps = usb_pipein(pipe) ? dev->ep_in : dev->ep_out;
	return eps[usb_pipeendpoint(pipe)];
}

static inline void usb_set_intfdata(struct usb_interface *intf, void *data){
	int ret = 0;
	struct device *dev = NULL;

	if(intf != NULL){
		dev = &intf->dev;
		dev->driver_data = data;
	}
}

static inline void *usb_get_intfdata(struct usb_interface *intf){
	int ret = 0;
	struct device *dev = NULL;

	if(intf == NULL){
		return NULL;
	}
	dev = &intf->dev;
	return dev->driver_data;
}

static void usb_uncache_endpoints(struct usb_device *udev, struct usb_host_interface *alt){
	int ret = 0;
	u8 endpoint = 0;

	if(udev == NULL || alt == NULL){
		return;
	}
	for(u8 i = 0; i < alt->desc.bNumEndpoints; i++){
		endpoint = usb_endpoint_num(&alt->endpoint[i].desc);
		if(usb_endpoint_dir_in(&alt->endpoint[i].desc)){
			udev->ep_in[endpoint] = NULL;
		}else{
			udev->ep_out[endpoint] = NULL;
		}
	}
}

static void usb_cache_endpoints(struct usb_device *udev, struct usb_host_interface *alt){
	int ret = 0;
	u8 endpoint = 0;

	if(udev == NULL || alt == NULL){
		return;
	}
	for(u8 i = 0; i < alt->desc.bNumEndpoints; i++){
		endpoint = usb_endpoint_num(&alt->endpoint[i].desc);
		INIT_LIST_HEAD(&alt->endpoint[i].urb_list);
		if(usb_endpoint_dir_in(&alt->endpoint[i].desc)){
			udev->ep_in[endpoint] = &alt->endpoint[i];
		}else{
			udev->ep_out[endpoint] = &alt->endpoint[i];
		}
	}
}

/* check_usb_presence: control if the usb dongle associated with the loaded kernel driver is still attached to the host system */
static int check_usb_presence(void){
	int ret = 0;
	bool have_usb_id = false;
	struct usb_device_id *id = NULL;
	libusb_device_handle *check_usb_handler = NULL;

	id = director.usb_device_id;
	if(id != NULL && id->idVendor != 0 && id->idProduct != 0){
		check_usb_handler = libusb_open_device_with_vid_pid(director.usb_ctx, id->idVendor, id->idProduct);
		if(check_usb_handler == NULL){
			ret = -ENODEV;
			return ret;
		}
		libusb_close(check_usb_handler);
		return ret;
	}

	if(have_usb_id == true){
		/* the device is disconnected from the host system! */
		ret = -ENODEV;
	}
	return ret;
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

/* urb_destroy: from the kref object, destroy the associated urb transfer by freeing its data */
static void urb_destroy(struct kref *kref){
	void *__mptr = NULL;
	struct urb *urb = NULL;

	__mptr = (void *)(kref);
	/* for avoiding problems, unroll the 'container_of' macro */
	urb = ((struct urb *)(__mptr - 0));

	if (urb->transfer_flags & URB_FREE_BUFFER){
		kfree(urb->transfer_buffer);
	}

	kfree(urb);
}

/*
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

/* usb_lock: lock the usb mutex */
static inline void usb_lock(void) {
	pthread_mutex_lock(&director.usb_io_lock);
	director.usb_io_depth++;
}

/* usb_unlock: unlock the usb mutex */
static inline void usb_unlock(void) {
	director.usb_io_depth--;
	if (director.usb_io_depth == 0){
		pthread_cond_broadcast(&director.usb_io_idle);
	}
	pthread_mutex_unlock(&director.usb_io_lock);
}

/* usb_libusb_context: get the libusb context */
static inline libusb_context *usb_libusb_context(void){
	return director.usb_ctx;
}

/* usb_transfer_pool_init: initialize the usb urb pool, uses for now the number defined in 'USB_TRANSFER_PREALLOC' */
static void usb_transfer_pool_init(void){
	struct libusb_transfer *transfer = NULL;

	pthread_mutex_init(&director.usb_transfer_pool_lock, NULL);
	memset(director.usb_transfer_pool, 0, sizeof(director.usb_transfer_pool));
	memset(director.usb_active_transfers, 0, sizeof(director.usb_active_transfers));
	director.usb_transfer_pool_count = 0;
	director.usb_active_transfer_count = 0;
	atomic_store_explicit(&director.usb_pending_transfers, 0, memory_order_relaxed);

	for (int i = 0; i < USB_TRANSFER_PREALLOC; i++){
		transfer = libusb_alloc_transfer(0);
		if (transfer == NULL){
			break;
		}
		director.usb_transfer_pool[director.usb_transfer_pool_count++] = transfer;
	}
}

static void usb_transfer_active_remove_locked(struct libusb_transfer *transfer){
	if (transfer == NULL){
		return;
	}
	for (u16 i = 0; i < director.usb_active_transfer_count; i++){
		if (director.usb_active_transfers[i] == transfer){
			director.usb_active_transfer_count--;
			director.usb_active_transfers[i] = director.usb_active_transfers[director.usb_active_transfer_count];
			director.usb_active_transfers[director.usb_active_transfer_count] = NULL;
			return;
		}
	}
}

static void usb_transfer_cancel_active(void){
	struct libusb_transfer *active[USB_ACTIVE_TRANSFER_MAX] = {0};
	u16 active_count = 0;

	pthread_mutex_lock(&director.usb_transfer_pool_lock);
	active_count = director.usb_active_transfer_count;
	for (u16 i = 0; i < active_count; i++){
		active[i] = director.usb_active_transfers[i];
	}
	pthread_mutex_unlock(&director.usb_transfer_pool_lock);

	for (u16 i = 0; i < active_count; i++){
		if (active[i] != NULL){
			libusb_cancel_transfer(active[i]);
		}
	}
}

/* usb_transfer_pool_flush: for every usb urb present in the queue, de-initialize it */
static void usb_transfer_pool_flush(void){
	struct libusb_transfer *transfer = NULL;

	pthread_mutex_lock(&director.usb_transfer_pool_lock);
	while (director.usb_transfer_pool_count > 0){
		director.usb_transfer_pool_count--;
		transfer = director.usb_transfer_pool[director.usb_transfer_pool_count];
		director.usb_transfer_pool[director.usb_transfer_pool_count] = NULL;
		libusb_free_transfer(transfer);
	}
	pthread_mutex_unlock(&director.usb_transfer_pool_lock);
}

/* usb_transfer_get: acquire and register an active transfer */
static struct libusb_transfer *usb_transfer_get(void){
	struct libusb_transfer *transfer = NULL;

	pthread_mutex_lock(&director.usb_transfer_pool_lock);
	if (director.usb_active_transfer_count == USB_ACTIVE_TRANSFER_MAX){
		pthread_mutex_unlock(&director.usb_transfer_pool_lock);
		return NULL;
	}
	if (director.usb_transfer_pool_count > 0) {
		director.usb_transfer_pool_count--;
		transfer = director.usb_transfer_pool[director.usb_transfer_pool_count];
		director.usb_transfer_pool[director.usb_transfer_pool_count] = NULL;
		director.usb_active_transfers[director.usb_active_transfer_count++] = transfer;
	}
	pthread_mutex_unlock(&director.usb_transfer_pool_lock);

	if (transfer != NULL){
		return transfer;
	}

	transfer = libusb_alloc_transfer(0);
	if (transfer == NULL){
		return NULL;
	}

	pthread_mutex_lock(&director.usb_transfer_pool_lock);
	if (director.usb_active_transfer_count == USB_ACTIVE_TRANSFER_MAX){
		pthread_mutex_unlock(&director.usb_transfer_pool_lock);
		libusb_free_transfer(transfer);
		return NULL;
	}
	director.usb_active_transfers[director.usb_active_transfer_count++] = transfer;
	pthread_mutex_unlock(&director.usb_transfer_pool_lock);
	return transfer;
}

/* usb_transfer_put: delete an urb transfer from the transfer pool */
static void usb_transfer_put(struct libusb_transfer *transfer){
	if (transfer == NULL){
		goto end;
	}

	transfer->user_data = NULL;
	transfer->callback = NULL;
	transfer->buffer = NULL;
	transfer->length = 0;
	transfer->actual_length = 0;

	pthread_mutex_lock(&director.usb_transfer_pool_lock);
	usb_transfer_active_remove_locked(transfer);
	if (director.usb_transfer_pool_count < USB_TRANSFER_POOL_SIZE){
		director.usb_transfer_pool[director.usb_transfer_pool_count++] = transfer;
		transfer = NULL;
	}
	pthread_mutex_unlock(&director.usb_transfer_pool_lock);

	if (transfer != NULL){
		libusb_free_transfer(transfer);
	}
	end:
}

/* usb_transfer_get_isoc: allocate an isochronous urb, used for HCI tasks */
static struct libusb_transfer *usb_transfer_get_isoc(int packets){
	int ret = 0;
	struct libusb_transfer *transfer = NULL;

	if(packets <= 0){
		return NULL;
	}
	transfer = libusb_alloc_transfer(packets);
	if(transfer == NULL){
		return NULL;
	}
	pthread_mutex_lock(&director.usb_transfer_pool_lock);
	if(director.usb_active_transfer_count == USB_ACTIVE_TRANSFER_MAX){
		pthread_mutex_unlock(&director.usb_transfer_pool_lock);
		libusb_free_transfer(transfer);
		return NULL;
	}
	director.usb_active_transfers[director.usb_active_transfer_count++] = transfer;
	pthread_mutex_unlock(&director.usb_transfer_pool_lock);
	return transfer;
}

static void usb_transfer_discard(struct libusb_transfer *transfer){
	int ret = 0;

	if(transfer == NULL){
		return;
	}
	pthread_mutex_lock(&director.usb_transfer_pool_lock);
	usb_transfer_active_remove_locked(transfer);
	pthread_mutex_unlock(&director.usb_transfer_pool_lock);
	libusb_free_transfer(transfer);
}

static void usb_release_submit(struct libusb_transfer *transfer, struct usb_submit *submit){
	int ret = 0;

	if(submit != NULL){
		free(submit->owned_buffer);
		submit->owned_buffer = NULL;
	}
	if(submit != NULL && submit->pooled_transfer){
		usb_transfer_put(transfer);
	}else{
		usb_transfer_discard(transfer);
	}
	if(submit != NULL){
		free(submit);
	}
}

static inline void usb_transfer_note_submit(void){
	atomic_fetch_add_explicit(&director.usb_pending_transfers, 1, memory_order_relaxed);
}

static inline void usb_transfer_note_complete(void){
	int ret = 0;

	atomic_fetch_sub_explicit(&director.usb_pending_transfers, 1, memory_order_release);
	if(director.usb_completion_initialized == true){
		pthread_mutex_lock(&director.usb_completion_lock);
		pthread_cond_broadcast(&director.usb_completion_cond);
		pthread_mutex_unlock(&director.usb_completion_lock);
	}
}

static void usb_report_rx(void *data, int len){
	int (*callback)(void *data, int len) = NULL;

	if(data == NULL || len <= 0 || !director.usb_rx_observer_initialized){
		return;
	}
	pthread_mutex_lock(&director.usb_rx_observer_lock);
	if(director.usb_rx_observer != NULL){
		callback = director.usb_rx_observer;
		director.usb_rx_observer_refs++;
	}
	pthread_mutex_unlock(&director.usb_rx_observer_lock);
	if(callback == NULL){
		return;
	}

	callback(data, len);
	pthread_mutex_lock(&director.usb_rx_observer_lock);
	director.usb_rx_observer_refs--;
	if(director.usb_rx_observer_refs == 0){
		pthread_cond_broadcast(&director.usb_rx_observer_idle);
	}
	pthread_mutex_unlock(&director.usb_rx_observer_lock);
}

static void usb_rx_observer_init(void){
	pthread_mutex_init(&director.usb_rx_observer_lock, NULL);
	pthread_cond_init(&director.usb_rx_observer_idle, NULL);
	director.usb_rx_observer = NULL;
	director.usb_rx_observer_refs = 0;
	director.usb_rx_observer_initialized = true;
}

static int usb_runtime_init(void){
	int ret = 0;

	director.usb_ctx = NULL;
	director.usb_claimed_interfaces = 0;
	ret = pthread_mutex_init(&director.usb_completion_lock, NULL);
	if(ret != 0){
		return -ret;
	}
	ret = pthread_cond_init(&director.usb_completion_cond, NULL);
	if(ret != 0){
		pthread_mutex_destroy(&director.usb_completion_lock);
		return -ret;
	}
	director.usb_completion_initialized = true;
	atomic_store_explicit(&director.usb_device_lost, false, memory_order_release);
	ret = libusb_init(&director.usb_ctx);
	if (ret != LIBUSB_SUCCESS){
		director.usb_ctx = NULL;
		pthread_cond_destroy(&director.usb_completion_cond);
		pthread_mutex_destroy(&director.usb_completion_lock);
		director.usb_completion_initialized = false;
		return -EIO;
	}

	usb_rx_observer_init();
	usb_transfer_pool_init();
	return ret;
}

static void usb_runtime_exit(void){
	struct timeval tv = {0, USB_EVENT_WAIT_USEC};
	libusb_device_handle *dev = director.usb_dev;

	director.usb_dev = NULL;
	atomic_store_explicit(&director.usb_device_lost, true, memory_order_release);

	if (director.usb_ctx == NULL){
		goto end;
	}
	usb_transfer_cancel_active();
	while (atomic_load_explicit(&director.usb_pending_transfers, memory_order_acquire) > 0){
		libusb_handle_events_timeout(usb_libusb_context(), &tv);
	}
	if (dev != NULL){
		for(u8 i = 0; i < USB_MAXINTERFACES; i++){
			if(director.usb_claimed_interfaces & BIT(i)){
				libusb_release_interface(dev, i);
			}
		}
		director.usb_claimed_interfaces = 0;
		libusb_close(dev);
	}
	usb_transfer_pool_flush();
	libusb_exit(director.usb_ctx);
	director.usb_ctx = NULL;
	pthread_mutex_destroy(&director.usb_transfer_pool_lock);
	end:
	usb_rx_observer_destroy();
	if(director.usb_completion_initialized == true){
		pthread_cond_destroy(&director.usb_completion_cond);
		pthread_mutex_destroy(&director.usb_completion_lock);
		director.usb_completion_initialized = false;
	}
}

/* libusb_urb_callback: main libusb callback used as a thin layer between the userspace and the kernelspace */
static void libusb_urb_callback(struct libusb_transfer *transfer){
	int ret = 0;
	int actual = 0;
	void *rx_data = NULL;
	struct urb *urb = NULL;
	struct usb_submit *submit = NULL;
	usb_complete_t complete = NULL;

	if(transfer == NULL){
		return;
	}
	submit = transfer->user_data;
	if(submit == NULL || submit->urb == NULL){
		usb_release_submit(transfer, submit);
		usb_transfer_note_complete();
		return;
	}
	urb = submit->urb;
	switch(transfer->status){
	case LIBUSB_TRANSFER_COMPLETED:
		urb->status = 0;
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
		atomic_store_explicit(&director.usb_device_lost, true, memory_order_release);
		break;
	case LIBUSB_TRANSFER_OVERFLOW:
		urb->status = -EOVERFLOW;
		break;
	default:
		urb->status = -EIO;
		break;
	}
	if(submit->isoc == true){
		urb->error_count = 0;
		for(int i = 0; i < transfer->num_iso_packets; i++){
			urb->iso_frame_desc[i].actual_length = transfer->iso_packet_desc[i].actual_length;
			urb->iso_frame_desc[i].status = transfer->iso_packet_desc[i].status == LIBUSB_TRANSFER_COMPLETED ? 0 : -EIO;
			actual += transfer->iso_packet_desc[i].actual_length;
			urb->error_count += urb->iso_frame_desc[i].status != 0;
		}
		urb->actual_length = actual;
		rx_data = urb->transfer_buffer;
	}else if(submit->control == true){
		urb->actual_length = transfer->actual_length;
		rx_data = libusb_control_transfer_get_data(transfer);
		if(urb->status == 0 && usb_pipein(urb->pipe) && urb->transfer_buffer != NULL){
			actual = min_t(int, transfer->actual_length, urb->transfer_buffer_length);
			memcpy(urb->transfer_buffer, rx_data, actual);
		}
	}else{
		urb->actual_length = transfer->actual_length;
		rx_data = transfer->buffer;
	}
	if(urb->status == 0 && urb->actual_length > 0 && usb_pipein(urb->pipe)){
		usb_report_rx(rx_data, urb->actual_length);
		#ifdef ENABLE_FUZZING
		fuzz_report_usb_rx(rx_data, urb->actual_length);
		#endif
	}
	if(urb->hcpriv == transfer){
		urb->hcpriv = NULL;
	}
	usb_unanchor_urb(urb);
	complete = (usb_complete_t)urb->complete;
	if(complete != NULL){
		complete(urb);
	}
	atomic_fetch_sub_explicit(&urb->use_count, 1, memory_order_release);
	usb_free_urb(urb);
	usb_release_submit(transfer, submit);
	usb_transfer_note_complete();
}

/* libusb_thread: handle async events */
static void *libusb_thread(void *unused){
	int ret = 0;
	struct timeval tv = {0, USB_EVENT_WAIT_USEC};

	setup_percpu();
	while (!atomic_load_explicit(&director.libusb_thread_stop, memory_order_acquire)){
		ret = libusb_handle_events_timeout(usb_libusb_context(), &tv);
		if (ret == LIBUSB_ERROR_NO_DEVICE){
			atomic_store_explicit(&director.usb_device_lost, true,
					      memory_order_release);
		}
	}
	cleanup_percpu(NULL);
	return NULL;
}
