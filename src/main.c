/* made by Edoardo Mantovani, 2026 */
#include <layer.h>

/* global symbol table */
struct internal_cb_table sym_table[SYM_ENTRIES];
struct module_controller director;
static unsigned int native_cb_entries;

/* include here the headers for the various supported architectures */
#include <arm64/arm64.h>
#include <x86_64/x86_64.h>

/* headers for the fuzzer API (if enabled during compilation) and the standalone ELF/PE API */
#include <fuzzer.h>
#include <standalone.h>

/* structures and definitions used for representing the command table used by the main program. */
enum {
	DRV_CMD_EXIT,
	DRV_CMD_RESET,
	DRV_CMD_LOAD,
	DRV_CMD_UNLOAD,
	DRV_CMD_START,
	DRV_CMD_STOP,
	DRV_CMD_RUN,
	DRV_CMD_SET,
	DRV_CMD_GET,
	DRV_CMD_OPS,
};

/* protocol types FOR 'exsul_protocol_hdr' */
enum {
        STANDALONE_TCC_SCRIPTING = 1,
        STANDALONE_CONSOLE_MGMT  = 2,
};

/* command entry, those are the commands the user can use from Exsul's shell */
struct cmd_table_entry {
        u8 cmd_name[18];
        u8 cmd_args;
        int (*cmd_ptr)(struct module_controller *, char **, int);
};

/* protocol format for the standalone ELF ipc */
struct exsul_protocol_hdr {
        u8 type;
        u8 len_hi;
        u16 len;
};

void track_skb_alloc(struct sk_buff *skb){
	struct tracked_skb_alloc *node = NULL;

	if(skb == NULL){
		return;
	}
	node = malloc(sizeof(*node));
	if(node == NULL){
		return;
	}
	node->skb = skb;
	pthread_mutex_lock(&director.skb_queue_lock);
	node->next = tracked_skb_allocs;
	tracked_skb_allocs = node;
	pthread_mutex_unlock(&director.skb_queue_lock);
}

void untrack_skb_alloc(struct sk_buff *skb){
	struct tracked_skb_alloc **pp = NULL;
	struct tracked_skb_alloc *node = NULL;

	if(skb == NULL){
		return;
	}
	pthread_mutex_lock(&director.skb_queue_lock);
	pp = &tracked_skb_allocs;
	while(*pp != NULL){
		if((*pp)->skb == skb){
			node = *pp;
			*pp = node->next;
			break;
		}
		pp = &(*pp)->next;
	}
	pthread_mutex_unlock(&director.skb_queue_lock);
	free(node);
}

void free_tracked_skb_allocs(void){
	struct tracked_skb_alloc *node = NULL;
	struct tracked_skb_alloc *next = NULL;
	struct sk_buff *skb = NULL;

	pthread_mutex_lock(&director.skb_queue_lock);
	node = tracked_skb_allocs;
	tracked_skb_allocs = NULL;
	pthread_mutex_unlock(&director.skb_queue_lock);

	while(node != NULL){
		next = node->next;
		skb = node->skb;
		if(skb != NULL){
			if(skb->head != NULL && !skb->head_frag){
				free(skb->head);
			}
			free(skb);
		}
		free(node);
		node = next;
	}
}

void track_urb_alloc(struct urb *urb){
	struct tracked_urb_alloc *node = NULL;

	if(urb == NULL){
		return;
	}
	node = malloc(sizeof(*node));
	if(node == NULL){
		return;
	}
	node->urb = urb;
	pthread_mutex_lock(&director.skb_queue_lock);
	node->next = tracked_urb_allocs;
	tracked_urb_allocs = node;
	pthread_mutex_unlock(&director.skb_queue_lock);
}

void untrack_urb_alloc(struct urb *urb){
	struct tracked_urb_alloc **pp = NULL;
	struct tracked_urb_alloc *node = NULL;

	if(urb == NULL){
		return;
	}
	pthread_mutex_lock(&director.skb_queue_lock);
	pp = &tracked_urb_allocs;
	while(*pp != NULL){
		if((*pp)->urb == urb){
			node = *pp;
			*pp = node->next;
			break;
		}
		pp = &(*pp)->next;
	}
	pthread_mutex_unlock(&director.skb_queue_lock);
	free(node);
}

void free_tracked_urb_allocs(void){
	struct tracked_urb_alloc *node = NULL;
	struct tracked_urb_alloc *next = NULL;
	struct urb *urb = NULL;

	pthread_mutex_lock(&director.skb_queue_lock);
	node = tracked_urb_allocs;
	tracked_urb_allocs = NULL;
	pthread_mutex_unlock(&director.skb_queue_lock);

	while(node != NULL){
		next = node->next;
		urb = node->urb;
		if(urb != NULL){
			urb_destroy(&urb->kref);
		}
		free(node);
		node = next;
	}
}

void track_kmem_alloc(void *ptr){
	if(ptr == NULL){
		return;
	}
	for(u8 i = 0; i < MAX_KMEM_ARRAY_SIZE; i++){
		if(director.kmem_array[i] == ptr){
			return;
		}
	}
	for(u8 i = 0; i < MAX_KMEM_ARRAY_SIZE; i++){
		if(director.kmem_array[i] == NULL){
			director.kmem_array[i] = ptr;
			return;
		}
	}
}

void untrack_kmem_alloc(void *ptr){
	if(ptr == NULL){
		return;
	}
	for(u8 i = 0; i < MAX_KMEM_ARRAY_SIZE; i++){
		if(director.kmem_array[i] == ptr){
			director.kmem_array[i] = NULL;
			return;
		}
	}
}

void free_tracked_kmem_allocs(void){
	for(u8 i = 0; i < MAX_KMEM_ARRAY_SIZE; i++){
		if(director.kmem_array[i] != NULL){
			free(director.kmem_array[i]);
			director.kmem_array[i] = NULL;
		}
	}
}

/* parse_module_usb_table: get the vid/pid entries from the loaded kernel object */
int parse_module_usb_table(struct usb_device_id **id){
	int ret = 0;
	int usb_table_size = 0;
	struct usb_device_id *p = NULL;

	if(director.usb_table_size != 0 && director.usb_table_ptr != NULL){
		p = director.usb_table_ptr;
		goto usb_table_parse;
	}
	usb_table_size = fast_get_symbol(USB_TABLE_SYM_NAME, &p);
	if(p == NULL || usb_table_size == 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to get the usb_table_size symbol!\n");
		#endif
		ret = -ENOMEM;
		return ret;
	}else{
		director.usb_table_ptr  = p;
		director.usb_table_size = usb_table_size;
	}

	usb_table_parse:
	*id = p;
	ret = (usb_table_size % sizeof(struct usb_device_id));
	if(ret != 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] usb device table symbol isn't aligned with the size of \'struct usb_device_id\', aborting...\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	#ifdef ENABLE_DBG_PRINT
	printf("[*] usb devices detected for the current kernel module:\n");
	#endif
	for(u8 j = 0; ; j++){
		if(p == NULL || p->idProduct == 0x00 || p->idVendor == 0x00){
			break;
		}
		#ifdef ENABLE_DBG_PRINT
		printf("[*] usb pid: 0x%08x vid: 0x%08x\n", p->idProduct, p->idVendor);
		#endif
		/* now add the pid/vid tuple into our main struct, this will be used later for the libusb device handling */
		for(u8 k = 0; k < MAX_USB_DEV_LIST; k++){
			if(director.usb_dev_list[k].is_used == false){
				director.usb_dev_list[k].usb_vid = p->idVendor;
				director.usb_dev_list[k].usb_pid = p->idProduct;
				director.usb_dev_list[k].is_used = true;
				#ifdef ADVANCED_USB_DEBUGGING
				printf("entry %d usb_vid: 0x%08x usb_pid: 0x%08x is_used: %d\n",
					k,
					director.usb_dev_list[k].usb_vid,
					director.usb_dev_list[k].usb_pid,
					director.usb_dev_list[k].is_used);
				#endif
				break;
			}
		}
		p++;
	}

	return ret;
}

#ifdef ADVANCED_USB_DEBUGGING
/* dump_usb_host_interface: dump the endpoints from an attached usb device */
void dump_usb_host_interface(struct usb_interface *intf){
	if(intf == NULL){
		return;
	}
	if(intf->altsetting == NULL){
		return;
	}
	if(intf->altsetting->endpoint == NULL){
		return;
	}

	for(u8 k = 0; k < intf->altsetting->desc.bNumEndpoints; k++){
		printf("[*] endpoint %d length %d\n", k, intf->altsetting->endpoint[k].desc.bLength);
		printf("[*] endpoint %d desc: %d\n", k, intf->altsetting->endpoint[k].desc.bDescriptorType);
		printf("[*] endpoint %d ep_addr: %d\n", k, intf->altsetting->endpoint[k].desc.bEndpointAddress);
		printf("[*] endpoint %d attributes: %d\n", k, intf->altsetting->endpoint[k].desc.bmAttributes);
		printf("[*] endpoint %d max pkt size: %d\n", k, intf->altsetting->endpoint[k].desc.wMaxPacketSize);
		printf("[*] endpoint %d interval: %d\n", k, intf->altsetting->endpoint[k].desc.bInterval);
		printf("[*] endpoint %d refresh: %d\n", k, intf->altsetting->endpoint[k].desc.bRefresh);
		printf("[*] endpoint %d sync address: %d\n", k, intf->altsetting->endpoint[k].desc.bSynchAddress);
	}
}
#endif

/* parse_usb_and_alloc_intf: allocate the 'usb_interface' struct which will be passed to the 'probe' callback of the driver */
int parse_usb_and_alloc_intf(libusb_device_handle *handler, struct usb_interface **in_intf) {
        int ret = 0;
        int int_number = 0;
        u32 struct_usb_interface_size = 0;
        u32 struct_usb_device_size = 0;
        u32 struct_device_size = 0;
        u8  num_altsetting  = 0;
        u8  num_ep = 0;
        libusb_device *libusb_dev = NULL;
	struct libusb_config_descriptor *cfg = NULL;
	const struct libusb_interface *libusb_intf = NULL;
	struct usb_interface *intf_ptr = NULL;
	struct usb_device *udev_ptr = NULL;
	const struct libusb_interface_descriptor *alt = NULL;
	const struct libusb_endpoint_descriptor *ep = NULL;
	struct libusb_device_descriptor dev_desc = {};

	if (handler == NULL || in_intf == NULL) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] invalid usb handler or invalid usb interface!\n");
		#endif
                ret = -EINVAL;
		return ret;
        }

        *in_intf = NULL;
        libusb_dev = libusb_get_device(handler);
        if (libusb_dev == NULL) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] failed to acquire libusb_device!\n");
		#endif
                ret = -EINVAL;
		return ret;
        }

	ret = libusb_get_active_config_descriptor(libusb_dev, &cfg);
        if (ret < 0) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] failed to get libusb_config_descriptor!\n");
		#endif
                ret = -EINVAL;
		return ret;
        }

	if (cfg->bNumInterfaces == 0) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] device has no interfaces!\n");
		#endif
                ret = -ENODEV;
                goto fail_free_cfg;
        }

        struct_usb_interface_size = director.struct_sizes.usb_interface;
        struct_usb_device_size    = director.struct_sizes.usb_device;
        struct_device_size        = director.struct_sizes.device;

	if (struct_usb_interface_size == 0) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] failed to retrieve size for struct usb_interface!\n");
		#endif
                ret = -EINVAL;
                goto fail_free_cfg;
        }
        if (struct_usb_device_size == 0) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] failed to retrieve size for struct usb_device!\n");
		#endif
                ret = -EINVAL;
                goto fail_free_cfg;
        }
        if (struct_device_size == 0) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] failed to retrieve size for struct device!\n");
		#endif
                ret = -EINVAL;
                goto fail_free_cfg;
        }

	intf_ptr = (struct usb_interface *)calloc(1, struct_usb_interface_size);
        if (intf_ptr == NULL) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] failed to allocate usb_interface!\n");
		#endif
                ret = -ENOMEM;
                goto fail_free_cfg;
        }

	libusb_intf = &cfg->interface[0];
	num_altsetting = (u8)libusb_intf->num_altsetting;

	#ifdef ADVANCED_USB_DEBUGGING
        printf("[*] interface 0 has %d altsetting(s)\n", num_altsetting);
	#endif

        if (num_altsetting == 0) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] interface 0 has no altsettings!\n");
		#endif
                ret = -ENODEV;
                goto fail_free_intf;
        }

        intf_ptr->altsetting = (struct usb_host_interface *)calloc(num_altsetting, sizeof(struct usb_host_interface));
        if (intf_ptr->altsetting == NULL) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] failed to allocate usb_host_interface array!\n");
		#endif
                ret = -ENOMEM;
                goto fail_free_intf;
        }

	intf_ptr->num_altsetting = num_altsetting;
        for (u8 k = 0; k < num_altsetting; k++) {
                alt = &libusb_intf->altsetting[k];

                num_ep = alt->bNumEndpoints;

		#ifdef ADVANCED_USB_DEBUGGING
                printf("[*] altsetting %u: bInterfaceNumber=%u bNumEndpoints=%u\n", k, alt->bInterfaceNumber, num_ep);
		#endif
                intf_ptr->altsetting[k].desc.bLength              = alt->bLength;
                intf_ptr->altsetting[k].desc.bDescriptorType      = alt->bDescriptorType;
                intf_ptr->altsetting[k].desc.bInterfaceNumber     = alt->bInterfaceNumber;
                intf_ptr->altsetting[k].desc.bAlternateSetting    = alt->bAlternateSetting;
                intf_ptr->altsetting[k].desc.bNumEndpoints        = alt->bNumEndpoints;
                intf_ptr->altsetting[k].desc.bInterfaceClass      = alt->bInterfaceClass;
                intf_ptr->altsetting[k].desc.bInterfaceSubClass   = alt->bInterfaceSubClass;
                intf_ptr->altsetting[k].desc.bInterfaceProtocol   = alt->bInterfaceProtocol;
                intf_ptr->altsetting[k].desc.iInterface           = alt->iInterface;

                if (k == 0) {
                        int_number = alt->bInterfaceNumber;
                }

                if (num_ep == 0) {
                        intf_ptr->altsetting[k].endpoint = NULL;
                        continue;
                }

                intf_ptr->altsetting[k].endpoint = (struct usb_host_endpoint *)calloc(num_ep, sizeof(struct usb_host_endpoint));
                if (intf_ptr->altsetting[k].endpoint == NULL) {
                        ret = -ENOMEM;
                        for (u8 m = 0; m < k; m++) {
                                free(intf_ptr->altsetting[m].endpoint);
                                intf_ptr->altsetting[m].endpoint = NULL;
			}
                        goto fail_free_altsetting;
                }

		for (u8 z = 0; z < num_ep; z++) {
			ep = &alt->endpoint[z];
			intf_ptr->altsetting[k].endpoint[z].desc.bLength          = ep->bLength;
			intf_ptr->altsetting[k].endpoint[z].desc.bDescriptorType  = ep->bDescriptorType;
			intf_ptr->altsetting[k].endpoint[z].desc.bEndpointAddress = ep->bEndpointAddress;
			intf_ptr->altsetting[k].endpoint[z].desc.bmAttributes     = ep->bmAttributes;
			intf_ptr->altsetting[k].endpoint[z].desc.wMaxPacketSize   = ep->wMaxPacketSize;
			intf_ptr->altsetting[k].endpoint[z].desc.bInterval        = ep->bInterval;
			intf_ptr->altsetting[k].endpoint[z].desc.bRefresh         = ep->bRefresh;
			intf_ptr->altsetting[k].endpoint[z].desc.bSynchAddress    = ep->bSynchAddress;
		}
	}

	intf_ptr->cur_altsetting = &intf_ptr->altsetting[0];
        udev_ptr = (struct usb_device *)calloc(1, struct_usb_device_size);
        if (udev_ptr == NULL) {
		#ifdef ENABLE_DBG_PRINT
                printf("[!] failed to allocate usb_device!\n");
		#endif
                ret = -ENOMEM;
                goto fail_free_endpoints;
        }
	/* allocate also 'usb_bus' */
	if(director.struct_sizes.usb_bus <= 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] usb_bus struct has size 0!\n");
		#endif
		/* ignore but it will lead to a crash if certain drivers are loaded */
		ret = -EINVAL;
	}
	udev_ptr->bus = calloc(1, director.struct_sizes.usb_bus);
	if(udev_ptr->bus == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to allocate usb_bus!\n");
		#endif
		ret = -ENOMEM;
	}
	/* get the usb device descriptor */
	ret = libusb_get_device_descriptor(libusb_dev, &dev_desc);
	if(ret == 0) {
		udev_ptr->descriptor.bLength            = dev_desc.bLength;
		udev_ptr->descriptor.bDescriptorType    = dev_desc.bDescriptorType;
		udev_ptr->descriptor.bcdUSB             = dev_desc.bcdUSB;
		udev_ptr->descriptor.bDeviceClass       = dev_desc.bDeviceClass;
		udev_ptr->descriptor.bDeviceSubClass    = dev_desc.bDeviceSubClass;
		udev_ptr->descriptor.bDeviceProtocol    = dev_desc.bDeviceProtocol;
		udev_ptr->descriptor.bMaxPacketSize0    = dev_desc.bMaxPacketSize0;
		udev_ptr->descriptor.idVendor           = dev_desc.idVendor;
		udev_ptr->descriptor.idProduct          = dev_desc.idProduct;
		udev_ptr->descriptor.bcdDevice          = dev_desc.bcdDevice;
		udev_ptr->descriptor.iManufacturer      = dev_desc.iManufacturer;
		udev_ptr->descriptor.iProduct           = dev_desc.iProduct;
		udev_ptr->descriptor.iSerialNumber      = dev_desc.iSerialNumber;
		udev_ptr->descriptor.bNumConfigurations = dev_desc.bNumConfigurations;
	}else{
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to get libusb device descriptor, defaulting bNumConfigurations=1\n");
		#endif
                udev_ptr->descriptor.bNumConfigurations = 1;
	}

	switch (libusb_get_device_speed(libusb_dev)) {
		case LIBUSB_SPEED_SUPER:
		case LIBUSB_SPEED_SUPER_PLUS:
			udev_ptr->speed = USB_SPEED_SUPER;
		break;
		case LIBUSB_SPEED_HIGH:
			udev_ptr->speed = USB_SPEED_HIGH;
		break;
		case LIBUSB_SPEED_FULL:
			udev_ptr->speed = USB_SPEED_FULL;
		break;
		case LIBUSB_SPEED_LOW:
			udev_ptr->speed = USB_SPEED_LOW;
		break;
		default:
			udev_ptr->speed = USB_SPEED_UNKNOWN;
		break;
        }

	intf_ptr->dev.parent    = &udev_ptr->dev;
	director.usb_device_ptr = udev_ptr;

	#ifdef ADVANCED_USB_DEBUGGING
        dump_usb_host_interface(intf_ptr);
	#endif

        *in_intf = intf_ptr;
        libusb_free_config_descriptor(cfg);
        return int_number;

	fail_free_endpoints:
        for (u8 m = 0; m < num_altsetting; m++) {
                free(intf_ptr->altsetting[m].endpoint);
                intf_ptr->altsetting[m].endpoint = NULL;
        }
	fail_free_altsetting:
        free(intf_ptr->altsetting);
        intf_ptr->altsetting = NULL;
	fail_free_intf:
        free(intf_ptr);
	fail_free_cfg:
        libusb_free_config_descriptor(cfg);
        return ret;
}

static void free_usb_interface_alloc(struct usb_interface *intf){
	if(intf == NULL){
		return;
	}
	if(intf->altsetting != NULL){
		for(u8 i = 0; i < intf->num_altsetting; i++){
			free(intf->altsetting[i].endpoint);
			intf->altsetting[i].endpoint = NULL;
		}
		free(intf->altsetting);
		intf->altsetting = NULL;
	}
	if(director.usb_interface == intf){
		director.usb_interface = NULL;
	}
	free(intf);
}

/* usb_disk_modeswitch: under Windows and other OSes, 'usb_modeswitch' isn't always available,
 * in this case, invoke the hardcoded minimal implementation of modeswitch */
static int usb_disk_modeswitch(struct module_controller *controller){
	int ret = 0;
	int transferred = 0;
	const struct usb_device_id *id = NULL;
	libusb_device *dev = NULL;
	struct libusb_config_descriptor *cfg = NULL;
	const struct libusb_interface_descriptor *alt = NULL;
	const struct libusb_endpoint_descriptor *ep = NULL;
	u8 intf_num = 0;
	u8 bulk_in = 0;
	u8 bulk_out = 0;
	u8 csw[13] = {0};
	const u8 eject_cmd[] = {
		0x55, 0x53, 0x42, 0x43, 0x12, 0x34, 0x56, 0x78,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x1b,
		0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	const struct usb_device_id usb_disk_ids[] = {
		/* Realtek under Windows */
		{
			.idVendor = 0x0bda,
			.idProduct = 0x1a2b
		},
		/* aic8800, OS independent */
		{
			.idVendor = 0xa69c,
			.idProduct = 0x5721,
		},
		{
			.idVendor = 0xa69c,
			.idProduct = 0x5725,
		},
		{},
	};

	for(id = usb_disk_ids; id->idVendor && id->idProduct; id++){
		controller->usb_dev = libusb_open_device_with_vid_pid(usb_libusb_context(), id->idVendor, id->idProduct);
		if(controller->usb_dev != NULL){
			break;
		}
	}
	if(controller->usb_dev == NULL){
		ret = -ENODEV;
		return ret;
	}

	dev = libusb_get_device(controller->usb_dev);
	if(dev == NULL){
		ret = -ENODEV;
		goto out_close;
	}

	ret = libusb_get_active_config_descriptor(dev, &cfg);
	if(ret < 0){
		ret = libusb_get_config_descriptor(dev, 0, &cfg);
	}
	if(ret < 0 || cfg == NULL){
		ret = -EIO;
		goto out_close;
	}

	for(u8 i = 0; i < cfg->bNumInterfaces && bulk_out == 0; i++){
		for(int j = 0; j < cfg->interface[i].num_altsetting && bulk_out == 0; j++){
			alt = &cfg->interface[i].altsetting[j];
			if(alt->bInterfaceClass != LIBUSB_CLASS_MASS_STORAGE){
				continue;
			}
			intf_num = alt->bInterfaceNumber;
			for(u8 k = 0; k < alt->bNumEndpoints; k++){
				ep = &alt->endpoint[k];
				if((ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) != LIBUSB_TRANSFER_TYPE_BULK){
					continue;
				}
				if((ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN){
					bulk_in = ep->bEndpointAddress;
				}else{
					bulk_out = ep->bEndpointAddress;
				}
			}
		}
	}

	if(bulk_out == 0){
		ret = -ENODEV;
		goto out_free_cfg;
	}

	#ifndef _WIN64
	libusb_set_auto_detach_kernel_driver(controller->usb_dev, 1);
	if(libusb_kernel_driver_active(controller->usb_dev, intf_num) == 1){
		libusb_detach_kernel_driver(controller->usb_dev, intf_num);
	}
	#endif

	ret = libusb_claim_interface(controller->usb_dev, intf_num);
	if(ret == 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[*] detected DISK device on interface %u (bulk out: 0x%02x, bulk in: 0x%02x), sending the eject command!\n", intf_num, bulk_out, bulk_in);
		#endif
		ret = libusb_bulk_transfer(controller->usb_dev, bulk_out, (u8 *)eject_cmd, sizeof(eject_cmd), &transferred, 5000);
		if(ret == 0 && transferred == sizeof(eject_cmd) && bulk_in != 0){
			int csw_transferred = 0;
			libusb_bulk_transfer(controller->usb_dev, bulk_in, csw, sizeof(csw), &csw_transferred, 1000);
		}
		libusb_release_interface(controller->usb_dev, intf_num);
	}

	out_free_cfg:
	if(cfg != NULL){
		libusb_free_config_descriptor(cfg);
	}
	out_close:
	libusb_close(controller->usb_dev);
	controller->usb_dev = NULL;

	if(ret < 0 || transferred != sizeof(eject_cmd)){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to eject the DISK: ret=%d transferred=%d/%d\n", ret, transferred, (int)sizeof(eject_cmd));
		#endif
		ret = -EIO;
		return ret;
	}

	#ifdef _WIN64
	Sleep(3000);
	#else
	usleep(3000);
	#endif
	return ret;
}

/* detect_usb_device: main function for detecting the attached usb dongle, requires a full 'usb_dev_list' table */
int detect_usb_device(const struct usb_device_id *id_table, struct usb_interface **intf_out, const struct usb_device_id **matched_id_out){
	int ret = 0;
	struct usb_interface *usb_intf = NULL;

	for (struct usb_device_id *p = id_table; p != NULL && p->idVendor && p->idProduct; p++) {
		director.usb_dev = libusb_open_device_with_vid_pid(usb_libusb_context(), p->idVendor, p->idProduct);
		if (director.usb_dev == NULL){
			continue;
		}

		#ifndef _WIN64
		ret = libusb_kernel_driver_active(director.usb_dev, 1);
		if(ret){
			/* if there is a kernel driver attached to the interface, reset the usb device */
			ret = libusb_reset_device(director.usb_dev);
		}
		ret = libusb_set_auto_detach_kernel_driver(director.usb_dev, 1);
		if (ret < 0) {
			libusb_close(director.usb_dev);
			director.usb_dev = NULL;
			continue;
		}else{
			ret = libusb_detach_kernel_driver(director.usb_dev, 0);
		}
		#endif

		/* alloc the usb_interface which will be passed to the '->probe' callback */
		ret = parse_usb_and_alloc_intf(director.usb_dev, &usb_intf);
		if (ret < 0) {
			libusb_close(director.usb_dev);
			director.usb_dev = NULL;
			continue;
		}

		director.usb_interface_number = ret;

		ret = libusb_claim_interface(director.usb_dev, director.usb_interface_number);
		if (ret < 0) {
			free_usb_interface_alloc(usb_intf);
			usb_intf = NULL;
			if(director.usb_device_ptr != NULL){
				free(director.usb_device_ptr);
				director.usb_device_ptr = NULL;
			}
			libusb_close(director.usb_dev);
			director.usb_dev = NULL;
			continue;
		}

		*intf_out = usb_intf;
		*matched_id_out = p;
		director.usb_device_id = p;
		atomic_store_explicit(&director.usb_device_lost, false,
				      memory_order_release);
		return ret;
	}

	/* when the original vid/pid pair isn't detected, search for any 'DISK' attached to the system and try to eject it */
	director.enable_usb_disk_search = true;
	if(director.enable_usb_disk_search == true){
		ret = usb_disk_modeswitch(&director);
		if(ret == 0){
			ret = detect_usb_device(id_table, intf_out, matched_id_out);
			return ret;
		}
	}
	#ifdef ENABLE_DBG_PRINT
	printf("[!] usb device not attached to the host!\n");
	#endif
	ret = -ENODEV;
	return ret;
}

/* invoke_usb_probe: call the driver's probe callback */
int invoke_usb_probe(struct usb_driver *new_driver, struct usb_device_id *id, struct usb_interface *intf){
	int ret = 0;
	int (LINUX_ABI *probe)(struct usb_interface *intf, const struct usb_device_id *id) = NULL;
	probe = new_driver->probe;

	/* in normal conditions here a mutex would have been implemented, but in this case there is no necessity */
	ret = probe(intf, id);

	return ret;
}

/* dwarf_get_member_offset: read the DW_AT_data_member_location of a DWARF member DIE. */
static Dwarf_Unsigned dwarf_get_member_offset(Dwarf_Debug dbg, Dwarf_Die member_die){
	int ret = 0;
	Dwarf_Error err = 0;
	Dwarf_Attribute attr = 0;
	Dwarf_Unsigned offset = 0;
	Dwarf_Unsigned exprlen = 0;
	char *block = NULL;

	if (dwarf_attr(member_die, DW_AT_data_member_location, &attr, &err) != DW_DLV_OK){
		return ret;
	}
	if (dwarf_formudata(attr, &offset, &err) != DW_DLV_OK) {
		if (dwarf_formexprloc(attr, &exprlen, &block, &err) == DW_DLV_OK && exprlen > 0 && block) {
			if (block[0] == DW_OP_plus_uconst || block[0] == DW_OP_constu){
				offset = read_uleb128(block + 1, (int)exprlen - 1);
			}
		}
	}
	dwarf_dealloc(dbg, attr, DW_DLA_ATTR);
	return offset;
}

static int dwarf_search_field_recursive(unsigned char *field_input_name, Dwarf_Debug dbg, Dwarf_Die struct_die, unsigned int base_offset){
	int ret = 0;
	int nested = 0;
	int sibling = 0;
	unsigned int member_off = 0;
	Dwarf_Error err = 0;
	Dwarf_Die cur = 0;
	Dwarf_Die sib = 0;
	Dwarf_Die child = 0;
	Dwarf_Half tag = 0;
	char *field_name = NULL;

	ret = dwarf_child(struct_die, &child, &err);
	if (ret != DW_DLV_OK){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to get the dwarf child!\n");
		#endif
		ret = -ENOENT;
		return ret;
	}
	cur = child;

	for (;;) {
		tag = 0;
		ret = dwarf_tag(cur, &tag, &err);
		if (ret == DW_DLV_OK && tag == DW_TAG_member) {
			member_off = (unsigned int)dwarf_get_member_offset(dbg, cur);

			field_name = NULL;
			ret = dwarf_diename(cur, &field_name, &err);

			if (ret == DW_DLV_OK && field_name && strcmp(field_name, (char *)field_input_name) == 0) {
				/* Found the named field */
				dwarf_dealloc(dbg, cur, DW_DLA_DIE);
				return (int)(base_offset + member_off);
			}

			if (ret != DW_DLV_OK || !field_name) {
				/* Anonymous member: resolve its type and recurse */
				Dwarf_Attribute type_attr = 0;
				if (dwarf_attr(cur, DW_AT_type, &type_attr, &err) == DW_DLV_OK) {
					Dwarf_Off type_off = 0;
					if (dwarf_global_formref(type_attr, &type_off, &err) == DW_DLV_OK) {
						Dwarf_Die type_die = 0;
						if (dwarf_offdie_b(dbg, type_off, true, &type_die, &err) == DW_DLV_OK) {
							Dwarf_Half type_tag = 0;
							if (dwarf_tag(type_die, &type_tag, &err) == DW_DLV_OK &&
							    (type_tag == DW_TAG_structure_type || type_tag == DW_TAG_union_type)) {
								nested = dwarf_search_field_recursive(field_input_name, dbg, type_die, base_offset + member_off);
								if (nested >= 0) {
									dwarf_dealloc(dbg, type_die, DW_DLA_DIE);
									dwarf_dealloc(dbg, type_attr, DW_DLA_ATTR);
									dwarf_dealloc(dbg, cur, DW_DLA_DIE);
									return nested;
								}
							}
							dwarf_dealloc(dbg, type_die, DW_DLA_DIE);
						}
					}
					dwarf_dealloc(dbg, type_attr, DW_DLA_ATTR);
				}
			}
		}
		sibling = dwarf_siblingof_b(dbg, cur, true, &sib, &err);
		dwarf_dealloc(dbg, cur, DW_DLA_DIE);
		if (sibling != DW_DLV_OK){
			break;
		}
		cur = sib;
	}

	ret = -ENOENT;
	return ret;
}

/* dwarf_search_field: wrapper for 'dwarf_search_field_recursive' */
int dwarf_search_field(unsigned char *field_input_name, Dwarf_Debug dbg, Dwarf_Die struct_die){
	int ret = 0;
	ret = dwarf_search_field_recursive(field_input_name, dbg, struct_die, 0);
	return ret;
}

/* dwarf_detect_struct: find a struct in the dwarf module data */
int dwarf_detect_struct(Dwarf_Debug dbg, Dwarf_Die die, unsigned char *struct_to_search, Dwarf_Die *struct_found, unsigned short *out_size){
	int ret = 0;
	Dwarf_Error err = 0;
	Dwarf_Half tag = 0;
	Dwarf_Die child = 0;
	Dwarf_Die cur = 0;
	Dwarf_Die sib = 0;
	Dwarf_Unsigned struct_size = 0;
	char *die_name = NULL;
	Dwarf_Attribute size_attr = 0;

	ret = dwarf_tag(die, &tag, &err);
	if (ret != DW_DLV_OK){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to find dwarf tag\n");
		#endif
		ret = false;
		return ret;
	}

	if (tag == DW_TAG_structure_type) {
		/* find the 'DW_AT_NAME' used for the struct name*/
		ret = dwarf_diename(die, &die_name, &err);
		if (ret == DW_DLV_OK && die_name && strcmp(die_name, (char *)struct_to_search) == 0) {
			/* update:
			 * after my analysis on 'dwarfdump', I noticed that for finding the *total*
			 * size of the struct encoded in the dwarf format, I can extract the 'DW_AT_byte_size'
			 * field.
			 */
			ret = dwarf_attr(die, DW_AT_byte_size, &size_attr, &err);
			if(ret == DW_DLV_OK){
				ret = dwarf_formudata(size_attr, &struct_size, &err);
				dwarf_dealloc(dbg, size_attr, DW_DLA_ATTR);
				if(ret == DW_DLV_OK){
					#ifdef ENABLE_DWARF_DBG_PRINT
					printf("[*] struct %s size: %d\n", die_name, struct_size);
					#endif
					*out_size = struct_size;
				}
			}
			*struct_found = die;
			ret = true;
			return ret;
		}
	}
	ret = dwarf_child(die, &child, &err);
	if (ret != DW_DLV_OK){
		ret = false;
		return ret;
	}

	cur = child;
	for (;;) {
		ret = dwarf_detect_struct(dbg, cur, struct_to_search, struct_found, out_size);
		if (ret == true) {
			if (*struct_found != cur){
				dwarf_dealloc(dbg, cur, DW_DLA_DIE);
			}
			ret = true;
			return ret;
		}
		if (ret < 0) {
			dwarf_dealloc(dbg, cur, DW_DLA_DIE);
			return ret;
		}

		ret = dwarf_siblingof_b(dbg, cur, true, &sib, &err);
		dwarf_dealloc(dbg, cur, DW_DLA_DIE);
		if (ret != DW_DLV_OK){
			break;
		}
		cur = sib;
	}
	ret = 0;
	return ret;
}

/* dwarf_loader_parser: init the dwarf parser and start the scan on the loaded kernel object */
int dwarf_loader_parser(unsigned char *struct_to_search, unsigned char *field_to_search, unsigned short *field_offset, unsigned short *out_size){
	int ret = 0;
	int fd = -1;
	Dwarf_Debug dbg = 0;
	Dwarf_Error err = 0;

	Dwarf_Unsigned cu_header_len = 0;
	Dwarf_Off abbrev_off = 0;
	Dwarf_Unsigned next_cu = 0;
	Dwarf_Half version = 0;
	Dwarf_Half addr_size = 0;
	Dwarf_Half offset_size = 0;
	Dwarf_Half extension_size = 0;
	Dwarf_Unsigned type_offset = 0;
	Dwarf_Bool is_info = true;
	Dwarf_Sig8 type_signature = {0};
	Dwarf_Half header_cu_type = 0;

	Dwarf_Die found = 0;
	Dwarf_Die cu_die = 0;

	if(director.fd < 0){
		ret = -EINVAL;
		return ret;
	}
	fd = dup(director.fd);
	if (fd < 0){
		ret = -EINVAL;
		return ret;
	}

	ret = dwarf_init_b(fd, DW_DLC_READ, DW_GROUPNUMBER_ANY, NULL, NULL, &dbg, &err);
	if (ret != DW_DLV_OK) {
		close(fd);
		ret = -EINVAL;
		return ret;
	}

	while (dwarf_next_cu_header_d(dbg,
			is_info,
			&cu_header_len,
			&version,
			&abbrev_off,
			&addr_size,
			&offset_size,
			&extension_size,
			&type_signature,
			&type_offset,
			&next_cu,
			&header_cu_type,
			&err) == DW_DLV_OK) {
		cu_die = 0;
		ret = dwarf_siblingof_b(dbg, NULL, is_info, &cu_die, &err);
		if (ret != DW_DLV_OK){
			continue;
		}

		if (dwarf_detect_struct(dbg, cu_die, struct_to_search, &found, out_size)) {
			dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
			break;
		}
		dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
	}

	if (found == 0) {
		goto out;
	}

	ret = dwarf_search_field(field_to_search, dbg, found);
	if (ret >= 0){
		*field_offset = ret;
	}

	out:
	if (found == true){
		dwarf_dealloc(dbg, found, DW_DLA_DIE);
	}

	dwarf_finish(dbg, &err);
	close(fd);
	return ret;
}

/* parse_module_offset_table: fill every field of 'runtime_generated_structs' by doing the DWARF research lookup */
int parse_module_offset_table(void){
	int ret = 0;
	u8 counter = 0;
	struct runtime_struct *runtime_structs = NULL;
	struct struct_field *current_struct_fields = NULL;
	struct runtime_struct_layout *runtime_struct_layout = NULL;

	runtime_structs = &runtime_generated_structs;
	if(runtime_structs == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] no runtime struct data!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	for (counter = 0; counter < ARRAY_SIZE(runtime_structs->structs); counter++) {
		runtime_struct_layout = &runtime_structs->structs[counter];
		if(runtime_struct_layout->struct_field_num == 0){
			continue;
		}
		current_struct_fields = runtime_struct_layout->fields;
		for (u8 k = 0; k < runtime_struct_layout->struct_field_num; k++) {
			if(current_struct_fields[k].field_name == NULL){
				break;
			}
			ret = dwarf_loader_parser(
				runtime_struct_layout->struct_name,
				current_struct_fields[k].field_name,
				&current_struct_fields[k].field_offset,
				&runtime_struct_layout->struct_size);
			#ifdef ENABLE_DBG_PRINT
			if(ret > 0 && director.global_conf.show_struct_offset == true){
				printf("  - %s->%s offset: %d original struct size: %d\n",
					runtime_struct_layout->struct_name,
					current_struct_fields[k].field_name,
					current_struct_fields[k].field_offset,
					runtime_struct_layout->struct_size
				);
			}
			#endif
		}
	}

	dwarf_finish(director.dbg, &director.err);
	return ret;
}

#ifdef ENABLE_DBG_PRINT
/* mem_type_to_string: given the module section index, return the respective string name, used only for debugging purposes */
unsigned char *mem_type_to_string(enum mod_mem_type type){
	switch(type){
		case MOD_TEXT:
		case MOD_INIT_TEXT:
			return ".text";
		case MOD_DATA:
		case MOD_INIT_DATA:
			return ".data";
		case MOD_RODATA:
		case MOD_RO_AFTER_INIT:
		case MOD_INIT_RODATA:
			return ".rodata";
		default:
			return "unknown";
	}
}
#endif

/* module_next_tag_pair: start of the core kernel functions related to the loading of lkm */
unsigned char *module_next_tag_pair(char *string, unsigned long *secsize) {
	while (string[0]) {
		string++;
		if ((*secsize)-- <= 1) {
			return NULL;
		}
	}

	while (!string[0]) {
		string++;
		if ((*secsize)-- <= 1) {
			return NULL;
		}
	}
	return string;
}

static char *get_next_modinfo(const struct load_info *info, const char *tag, unsigned char *prev) {
	unsigned long size = 0;
	unsigned int taglen = 0;
	unsigned char *p = NULL;
	unsigned char *modinfo = NULL;
	Elf(Shdr) *infosec = &info->sechdrs[info->index.info];

	taglen = strlen(tag);
	size    = infosec->sh_size;

	modinfo = (unsigned char *)info->hdr + infosec->sh_offset;
	if (prev) {
		size -= prev - modinfo;
		modinfo = module_next_tag_pair(prev, &size);
	}

	for (p = modinfo; p; p = module_next_tag_pair(p, &size)) {
		if (strncmp(p, tag, taglen) == 0 && p[taglen] == '=') {
			return p + taglen + 1;
		}
	}
	return NULL;
}

static char *get_modinfo(const struct load_info *info, const char *tag) {
	return get_next_modinfo(info, tag, NULL);
}

static int elf_validity_cache_copy(struct load_info *info, int flags) {
	int ret = 0;
	int i   = 0;
	int err = 0;
	unsigned int num_mod_secs  = 0;
	unsigned int mod_idx	   = 0;
	unsigned int num_info_secs = 0;
	unsigned int info_idx	   = 0;
	unsigned int num_sym_secs  = 0;
	unsigned int sym_idx	   = 0;
	Elf(Shdr) *shdr = NULL;
        Elf(Shdr) *strhdr = NULL;

	if (info->hdr->e_shentsize != sizeof(Elf(Shdr))) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] invalid ELF section header size\n");
		#endif
		goto no_exec;
	}

	info->sechdrs = (void *)info->hdr + info->hdr->e_shoff;

	if (info->hdr->e_shstrndx == SHN_UNDEF || info->hdr->e_shstrndx >= info->hdr->e_shnum) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] invalid ELF section name index: %d || e_shstrndx (%d) >= e_shnum (%d)\n",
		       info->hdr->e_shstrndx, info->hdr->e_shstrndx,
		       info->hdr->e_shnum);
		#endif
		goto no_exec;
	}

	strhdr = &info->sechdrs[info->hdr->e_shstrndx];

	if (strhdr->sh_offset == 0) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] sh offset of string header points to 0!\n");
		#endif
		goto no_exec;
	}

	info->secstrings = (void *)info->hdr + strhdr->sh_offset;

	if (strhdr->sh_size == 0) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] empty section name table\n");
		#endif
		goto no_exec;
	}
	if (info->secstrings[strhdr->sh_size - 1] != '\0') {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] ELF Spec violation: section name table isn't null terminated\n");
		#endif
		goto no_exec;
	}

	if (info->sechdrs[0].sh_type != SHT_NULL || info->sechdrs[0].sh_size != 0 || info->sechdrs[0].sh_addr != 0) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] ELF Spec violation: section 0 type(%d)!=SH_NULL or non-zero len or addr\n", info->sechdrs[0].sh_type);
		#endif
		goto no_exec;
	}

	for (i = 1; i < info->hdr->e_shnum; i++) {
		shdr = &info->sechdrs[i];
		switch (shdr->sh_type) {
		case SHT_NULL:
		case SHT_NOBITS:
			/* do nothing */
			continue;
		case SHT_SYMTAB:

			if (shdr->sh_link == SHN_UNDEF || shdr->sh_link >= info->hdr->e_shnum) {
				#ifdef ENABLE_DBG_PRINT
				printf("[!] invalid ELF sh_link!=SHN_UNDEF(%d) or (sh_link(%d) >= hdr->e_shnum(%d)\n",
				       shdr->sh_link, shdr->sh_link,
				       info->hdr->e_shnum);
				#endif
				goto no_exec;
			}

			num_sym_secs++;
			sym_idx = i;
			fallthrough;
		default:
			if (strcmp(info->secstrings + shdr->sh_name, ".gnu.linkonce.this_module") == 0) {
				num_mod_secs++;
				mod_idx = i;
				#ifdef ENABLE_DBG_PRINT
				printf("[!] detected .gnu.linkonce.this_module\n");
				#endif
			} else if (strcmp(info->secstrings + shdr->sh_name, ".modinfo") == 0) {
				num_info_secs++;
				info_idx = i;
				#ifdef ENABLE_DBG_PRINT
				printf("[!] detected .modinfo\n");
				#endif
			}
			if (shdr->sh_flags & SHF_ALLOC) {
				if (shdr->sh_name >= strhdr->sh_size) {
					#ifdef ENABLE_DBG_PRINT
					printf("[!] invalid ELF section name in module (section %u type %u)\n", i, shdr->sh_type);
					#endif
					ret = -ENOEXEC;
					return ret;
				}
			}
			break;
		}
	}

	if (num_info_secs > 1) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] only one .modinfo section must exist.\n");
		#endif
		goto no_exec;
	} else if (num_info_secs == 1) {
		/* Try to find a name early so we can log errors with a module
		 * name */
		info->index.info = info_idx;
		info->name = get_modinfo(info, "name");
	}

	info->index.sym = sym_idx;
	shdr		= &info->sechdrs[sym_idx];
	info->index.str = shdr->sh_link;
	info->strtab	= (unsigned char *)info->hdr + info->sechdrs[info->index.str].sh_offset;

	if (num_mod_secs != 1) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] module %s: Only one .gnu.linkonce.this_module section must exist.\n", info->name ?: "(missing .modinfo section or name field)");
		#endif
		goto no_exec;
	}

	shdr = &info->sechdrs[mod_idx];
	if (shdr->sh_type == SHT_NOBITS) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] module %s: .gnu.linkonce.this_module section must have a size set\n", info->name ?: "(missing .modinfo section or name field)");
		#endif
		goto no_exec;
	}

	if (!(shdr->sh_flags & SHF_ALLOC)) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] module %s: .gnu.linkonce.this_module must occupy memory during process execution\n", info->name ?: "(missing .modinfo section or name field)");
		#endif
		goto no_exec;
	}

	/* disable the check of the .this_module section */

	/*
	 * if (shdr->sh_size != sizeof(struct module)) {
	 *	#ifdef ENABLE_DBG_PRINT
	 *	printf(
	 *	    "module %s: .gnu.linkonce.this_module section size must "
	 *	    "match the kernel's built struct module size at run time\n",
	 *	    info->name ?: "(missing .modinfo section or name field)");
	 *	#endif
	 * }
	 */

	info->index.mod	 = mod_idx;
	info->mod	 = (void *)info->hdr + shdr->sh_offset;
	info->index.vers = 0;
	return ret;

	no_exec:
	ret = -ENOEXEC;
	return ret;
}

static int rewrite_section_headers(struct load_info *info, int flags) {
	int ret = 0;
	Elf(Shdr) *shdr	= NULL;

	info->sechdrs[0].sh_addr = 0;
	for (int i = 1; i < info->hdr->e_shnum; i++) {
		shdr = &info->sechdrs[i];
		shdr->sh_addr = (size_t)info->hdr + shdr->sh_offset;
	}

	info->sechdrs[info->index.vers].sh_flags &= ~(unsigned long)SHF_ALLOC;
	info->sechdrs[info->index.info].sh_flags &= ~(unsigned long)SHF_ALLOC;
	return ret;
}

static int early_mod_check(struct load_info *info, int flags) {
	int ret = 0;

	ret = rewrite_section_headers(info, flags);
	return ret;
}

static inline bool within_module_mem_type(uintptr_t addr, const struct module *mod, enum mod_mem_type type) {
	uintptr_t base = 0;
	uintptr_t size = 0;

	base = (uintptr_t)mod->mem[type].base;
	size = mod->mem[type].size;
	return addr - base < size;
}

int module_enforce_rwx_sections(Elf(Ehdr) * hdr, Elf(Shdr) * sechdrs, char *secstrings, struct module *mod) {
	int ret = 0;
	const unsigned long shf_wx = SHF_WRITE | SHF_EXECINSTR;

	for (int i = 0; i < hdr->e_shnum; i++) {
		if ((sechdrs[i].sh_flags & shf_wx) == shf_wx) {
			#ifdef ENABLE_DBG_PRINT
			printf("[*] %s: section %s (index %d) has invalid WRITE|EXEC flags\n",
				mod->name, secstrings + sechdrs[i].sh_name, i);
			#endif
			ret = -ENOEXEC;
			return ret;
		}
	}
	return ret;
}

static unsigned int find_sec(const struct load_info *info, const char *name) {
	Elf(Shdr) *shdr = NULL;
	for (int i = 1; i < info->hdr->e_shnum; i++) {
		shdr = &info->sechdrs[i];
		/* Alloc bit cleared means "ignore it." */
		if ((shdr->sh_flags & SHF_ALLOC) && strcmp(info->secstrings + shdr->sh_name, name) == 0) {
			return i;
		}
	}
	return 0;
}

bool module_init_section(const char *name) {
	return strstarts(name, ".init");
}

bool module_exit_section(const char *name) {
	return strstarts(name, ".exit");
}

bool module_init_layout_section(const char *sname) {
	return module_init_section(sname);
}

Elf(Xword) module_get_offset_and_type(struct module *mod, enum mod_mem_type type, Elf(Shdr) * sechdr, unsigned int      section) {
	Elf(Xword) offset = 0;
	Elf(Xword) mask   = 0;

	mask = ((Elf(Xword))(type)&SH_ENTSIZE_TYPE_MASK) << SH_ENTSIZE_TYPE_SHIFT;

	mod->mem[type].size += 0;
	offset = ALIGN(mod->mem[type].size, sechdr->sh_addralign ?: 1);
	mod->mem[type].size = offset + sechdr->sh_size;

	#ifdef ENABLE_DBG_PRINT
	printf("[*] %s: offset: %llu section size: %llu final size: %u\n",
	       __FUNCTION__,
	       (unsigned long long)offset,
	       (unsigned long long)sechdr->sh_size,
	       mod->mem[type].size);
	#endif
	return offset | mask;
}

static void __layout_sections(struct module *mod, struct load_info *info, bool is_init) {
	unsigned int m = 0;
	unsigned int i = 0;
	enum mod_mem_type type = 0;
	Elf(Shdr) *s = NULL;
	const char *sname = NULL;

	static const unsigned long masks[][2] = {
	    {SHF_EXECINSTR | SHF_ALLOC, 0x00},
	    {SHF_ALLOC, SHF_WRITE | 0x00},
	    {SHF_RO_AFTER_INIT | SHF_ALLOC, 0x00},
	    {SHF_WRITE | SHF_ALLOC, 0x00},
	    {0x00 | SHF_ALLOC, 0}};

	static const int core_m_to_mem_type[] = {
	    MOD_TEXT, MOD_RODATA, MOD_RO_AFTER_INIT, MOD_DATA, MOD_DATA,
	};

	static const int init_m_to_mem_type[] = {
	    MOD_INIT_TEXT, MOD_INIT_RODATA, MOD_INVALID,
	    MOD_INIT_DATA, MOD_INIT_DATA,
	};

	for (m = 0; m < ARRAY_SIZE(masks); ++m) {
		type = is_init ? init_m_to_mem_type[m] : core_m_to_mem_type[m];

		for (i = 0; i < info->hdr->e_shnum; ++i) {
			s     = &info->sechdrs[i];
			sname = info->secstrings + s->sh_name;

			if(strstr(sname, "debug") != NULL){
				continue;
			}

			if ((s->sh_flags & masks[m][0]) != masks[m][0] ||
			    (s->sh_flags & masks[m][1]) ||
			    s->sh_entsize != ~0UL ||
			    is_init != module_init_layout_section(sname)) {
				continue;
			}
			if ((type == MOD_INVALID)) {
				continue;
			}

			s->sh_entsize = module_get_offset_and_type(mod, type, s, i);
			#ifdef ENABLE_DBG_PRINT
			printf("%s\n", sname);
			#endif
		}

	}

}

static void layout_sections(struct module *mod, struct load_info *info) {
	for (unsigned int i = 0; i < info->hdr->e_shnum; i++) {
		info->sechdrs[i].sh_entsize = ~0UL;
	}

	#ifdef ENABLE_DBG_PRINT
	printf("[*] core section allocation order for %s:\n", mod->name);
	#endif
	__layout_sections(mod, info, false);

	#ifdef ENABLE_DBG_PRINT
	printf("[*] init section allocation order for %s:\n", mod->name);
	#endif
	__layout_sections(mod, info, true);
	return;
}

static int module_memory_alloc(struct module *mod, enum mod_mem_type type) {
	int ret = 0;
	unsigned int size = PAGE_ALIGN(mod->mem[type].size);
	unsigned int mem_flags = PROT_READ | PROT_WRITE;
	void *ptr = NULL;

	mod->mem[type].size = size;
	#ifdef ENABLE_DBG_PRINT
	printf("[*] module memory alloc %d bytes for type '%s'\n", size, mem_type_to_string(type));
	#endif
	if(type == MOD_TEXT || type == MOD_INIT_TEXT){
		mem_flags |= PROT_EXEC;
	}
	ptr = mmap(NULL, size, mem_flags, MAP_PRIVATE | MAP_ANON | MAP_32BIT, -1, 0x00);

	if (ptr == MAP_FAILED) {
		ret = -ENOMEM;
		return ret;
	}
	director.module_allocation_list = new_allocation(ALLOCATION_TYPE_MMAP, ptr, size, mem_flags, PT_LOAD, director.module_allocation_list);
	memset(ptr, 0, size);
	mod->mem[type].base = ptr;

	return ret;
}

static void module_memory_free(struct module *mod, enum mod_mem_type type, bool unload_codetags) {
	void *ptr = mod->mem[type].base;

	if (mod_mem_type_is_core_data(type)) {
		return;
	}
}

static int move_module(struct module *mod, struct load_info *info) {
	int ret = 0;
	enum mod_mem_type t = 0;
	enum mod_mem_type type = 0;
	void *dest = NULL;
	Elf(Shdr) *shdr	= NULL;

	for_each_mod_mem_type(type) {
		if (!mod->mem[type].size) {
			mod->mem[type].base = NULL;
			continue;
		}

		ret = module_memory_alloc(mod, type);
		if (ret) {
			t = type;
			goto out_enomem;
		}
	}

	#ifdef ENABLE_DBG_PRINT
	printf("[*] final section addresses for %s:\n", mod->name);
	#endif
	for (int i = 0; i < info->hdr->e_shnum; i++) {
		dest = NULL;
		shdr = &info->sechdrs[i];
		type = (enum mod_mem_type)(shdr->sh_entsize >> SH_ENTSIZE_TYPE_SHIFT);

		if (!(shdr->sh_flags & SHF_ALLOC)) {
			continue;
		}

		dest = (char *)mod->mem[type].base + (shdr->sh_entsize & SH_ENTSIZE_OFFSET_MASK);

		if (shdr->sh_type != SHT_NOBITS) {
			memcpy(dest, (void *)shdr->sh_addr, shdr->sh_size);
		}
		shdr->sh_addr = (unsigned long)dest;
		#ifdef ENABLE_DBG_PRINT
		printf("\t0x%lx 0x%.8lx %s\n", (long)shdr->sh_addr, (long)shdr->sh_size, info->secstrings + shdr->sh_name);
		#endif
	}

	return ret;
	out_enomem:
	for (t--; t >= 0; t--) {
		module_memory_free(mod, t, true);
	}
	return ret;
}

static bool is_core_symbol(const Elf(Sym) * src, const Elf(Shdr) * sechdrs, unsigned int shnum, unsigned int pcpundx) {
	enum mod_mem_type type = 0;
	const Elf(Shdr) *sec = NULL;

	if (src->st_shndx == SHN_UNDEF || src->st_shndx >= shnum || !src->st_name) {
		return false;
	}

	sec  = sechdrs + src->st_shndx;
	type = (enum mod_mem_type)(sec->sh_entsize >> SH_ENTSIZE_TYPE_SHIFT);
	if (!(sec->sh_flags & SHF_ALLOC) || mod_mem_type_is_init(type)) {
		return false;
	}

	return true;
}

void layout_symtab(struct module *mod, struct load_info *info) {
	int i = 0;
	int nsrc = 0;
	int ndst = 0;
	int strtab_size = 0;
	Elf(Sym) *src = NULL;

	Elf(Shdr) *symsect = (Elf(Shdr) *)(info->sechdrs + info->index.sym);
	Elf(Shdr) *strsect = (Elf(Shdr) *)(info->sechdrs + info->index.str);
	struct module_memory *mod_mem_data	= &mod->mem[MOD_DATA];
	struct module_memory *mod_mem_init_data = &mod->mem[MOD_INIT_DATA];

	symsect->sh_flags |= SHF_ALLOC;
	symsect->sh_entsize = module_get_offset_and_type(mod, MOD_INIT_DATA, symsect, info->index.sym);
	#ifdef ENABLE_DBG_PRINT
	printf("\t%s\n", info->secstrings + symsect->sh_name);
	#endif

	src  = (void *)info->hdr + symsect->sh_offset;
	nsrc = symsect->sh_size / sizeof(*src);

	for (ndst = i = 0; i < nsrc; i++) {
		if (i == 0 || is_core_symbol(src + i, info->sechdrs, info->hdr->e_shnum, info->index.pcpu)) {
			strtab_size += strlen(&info->strtab[src[i].st_name]) + 1;
			ndst++;
		}
	}

	info->symoffs = ALIGN(mod_mem_data->size, symsect->sh_addralign ?: 1);
	info->stroffs = mod_mem_data->size = info->symoffs + ndst * sizeof(Elf(Sym));
	mod_mem_data->size += strtab_size;
	info->core_typeoffs = mod_mem_data->size;
	mod_mem_data->size += ndst * sizeof(char);

	strsect->sh_flags |= SHF_ALLOC;
	strsect->sh_entsize = module_get_offset_and_type(mod, MOD_INIT_DATA, strsect, info->index.str);
	#ifdef ENABLE_DBG_PRINT
	printf("\t%s\n", info->secstrings + strsect->sh_name);
	#endif

	mod_mem_init_data->size = ALIGN(mod_mem_init_data->size, __alignof__(struct mod_kallsyms));
	info->mod_kallsyms_init_off = mod_mem_init_data->size;

	mod_mem_init_data->size += sizeof(struct mod_kallsyms);
	info->init_typeoffs = mod_mem_init_data->size;
	mod_mem_init_data->size += nsrc * sizeof(char);
}

static struct module *layout_and_allocate(struct load_info *info, int flags) {
	int ret = 0;
	int ndx = 0;
	struct module *mod = NULL;

	ret = module_frob_arch_sections(info->hdr, info->sechdrs, info->secstrings, info->mod);
	if (ret < 0) {
		return NULL;
	}

	ret = module_enforce_rwx_sections(info->hdr, info->sechdrs, info->secstrings, info->mod);
	if (ret < 0) {
		return NULL;
	}

	info->sechdrs[info->index.pcpu].sh_flags &= ~(unsigned long)SHF_ALLOC;
	ndx = find_sec(info, ".data..ro_after_init");

	if (ndx) {
		#ifdef ENABLE_DBG_PRINT
		printf("[*] section %s found!\n", ".data..ro_after_init");
		#endif
		info->sechdrs[ndx].sh_flags |= SHF_RO_AFTER_INIT;
	}

	ndx = find_sec(info, "__jump_table");
	if (ndx) {
		#ifdef ENABLE_DBG_PRINT
		printf("[*] section %s found!\n", "__jump_table");
		#endif
		info->sechdrs[ndx].sh_flags |= SHF_RO_AFTER_INIT;
	}

	layout_sections(info->mod, info);
	layout_symtab(info->mod, info);

	ret = move_module(info->mod, info);
	if (ret) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to move module!\n");
		#endif
		return NULL;
	}

	mod = (void *)info->sechdrs[info->index.mod].sh_addr;
	return mod;
}

static int percpu_modalloc(struct module *mod, struct load_info *info) {
	int ret = 0;

	/* fix: ignore this function */
	return ret;
}

static void *section_objs(const struct load_info *info, const char *name, size_t object_size, unsigned int *num) {
	unsigned int sec = find_sec(info, name);

	/* Section 0 has sh_addr 0 and sh_size 0. */
	*num = info->sechdrs[sec].sh_size / object_size;
	return (void *)info->sechdrs[sec].sh_addr;
}

static void *section_addr(const struct load_info *info, const char *name) {
	/* Section 0 has sh_addr 0. */
	return (void *)info->sechdrs[find_sec(info, name)].sh_addr;
}

void *find_runtime_section(struct module *mod, struct load_info *info, unsigned char *section_name) {
	int ret  = 0;
	enum mod_mem_type type = 0;
	void *dest = NULL;
	Elf(Shdr) *shdr = NULL;

	for (int i = 0; i < info->hdr->e_shnum; i++) {
		dest = NULL;
		shdr = &info->sechdrs[i];
		type = (enum mod_mem_type)(shdr->sh_entsize >> SH_ENTSIZE_TYPE_SHIFT);

		if (!(shdr->sh_flags & SHF_ALLOC)) {
			continue;
		}

		dest = (char *)mod->mem[type].base + (shdr->sh_entsize & SH_ENTSIZE_OFFSET_MASK);
		if (strcmp((unsigned char *)(info->secstrings + shdr->sh_name), section_name) == 0) {
			return (void *)shdr->sh_addr;
		}
	}
	out_enomem:
	return NULL;
}

static int find_module_sections(struct module *mod, struct load_info *info) {
	int ret = 0;
	mod->kp = section_objs(info, "__param", sizeof(*mod->kp), &mod->num_kp);
	mod->syms = section_objs(info, "__ksymtab", sizeof(*mod->syms), &mod->num_syms);
	mod->crcs     = section_addr(info, "__kcrctab");
	mod->gpl_syms = section_objs(info, "__ksymtab_gpl", sizeof(*mod->gpl_syms), &mod->num_gpl_syms);
	mod->gpl_crcs = section_addr(info, "__kcrctab_gpl");

	#ifdef CONFIG_CONSTRUCTORS
	mod->ctors = section_objs(info, ".ctors", sizeof(*mod->ctors), &mod->num_ctors);
	if (!mod->ctors) {
		mod->ctors = section_objs(info, ".init_array", sizeof(*mod->ctors), &mod->num_ctors);
	}else if (find_sec(info, ".init_array")) {
		#ifdef ENABLE_DBG_PRINT
		printf("%s: has both .ctors and .init_array.\n", mod->name);
		#endif
		ret = -EINVAL;
		return ret;
	}
	#endif

	mod->noinstr_text_start = section_objs(info, ".noinstr.text", 1, &mod->noinstr_text_size);
	mod->extable = section_objs(info, "__ex_table", sizeof(*mod->extable), &mod->num_exentries);

	if (section_addr(info, "__obsparm")) {
		#ifdef ENABLE_DBG_PRINT
		printf("[*] %s: Ignoring obsolete parameters\n", mod->name);
		#endif
	}

	#ifdef ENABLE_DYNDBG
	mod->dyndbg_info.descs = section_objs(info, "__dyndbg", sizeof(*mod->dyndbg_info.descs), &mod->dyndbg_info.num_descs);
	mod->dyndbg_info.classes = section_objs(info, "__dyndbg_classes", sizeof(*mod->dyndbg_info.classes), &mod->dyndbg_info.num_classes);
	#endif
	return ret;
}

#if defined(__x86_64__) || defined(_M_X64)
static bool symbol_uses_plt32(struct load_info *info, unsigned int symbol_index){
	Elf(Shdr) *section = NULL;
	Elf(Rela) *relocations = NULL;
	unsigned int relocation_count = 0;

	for (unsigned int i = 1; i < info->hdr->e_shnum; i++) {
		section = &info->sechdrs[i];
		if(section->sh_type != SHT_RELA || section->sh_link != info->index.sym){
			continue;
		}

		relocations = (Elf(Rela) *)(uintptr_t)section->sh_addr;
		relocation_count = section->sh_size / sizeof(*relocations);
		for (unsigned int j = 0; j < relocation_count; j++) {
			if(ELF64_R_SYM(relocations[j].r_info) == symbol_index && ELF64_R_TYPE(relocations[j].r_info) == R_X86_64_PLT32){
				return true;
			}
		}
	}

	return false;
}
#endif

#if defined(__linux__) && defined(__x86_64__)
static bool symbol_needs_plt32_trampoline(struct load_info *info,
					 unsigned int symbol_index,
					 const void *target){
	Elf(Shdr) *section = NULL;
	Elf(Shdr) *target_section = NULL;
	Elf(Rela) *relocations = NULL;
	unsigned int relocation_count = 0;
	uint64_t location = 0;
	int64_t displacement = 0;
	unsigned int relocation_type = 0;

	for (unsigned int i = 1; i < info->hdr->e_shnum; i++) {
		section = &info->sechdrs[i];
		if(section->sh_type != SHT_RELA ||
		   section->sh_link != info->index.sym ||
		   section->sh_info >= info->hdr->e_shnum){
			continue;
		}

		target_section = &info->sechdrs[section->sh_info];
		relocations = (Elf(Rela) *)(uintptr_t)section->sh_addr;
		relocation_count = section->sh_size / sizeof(*relocations);
		for (unsigned int j = 0; j < relocation_count; j++) {
			if(ELF64_R_SYM(relocations[j].r_info) != symbol_index){
				continue;
			}

			relocation_type = ELF64_R_TYPE(relocations[j].r_info);
			if(relocation_type != R_X86_64_PLT32){
				continue;
			}

			location = target_section->sh_addr + relocations[j].r_offset;
			displacement = (int64_t)(uintptr_t)target +
				       relocations[j].r_addend - (int64_t)location;
			if(displacement != (int64_t)(int32_t)displacement){
				return true;
			}
		}
	}

	return false;
}
#endif

static int simplify_symbols(struct module *mod, struct load_info *info) {
	int ret = 0;
	int i = 0;
	bool return_thunk = false;
	bool needs_trampoline = false;
	bool target_is_native_stub = false;
	long secbase = 0;
	void *target = NULL;
        void *dlopen_ctx = NULL;
        void *trampoline = NULL;
        unsigned char *name = NULL;
	struct kernel_symbol *ksym = NULL;

	Elf(Shdr) *symsec = (Elf(Shdr) *)&info->sechdrs[info->index.sym];
	Elf(Sym) *sym	  = (void *)symsec->sh_addr;

	if(native_cb_entries == 0){
		while(native_cb_entries < SYM_ENTRIES && sym_table[native_cb_entries].function_ptr != NULL){
			native_cb_entries++;
		}
	}

	for (i = 1; i < symsec->sh_size / sizeof(Elf(Sym)); i++) {
		name = (unsigned char *)(info->strtab + sym[i].st_name);

		switch (sym[i].st_shndx) {
		case SHN_COMMON:
			if (!strncmp(name, "__gnu_lto", 9)) {
				break;
			}
			#ifdef ENABLE_DBG_PRINT
			printf("[*] common symbol: %s\n", name);
			#endif
			ret = -ENOEXEC;
			break;
		case SHN_ABS:
			#ifdef ENABLE_DBG_PRINT
			printf("[*] absolute symbol: 0x%08lx %s\n", (long)sym[i].st_value, name);
			#endif
			break;
		case SHN_UNDEF:
			target = NULL;
			target_is_native_stub = false;
			for (int j = 0; j < SYM_ENTRIES; j++) {
				if(sym_table[j].function_ptr == NULL){
					continue;
				}
				if (strcmp(name, sym_table[j].function_name) == 0) {
					target = sym_table[j].function_ptr;
					target_is_native_stub = (unsigned int)j < native_cb_entries;
					break;
				}
			}
			#ifdef __linux__
			if (target == NULL) {
				dlopen_ctx = dlopen(NULL, 0);
				target	   = dlsym(dlopen_ctx, (char *)name);
				if(target != NULL){
					target_is_native_stub = true;
				}
			}
			#endif
			if (target == NULL) {
				#ifdef ENABLE_DBG_PRINT
				printf("[!] missing function: %s\n", name);
				#endif
				ret = -ENOEXEC;
				break;
			}
			#if defined(__x86_64__) || defined(_M_X64)
			needs_trampoline = ELF64_ST_TYPE(sym[i].st_info) == STT_FUNC;
			#if defined(__linux__) && defined(__x86_64__)
			/* Keep architecture thunks directly identifiable by apply_retpolines(). */
			needs_trampoline |= target_is_native_stub &&
					      symbol_uses_plt32(info, i) &&
					      strstr((char *)name, "__x86_indirect_thunk_") == NULL &&
					      strcmp((char *)name, "__x86_return_thunk") != 0 &&
					      strcmp((char *)name, "__fentry__") != 0;
			#endif
			return_thunk = false;
			#if defined(__linux__) && defined(__x86_64__)
			needs_trampoline |= symbol_needs_plt32_trampoline(info, i, target);
			if(strcmp((char *)name, "__fentry__") == 0){
				needs_trampoline = false;
				target_is_native_stub = false;
			}
			#endif
			#ifdef _WIN64
			/* Undefined kernel imports are commonly STT_NOTYPE on Windows. */
			needs_trampoline |= symbol_uses_plt32(info, i);
			return_thunk = strcmp((char *)name, "__x86_return_thunk") == 0;
			#endif
			if (needs_trampoline) {
				trampoline = x86_64_make_trampoline(target, target_is_native_stub, return_thunk);
				if (trampoline == NULL) {
					#ifdef ENABLE_DBG_PRINT
					printf("[!] trampoline alloc failed for %s\n", name);
					#endif
					ret = -ENOMEM;
					break;
				}
				sym[i].st_value = (Elf64_Addr)(uintptr_t)trampoline;
				#ifdef ENABLE_DBG_PRINT
				printf("- resolved %-24s target=%p tramp=%p\n", name, target, trampoline);
				#endif
			}else{
				sym[i].st_value = (Elf64_Addr)(uintptr_t)target;
				#ifdef ENABLE_DBG_PRINT
				printf("- resolved %-24s target=%p\n", name, target);
				#endif
			}
			#else
			sym[i].st_value = (Elf64_Addr)(uintptr_t)target;
			#endif
			break;
		default:
			if (sym[i].st_shndx == info->index.pcpu) {
				secbase = (unsigned long)(mod->percpu);
			}else{
				secbase = info->sechdrs[sym[i].st_shndx].sh_addr;
			}
			sym[i].st_value += secbase;
			if(ELF64_ST_TYPE(sym[i].st_info) == STT_FUNC && strcmp(name, "init_module") == 0 && sym[i].st_value != 0){
				director.init_cb = (int (LINUX_ABI *)(void))(uintptr_t)sym[i].st_value;
			}
                        /*
                         * if we have a function which is defined inside this module, add it to the internal symbol table, this will permit
                         * to resolve the relocations when we will have multi-modules dependencies
                         */
                        if ((ELF64_ST_TYPE(sym[i].st_info) == STT_FUNC || ELF64_ST_TYPE(sym[i].st_info) == STT_OBJECT) && strcmp(name, "cleanup_module") != 0 && strcmp(name, "init_module") != 0) {
				/* fix: exclude junk symbols, this introduces a small overhead during the module loading but it requires way less symbols in the sym_table */
				if(name[0] == '_' && name[1] == '_'){
					if(name[2] == 'p' && name[3] == 'f' && name[4] == 'x'){
						break;
					}
					if(name[2] == 'f' && name[3] == 'u' && name[4] == 'n' && name[5] == 'c'){
						break;
					}
				}
				if(name[0] == '_' && name[1] == 'n' && name[2] == 'o' && name[3] == 't' && name[4] == 'e'){
					break;
				}
				if(name[0] == '_' && name[1] == '_' && name[2] == 'U' && name[3] == 'N' && name[4] == 'I' && name[5] == 'Q' && name[6] == 'U' && name[7] == 'E'){
					break;
				}
				if(name[0] == '_' && name[1] == '_' && name[2] == 'F' && name[3] == 'U' && name[4] == 'N' && name[5] == 'C' && name[6] == 'T' && name[7] == 'I'){
					break;
				}
				if(name[0] == '_' && name[1] == '_' && name[2] == '_' && name[3] == '_' && name[4] == 'v' && name[5] == 'e' && name[6] == 'r' && name[7] == 's' && name[8] == 'i'){
					break;
				}
				if(name[0] == '_' && name[1] == '_' && name[2] == 'a' && name[3] == 'l' && name[4] == 'r' && name[5] == 'e' && name[6] == 'a' && name[7] == 'd' && name[8] == 'y'){
					break;
				}
				if(name[0] == '_' && name[1] == '_' && name[2] == 't' && name[3] == 'h' && name[4] == 'i' && name[5] == 's' && name[6] == '_' && name[7] == 'm' && name[8] == 'o'){
					break;
				}
				if(sym[i].st_value != 0){
					for (int j = 0; j < ARRAY_SIZE(sym_table); j++){
						if(sym_table[j].function_ptr != NULL){
							continue;
						}
						sym_table[j].function_ptr = sym[i].st_value;
						memset(sym_table[j].function_name, 0, sizeof(sym_table[j].function_name));
						if(strlen(name) < sizeof(sym_table[j].function_name)){
							strncpy(sym_table[j].function_name, name, strlen(name));
						}else{
							strncpy(sym_table[j].function_name, name, sizeof(sym_table[j].function_name));
						}
						sym_table[j].function_size = sym[i].st_size;
						director.total_memory_used += strlen(name);
						break;
					}
				}
                        }
			break;
		}
	}

	return ret;
}

static int apply_relocations(struct module *mod, const struct load_info *info) {
	int ret = 0;
	int infosec = 0;

	for (int i = 1; i < info->hdr->e_shnum; i++) {
		infosec = info->sechdrs[i].sh_info;

		if (infosec >= info->hdr->e_shnum) {
			continue;
		}

		if (!(info->sechdrs[infosec].sh_flags & SHF_ALLOC)) {
			continue;
		}

		if (info->sechdrs[i].sh_flags & SHF_RELA_LIVEPATCH) {
			#ifdef ENABLE_DBG_PRINT
			printf("[!] livepatch not supported!\n");
			#endif
			return ret;
		}

		if (info->sechdrs[i].sh_type == SHT_REL) {
			ret = apply_relocate(info->sechdrs, info->strtab, info->index.sym, i, mod);
		} else if (info->sechdrs[i].sh_type == SHT_RELA) {
			ret = apply_relocate_add(info->sechdrs, info->strtab, info->index.sym, i, mod);
		}

		if (ret < 0) {
			break;
		}
	}
	return ret;
}

static char elf_type(const Elf(Sym) *sym, const struct load_info *info) {
	const Elf(Shdr) *sechdrs = NULL;

	sechdrs = info->sechdrs;

	if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
		if (ELF_ST_TYPE(sym->st_info) == STT_OBJECT) {
			return 'v';
		} else {
			return 'w';
		}
	}
	if (sym->st_shndx == SHN_UNDEF) {
		return 'U';
	}
	if (sym->st_shndx == SHN_ABS || sym->st_shndx == info->index.pcpu) {
		return 'a';
	}
	if (sym->st_shndx >= SHN_LORESERVE) {
		return '?';
	}
	if (sechdrs[sym->st_shndx].sh_flags & SHF_EXECINSTR) {
		return 't';
	}
	if (sechdrs[sym->st_shndx].sh_flags & SHF_ALLOC &&
	    sechdrs[sym->st_shndx].sh_type != SHT_NOBITS) {
		if (!(sechdrs[sym->st_shndx].sh_flags & SHF_WRITE)) {
			return 'r';
		} else if (sechdrs[sym->st_shndx].sh_flags & 0x00) {
			return 'g';
		} else {
			return 'd';
		}
	}
	if (sechdrs[sym->st_shndx].sh_type == SHT_NOBITS) {
		if (sechdrs[sym->st_shndx].sh_flags & 0x00) {
			return 's';
		} else {
			return 'b';
		}
	}
	if (strstarts(info->secstrings + sechdrs[sym->st_shndx].sh_name, ".debug")) {
		return 'n';
	}
	return '?';
}

void add_kallsyms(struct module *mod, const struct load_info *info) {
	int ret = 0;
	unsigned int i = 0;
	unsigned int ndst = 0;
	unsigned long strtab_size = 0;
	char *s = NULL;
	void *data_base = NULL;
	void *init_data_base = NULL;
	Elf(Sym) *dst = NULL;
	const Elf(Sym) *src = NULL;

	Elf(Shdr) *symsec = &info->sechdrs[info->index.sym];
	data_base = mod->mem[MOD_DATA].base;
	init_data_base = mod->mem[MOD_INIT_DATA].base;

	mod->kallsyms = (void *)init_data_base + info->mod_kallsyms_init_off;

	((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->symtab = (void *)symsec->sh_addr;
	((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->num_symtab =
	    symsec->sh_size / sizeof(Elf(Sym));
	((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->strtab =
	    (void *)info->sechdrs[info->index.str].sh_addr;
	((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->typetab =
	    init_data_base + info->init_typeoffs;

	mod->core_kallsyms.symtab     = dst = data_base + info->symoffs;
	mod->core_kallsyms.strtab     = s = data_base + info->stroffs;
	mod->core_kallsyms.typetab    = data_base + info->core_typeoffs;
	strtab_size		      = info->core_typeoffs - info->stroffs;
	src			      = rcu_dereference(mod->kallsyms)->symtab;

	for (ndst = i = 0; i < ((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->num_symtab;
	     i++) {
		((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->typetab[i] =
		    elf_type(src + i, info);
		if (i == 0 ||
		    is_core_symbol(src + i, info->sechdrs, info->hdr->e_shnum,
				   info->index.pcpu)) {
			ret = 0;

			mod->core_kallsyms.typetab[ndst] =
			    ((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->typetab[i];
			dst[ndst]	    = src[i];
			dst[ndst++].st_name = s - mod->core_kallsyms.strtab;

			ret = strscpy(s,
				      &((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))
					   ->strtab[src[i].st_name],
				      strtab_size);

			if (ret < 0) {
				break;
			}

			s += ret + 1;
			strtab_size -= ret + 1;
		}
	}

	mod->core_kallsyms.num_symtab = ndst;
}

void sort_extable(struct exception_table_entry *start, struct exception_table_entry *finish) {
	/* do nothing */
}

static int post_relocation(struct module *mod, const struct load_info *info) {
	int ret = 0;
	sort_extable(mod->extable, mod->extable + mod->num_exentries);

	add_kallsyms(mod, info);
	ret = module_finalize(info->hdr, info->sechdrs, mod);
	return ret;
}

static int module_set_memory(const struct module *mod, enum mod_mem_type type) {
	int ret = 0;
	unsigned int mprotect_perm = 0;
	struct module_memory *mod_mem = NULL;

	mod_mem = (struct module_memory *)&mod->mem[type];
	if (mod_mem->base == NULL) {
		return ret;
	}

	switch (type) {
	case MOD_RODATA:
	case MOD_INIT_RODATA:
	case MOD_RO_AFTER_INIT:
		mprotect_perm = PROT_READ | PROT_WRITE;
		break;
	case MOD_DATA:
	case MOD_INIT_DATA:
		mprotect_perm = PROT_READ | PROT_WRITE;
		break;
	case MOD_TEXT:
	case MOD_INIT_TEXT:
		mprotect_perm = PROT_READ | PROT_WRITE | PROT_EXEC;
		break;
	default:
		#ifdef ENABLE_DBG_PRINT
		printf("[!] unknown memory type: %d\n", type);
		#endif
		return ret;
	}

	#ifdef _WIN64
	if(mod_mem->size == 0){
		return ret;
	}
	#endif

	ret = mprotect(mod_mem->base, mod_mem->size, mprotect_perm);
	return ret;
}

int module_enable_rodata_ro(const struct module *mod, bool after_init) {
	int ret = 0;

	ret = module_set_memory(mod, MOD_RODATA);
	if (ret) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to set read-only .rodata permissions!\n");
		#endif
		return ret;
	}

	ret = module_set_memory(mod, MOD_INIT_RODATA);
	if (ret) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to set read-only .mod_init_rodata permissions!\n");
		#endif
		return ret;
	}

	if (after_init) {
		ret = module_set_memory(mod, MOD_RO_AFTER_INIT);
	}
	return ret;
}

int module_enable_data_nx(const struct module *mod) {
	int ret = 0;

	for_class_mod_mem_type(type, data) {
		ret = module_set_memory(mod, type);
		if (ret) {
			#ifdef ENABLE_DBG_PRINT
			printf("[!] failed to set memory in \'module_enable_data_nx\'\n");
			#endif
			return ret;
		}
	}
	return ret;
}

int module_enable_text_rox(const struct module *mod) {
	int ret = 0;

	for_class_mod_mem_type(type, text) {
		ret = module_set_memory(mod, type);
		if (ret) {
			#ifdef ENABLE_DBG_PRINT
			printf("[!] failed to set memory in \'module_enable_text_rox\'\n");
			#endif
			return ret;
		}
	}
	return ret;
}

static int complete_formation(struct module *mod, struct load_info *info) {
	int ret = 0;

	ret = module_enable_rodata_ro(mod, false);
	if (ret) {
		goto out_strict_rwx;
	}

	ret = module_enable_data_nx(mod);
	if (ret) {
		goto out_strict_rwx;
	}

	ret = module_enable_text_rox(mod);
	if (ret) {
		goto out_strict_rwx;
	}

	mod->state = MODULE_STATE_COMING;
	return ret;

	out_strict_rwx:
	/* do nothing */
	out:
	return ret;
}

static void do_mod_ctors(struct module *mod) {
	/* do nothing here */
}

static inline unsigned long ex_to_insn(const struct exception_table_entry *x) {
	return (unsigned long)&x->insn + x->insn;
}

static inline bool within_module_init(unsigned long addr, const struct module *mod) {
	for_class_mod_mem_type(type, init) {
		if (within_module_mem_type(addr, mod, type)) {
			return true;
		}
	}
	return false;
}

void trim_init_extable(struct module *mod) {
	int num_entries_in_mod = 0;

	while (mod->num_exentries &&
	       within_module_init(ex_to_insn(&mod->extable[0]), mod)) {
		mod->extable++;
		mod->num_exentries--;
	}
	num_entries_in_mod = mod->num_exentries - 1;
	while (mod->num_exentries &&
	       within_module_init(ex_to_insn(&mod->extable[num_entries_in_mod]),
				  mod)) {
		mod->num_exentries--;
		num_entries_in_mod = mod->num_exentries - 1;
	}
}

static int do_init_module(struct module *mod) {
	int ret = 0;

	do_mod_ctors(mod);
	mod->state = MODULE_STATE_LIVE;
	director.mod = mod;
	/* Keep the latest loaded module's init callback; dependency modules are loaded first. */
	if(mod->init != NULL){
		director.init_cb = (int (LINUX_ABI *)(void))mod->init;
	}
	trim_init_extable(mod);
	return ret;
}

/* call_final_function: lookup the function name and invoke it if found */
static int call_final_function(struct module *mod, struct load_info *info, unsigned char *function_name) {
	int ret = 0;
	unsigned int n = 0;
	char *strtab = NULL;
	const char *name = NULL;
	unsigned char *ptr = NULL;
	int (LINUX_ABI *fn)(void) = NULL;
	Elf(Ehdr) *header = info->hdr;
	Elf(Shdr) *symsect = &info->sechdrs[info->index.sym];

	Elf(Sym) *sym = (void *)symsect->sh_addr;
	strtab = (void *)info->sechdrs[info->index.str].sh_addr;
	n = symsect->sh_size / sizeof(*sym);

	if(strcmp(function_name, "entry") == 0){
		fn = (int (*)(void))(uintptr_t)header->e_entry;
		ptr = (unsigned char *)fn;
		if (ptr[0] == 0xE8) {
			int32_t disp = *(int32_t *)(ptr + 1);
			void   *tgt  = ptr + 5 + disp;
 		}

		/* mark the module execution */
		printf("--------------------------\n");
		ret = fn();
		#ifdef ENABLE_DBG_PRINT
		printf("[*] %s returned %d\n", name, ret);
		#endif
	}

	for (unsigned int i = 0; i < n; i++) {
		name = &strtab[sym[i].st_name];

		if (name == NULL || name[0] == '\0'){
			continue;
		}

		if (strcmp((char *)function_name, name) == 0) {
			fn = (int (*)(void))(uintptr_t)sym[i].st_value;
			ptr = (unsigned char *)fn;
			if (ptr[0] == 0xE8) {
				int32_t disp = *(int32_t *)(ptr + 1);
				void   *tgt  = ptr + 5 + disp;
			}

			/* mark the module execution */
			printf("--------------------------\n");
			ret = fn();
			#ifdef ENABLE_DBG_PRINT
			printf("[*] %s returned %d\n", name, ret);
			#endif
			return ret;
		}
	}
	#ifdef ENABLE_DBG_PRINT
	printf("[!] function not found in symtab: %s\n", function_name);
	#endif
	ret = -ENOENT;
	return ret;
}

/* update_kernel_version: update the kernel version by converting the vermagic string, it will be useful later for setting the function arguments */
void update_kernel_version(unsigned char *vermagic){
	char *separate_version = NULL;
	unsigned char kernel_version_string_part[32] = {0};

	if(vermagic == NULL || strlen(vermagic) < 16){
		goto end;
	}

	if(sscanf(vermagic, "%31s", kernel_version_string_part) != 1){
		goto end;
	}

	/* isolate the version.subversion part */
	separate_version = strchr(kernel_version_string_part, '-');

	if (separate_version != NULL) {
		*separate_version = '\0';
	}

	/* final parsing */
	if (sscanf(kernel_version_string_part, "%hhu.%hhu.%hhu", &director.kernel_major_version, &director.kernel_minor_version, &director.kernel_patch) != 3){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to parse kernel version!\n");
		#endif
		goto end;
	}

	end:
}

/* save_kernel_module_name: save the currently loaded kernel module name */
void save_kernel_module_name(unsigned char *name){
	if(name == NULL || name[0] == 0){
		goto end;
	}
	memset(director.module_name, 0, sizeof(director.module_name));
	snprintf(director.module_name, sizeof(director.module_name), "%s_%hhu.%hhu.%hhu.elf", name, director.kernel_major_version, director.kernel_minor_version, director.kernel_patch);

	end:
}

/* module_init: main entry point of the kernel "module_init" function, permits to load entirely a kernel module,
 * in our case instead, it will load it only in memory and wait for the function to run */
int module_init(unsigned char *file_name, unsigned char *function_name, struct load_info *info) {
	int ret = 0;
	int fd = 0;
	unsigned int file_size	= 0;
	bool module_allocated = false;
	struct module *mod = NULL;
	char *after_dashes = NULL;
	unsigned char *mapped_file = NULL;
	unsigned char *tmp_mapped_file = NULL;
	struct stat file_stats = {};

	if (file_name == NULL) {
		ret = -EINVAL;
		#ifdef ENABLE_DBG_PRINT
		printf("[!] file name is null!\n");
		#endif
		return ret;
	}

	fd = open(file_name, O_RDONLY);
	if(fd < 0){
		ret = -EINVAL;
		#ifdef ENABLE_DBG_PRINT
		printf("[!] file %s not exist!\n", file_name);
		#endif
		return ret;
	}

	fstat(fd, &file_stats);
	file_size = file_stats.st_size;
	if(file_size == 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] file %s is empty!\n", file_name);
		#endif
		ret = -EINVAL;
		return ret;
	}

	tmp_mapped_file = (unsigned char *)mmap(NULL, file_size, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
	mapped_file = (unsigned char *)mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if(tmp_mapped_file == MAP_FAILED || mapped_file == MAP_FAILED){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] error during mmap! aborting...\n");
		#endif
		if(tmp_mapped_file != NULL){
			munmap(tmp_mapped_file, file_size);
		}
		if(mapped_file != NULL){
			munmap(mapped_file, file_size);
		}
		close(fd);
		ret = -EINVAL;
		return ret;
	}

	/* track the size of the *entire* module loaded in memory */
	director.total_memory_used += file_size;
	memset(mapped_file, 0x00, file_size);
	memcpy(mapped_file, tmp_mapped_file, file_size);
	munmap(tmp_mapped_file, file_size);

	director.fd = fd;
	/* skip completely the dwarf module parsing if the end user has already issued the offset configuration file */
        if(director.offset_init_by_conf == true){
		goto skip_dwarf;
	}else{
		ret = dwarf_init_b(director.fd, DW_DLC_READ, DW_GROUPNUMBER_ANY, NULL, NULL, &director.dbg, &director.err);
		if(ret != DW_DLV_OK){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] failed to initialize libdwarf!\n");
			#endif
			/*
			 * instead of not loading the kernel object,
			 * skip this phase, we may rely on other kernel objects loaded alongisde this module!
			 */
			goto skip_dwarf;
		}
	}
	parse_module_offset_table();

	skip_dwarf:
	close(fd);
	info->hdr = (Elf(Ehdr) *)mapped_file;
	info->len = file_size;
	elf_validity_cache_copy(info, 0);
	early_mod_check(info, 0);

	#ifdef ENABLE_DBG_PRINT
	printf("[*] name %s\n", get_modinfo(info, "name"));
	printf("[*] vermagic %s\n", get_modinfo(info, "vermagic"));
	#endif

	/* save the kernel version values for later use */
	update_kernel_version(get_modinfo(info, "vermagic"));
	/* save the module name for later use */
	save_kernel_module_name(get_modinfo(info, "name"));

	mod = layout_and_allocate(info, 0);
	if (mod == NULL) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] module struct is null!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	module_allocated = true;
	ret  = percpu_modalloc(mod, info);
	if (ret) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to allocate the percpu variable!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	ret = find_module_sections(mod, info);
	if (ret) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to detect module sections!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	ret = simplify_symbols(mod, info);
	if (ret < 0 && director.global_conf.strict_missing_syms == true) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to simplify symbols!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	ret = apply_relocations(mod, info);
	if (ret < 0) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed the relocations patch phase!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	ret = post_relocation(mod, info);
	if (ret < 0) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed the post relocation phase!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	ret = complete_formation(mod, info);
	if (ret) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to set permissions for the module\'s segments\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	ret = do_init_module(mod);
	if (ret) {
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to detect the module\'s probe function\n");
		#endif
		ret = -EINVAL;
		return ret;
	}
	free(director.info);
	director.info = NULL;
	director.loaded_lkm_num++;
	munmap(mapped_file, file_size);
	mapped_file = NULL;
	file_size = 0;
	return ret;
}

/* workqueue_init: internal function used for initialize a given workqueue */
int workqueue_init(struct workqueue_struct *wq, const char *name) {
	int ret = 0;
	pthread_condattr_t cattr = {};

	strncpy(wq->name, name, sizeof(wq->name));
	pthread_mutex_init(&wq->lock, NULL);

	pthread_condattr_init(&cattr);
	wq->clock_id = CLOCK_MONOTONIC;
	pthread_condattr_setclock(&cattr, wq->clock_id);
	pthread_cond_init(&wq->cond, &cattr);
	pthread_condattr_destroy(&cattr);

	INIT_LIST_HEAD(&wq->work_list);
	INIT_LIST_HEAD(&wq->delayed_list);
	wq->stop = 0;
	wq->nr_active = 0;

	ret = pthread_create(&wq->thread, NULL, worker_thread, wq);
	return ret;
}

/* initialize_director: allocate and fill the fields for the main global context */
int initialize_director(struct module_controller *director, struct load_info **info) {
	int ret = 0;

	director->runtime_generated_structs = &runtime_generated_structs;
        director->info = calloc(1, sizeof(struct load_info));
	if (director->info == NULL){
		ret = -ENOMEM;
		return ret;
	}
	if(director->rfkill == NULL){
		director->rfkill = calloc(1, sizeof(struct rfkill));
		if(director->rfkill == NULL){
			ret = -ENOMEM;
			return ret;
		}
	}

	/* initialize workqueues if necessary */
	if(system_wq == NULL){
		workqueue_init(&director->system_wq, "system_wq");
		system_wq = &director->system_wq;
	}
	if(system_percpu_wq == NULL){
		workqueue_init(&director->system_percpu_wq, "system_percpu_wq");
		system_percpu_wq = &director->system_percpu_wq;
	}
	if(system_dfl_wq == NULL){
		workqueue_init(&director->system_dfl_wq, "system_dfl_wq");
		system_dfl_wq = &director->system_dfl_wq;
	}

	if(system_freezable_wq == NULL){
		workqueue_init(&director->system_freezable_wq, "system_freezable_wq");
		system_freezable_wq = &director->system_freezable_wq;
	}

	/* for tasklets, just allocate an array of 'pthreads' */
	if(director->tasklet_workers == NULL){
		tasklet_subsystem_init(MAX_SPAWNED_TASKLETS);
	}

	/* initialize usb i/o mutex */
	pthread_mutex_init(&director->urb_anchor_mutex, NULL);
	pthread_mutex_init(&director->usb_io_lock, NULL);
	pthread_cond_init(&director->usb_io_idle, NULL);
	director->usb_io_depth = 0;

	/* initialize skb queue mutex */
	pthread_mutex_init(&director->skb_queue_lock, NULL);
	/* initialize the scan lock */
	pthread_mutex_init(&director->scan_cache_lock, NULL);
	connection_runtime_init();
	pthread_mutex_init(&director->wiphy_work_queue_mutex, NULL);
	/* set to '0' the scan and rx_dump parameters */
	atomic_store(&director->scan_generation, 0);
	atomic_store(&director->scan_result_epoch, 0);
	atomic_store(&director->scan_in_progress, false);
	atomic_store(&director->wifi_scan_thread_started, false);
	atomic_store(&director->wifi_scan_thread_stop, false);
	director->wifi_scan_thread = 0;
	atomic_store(&director->rx_dump_enabled, false);
	atomic_store(&director->rx_dump_sequence, 0);

	pthread_mutex_init(&director->timer_lock, NULL);
	pthread_cond_init(&director->timer_cond, NULL);
	director->timer_once  = PTHREAD_ONCE_INIT;

	/* if enabled, initialize also the tcc scripting layer */
	#ifdef TCC_SCRIPTING
	if(director->tcc_ctx == NULL){
		ret = drv_integrate_scripting();
		if(ret != 0){
			return ret;
		}
	}
	#endif
	*info = director->info;
	return ret;
}

/* finalize_director: clean everything allocated in the main controller */
void finalize_director(void){
	wifi_connection_stop(director.ieee80211_local);
	#ifdef ENABLE_FUZZING
	/* delete the urb queue used for fuzzing, not always initialized */
	fuzz_assign_urb_cb(NULL);
	#endif
	#ifdef TCC_SCRIPTING
	drv_shutdown_scripting();
	#endif
	if(director.info != NULL){
		free(director.info);
		director.info = NULL;
	}
	if(director.scan_in_progress == true){
		stop_wifi_scan_thread();
	}
	/* Stop the libusb event thread BEFORE freeing USB resources it accesses;
	 * otherwise Thread 3 races on the freed usb_device_ptr block. */
	atomic_store_explicit(&director.libusb_thread_stop, true, memory_order_release);
	libusb_interrupt_event_handler(usb_libusb_context());
	if (director.libusb_event_thread != 0) {
		pthread_join(director.libusb_event_thread, NULL);
		director.libusb_event_thread = 0;
	}
	usb_runtime_exit();
	#ifdef ENABLE_FUZZING
	fuzz_runtime_destroy();
	#endif
	tasklet_subsystem_shutdown();
	if(system_wq != NULL){
		destroy_workqueue(system_wq);
		system_wq = NULL;
	}
	if(system_percpu_wq != NULL){
		destroy_workqueue(system_percpu_wq);
		system_percpu_wq = NULL;
	}
	if(system_dfl_wq != NULL){
		destroy_workqueue(system_dfl_wq);
		system_dfl_wq = NULL;
	}
	if(system_freezable_wq != NULL){
		destroy_workqueue(system_freezable_wq);
		system_freezable_wq = NULL;
	}
	if(director.ieee80211_local != NULL){
		ieee80211_free_hw(director.ieee80211_local->ieee80211_hw);
	}
	free_usb_interface_alloc(director.usb_interface);
	if(director.usb_device_ptr != NULL){
		free(director.usb_device_ptr);
		director.usb_device_ptr = NULL;
	}
	if(director.rfkill != NULL){
		free(director.rfkill);
		director.rfkill = NULL;
	}
	free_tracked_urb_allocs();
	free_tracked_skb_allocs();
	free_tracked_kmem_allocs();
	if(percpu_area != NULL){
		munmap(percpu_area, 1024 * 1024 * 8);
		percpu_area = NULL;
	}
}

/* collect_sizes: called at once during bootup for assigning the size of the struct adopted by the loaded module into the main global context */
int collect_sizes(struct module_controller *director){
	int ret = 0;
	u16 current_struct_size = 0;

	memset(&director->struct_sizes, 0, sizeof(director->struct_sizes));
	current_struct_size = get_struct_size("file");
	if(current_struct_size != director->struct_sizes.file && current_struct_size > 0){
		director->struct_sizes.file = current_struct_size;
	}
	current_struct_size = get_struct_size("wiphy");
	if(current_struct_size != director->struct_sizes.wiphy && current_struct_size > 0){
		director->struct_sizes.wiphy = current_struct_size;
	}
        current_struct_size = get_struct_size("ieee80211_hw");
        if(current_struct_size != director->struct_sizes.ieee80211_hw && current_struct_size > 0){
                director->struct_sizes.ieee80211_hw = current_struct_size;
        }
	current_struct_size = get_struct_size("ieee80211_sta");
	if(current_struct_size != director->struct_sizes.ieee80211_sta && current_struct_size > 0){
		director->struct_sizes.ieee80211_sta = current_struct_size;
	}
        current_struct_size = get_struct_size("ieee80211_vif");
        if(current_struct_size != director->struct_sizes.ieee80211_vif && current_struct_size > 0){
                director->struct_sizes.ieee80211_vif = current_struct_size;
        }
	current_struct_size = get_struct_size("cfg80211_scan_request");
	if(current_struct_size != director->struct_sizes.cfg80211_scan_request && current_struct_size > 0){
		director->struct_sizes.cfg80211_scan_request = current_struct_size;
	}
	current_struct_size = get_struct_size("cfg80211_ap_settings");
	if(current_struct_size != director->struct_sizes.cfg80211_ap_settings && current_struct_size > 0){
		director->struct_sizes.cfg80211_ap_settings = current_struct_size;
	}
	current_struct_size = get_struct_size("ieee80211_scan_request");
	if(current_struct_size != director->struct_sizes.ieee80211_scan_request && current_struct_size > 0){
		director->struct_sizes.ieee80211_scan_request = current_struct_size;
	}
        current_struct_size = get_struct_size("ieee80211_bss_conf");
        if(current_struct_size != director->struct_sizes.ieee80211_bss_conf && current_struct_size > 0){
                director->struct_sizes.ieee80211_bss_conf = current_struct_size;
        }
        current_struct_size = get_struct_size("ieee80211_ops");
        if(current_struct_size != director->struct_sizes.ieee80211_ops && current_struct_size > 0){
                director->struct_sizes.ieee80211_ops = current_struct_size;
        }
	current_struct_size = get_struct_size("cfg80211_ops");
	if(current_struct_size != director->struct_sizes.cfg80211_ops && current_struct_size > 0){
		director->struct_sizes.cfg80211_ops = current_struct_size;
	}
        current_struct_size = get_struct_size("module");
        if(current_struct_size != director->struct_sizes.module && current_struct_size > 0){
                director->struct_sizes.module = current_struct_size;
        }
        current_struct_size = get_struct_size("net_device");
        if(current_struct_size != director->struct_sizes.net_device && current_struct_size > 0){
                director->struct_sizes.net_device = current_struct_size;
        }
        current_struct_size = get_struct_size("rate_info");
        if(current_struct_size != director->struct_sizes.rate_info && current_struct_size > 0){
                director->struct_sizes.rate_info = current_struct_size;
        }
        current_struct_size = get_struct_size("sk_buff");
        if(current_struct_size != director->struct_sizes.sk_buff && current_struct_size > 0){
                director->struct_sizes.sk_buff = current_struct_size;
        }
        current_struct_size = get_struct_size("sk_buff_head");
        if(current_struct_size != director->struct_sizes.sk_buff_head && current_struct_size > 0){
                director->struct_sizes.sk_buff_head = current_struct_size;
        }
        current_struct_size = get_struct_size("skb_shared_info");
        if(current_struct_size != director->struct_sizes.skb_shared_info && current_struct_size > 0){
                director->struct_sizes.skb_shared_info = current_struct_size;
        }
        current_struct_size = get_struct_size("sock");
        if(current_struct_size != director->struct_sizes.sock && current_struct_size > 0){
                director->struct_sizes.sock = current_struct_size;
        }
        current_struct_size = get_struct_size("urb");
        if(current_struct_size != director->struct_sizes.urb && current_struct_size > 0){
                director->struct_sizes.urb = current_struct_size;
        }
        current_struct_size = get_struct_size("usb_device");
        if(current_struct_size != director->struct_sizes.usb_device && current_struct_size > 0){
                director->struct_sizes.usb_device = current_struct_size;
        }
        current_struct_size = get_struct_size("usb_driver");
        if(current_struct_size != director->struct_sizes.usb_driver && current_struct_size > 0){
                director->struct_sizes.usb_driver = current_struct_size;
        }
        current_struct_size = get_struct_size("usb_interface");
        if(current_struct_size != director->struct_sizes.usb_interface && current_struct_size > 0){
                director->struct_sizes.usb_interface = current_struct_size;
        }
	current_struct_size = get_struct_size("usb_bus");
	if(current_struct_size != director->struct_sizes.usb_bus && current_struct_size > 0){
		director->struct_sizes.usb_bus = current_struct_size;
	}
        current_struct_size = get_struct_size("device");
        if(current_struct_size != director->struct_sizes.device && current_struct_size > 0){
                director->struct_sizes.device = current_struct_size;
        }
        current_struct_size = get_struct_size("hci_dev");
        if(current_struct_size != director->struct_sizes.hci_dev && current_struct_size > 0){
                director->struct_sizes.hci_dev = current_struct_size;
        }
	current_struct_size = get_struct_size("cfg80211_chan_def");
	if(current_struct_size != director->struct_sizes.cfg80211_chan_def && current_struct_size > 0){
		director->struct_sizes.cfg80211_chan_def = current_struct_size;
	}
	current_struct_size = get_struct_size("ieee80211_chanctx_conf");
	if(current_struct_size != director->struct_sizes.ieee80211_chanctx_conf && current_struct_size > 0){
		director->struct_sizes.ieee80211_chanctx_conf = current_struct_size;
	}
	/* fallback method for 'skb_shared_info' */
        if(director->struct_sizes.skb_shared_info == 0){
                director->struct_sizes.skb_shared_info = sizeof(struct skb_shared_info);
        }
        return ret;
}

/* FASTAPI: a very minimal and direct way for executing an immediate action (load a kernel driver, executing a function..) */

/* fast_drv_load: load directly a .ko module into memory and initialize it */
int fast_drv_load(struct module_controller *director, char *module_name) {
	int ret = 0;
	struct load_info *load_info = NULL;

	if(module_name == NULL){
		ret = -EINVAL;
		return ret;
	}

	if(strlen(module_name) < MODULE_INPUT_MIN_LEN){
		printf("[!] module %s too short!\n", module_name);
		ret = -EINVAL;
		return ret;
	}

	ret = initialize_director(director, &load_info);
	if (ret) {
		printf("[!] failed to initialize the main context!\n");
		return ret;
	}

	/* before quitting, collect the struct sizes now for getting a fast access later */
	ret = collect_sizes(director);
	/* now call the main driver loading function */
	ret = module_init(module_name, NULL, load_info);
	return ret;
}

/* fast_get_symbol: get the symbol pointer and it's size */
int fast_get_symbol(unsigned char *sym_name, void **sym_addr){
	int ret = 0;
	unsigned char *current_function_name = NULL;

	for (u16 i = 0; i < SYM_ENTRIES; i++) {
		if(sym_table[i].function_name[0] == '\0'){
			continue;
		}
		current_function_name = &sym_table[i].function_name;
		if(strstr(current_function_name, sym_name) != NULL){
			if(sym_table[i].function_size != 0){
				ret = sym_table[i].function_size;
			}
			*sym_addr = (void *)sym_table[i].function_ptr;
			break;
		}
	}
	return ret;
}

/* fast_get_symbol_from_parts: given 2 strings, find the object/function pointer whose name contains the strings and return it's size */
int fast_get_symbol_from_parts(unsigned char *sym_name_part_one, unsigned char *sym_name_part_two, void **sym_addr){
        int ret = 0;
        unsigned char *current_function_name = NULL;

        for (u16 i = 0; i < SYM_ENTRIES; i++) {
                if(sym_table[i].function_name[0] == '\0'){
                        continue;
                }
                current_function_name = &sym_table[i].function_name;
                if(strstr(current_function_name, sym_name_part_one) != NULL && strstr(current_function_name, sym_name_part_two) != NULL){
                        if(sym_table[i].function_size != 0){
                                ret = sym_table[i].function_size;
                        }
                        *sym_addr = (void *)sym_table[i].function_ptr;
                        break;
                }
        }
        return ret;
}

/* fast_get_name: do the opposite of the function above: given a function pointer, retrieve it's name */
int fast_get_name(void *sym_addr, unsigned char **sym_name){
	int ret = 0;

	for (u16 i = 0; ; i++) {
		if(sym_table[i + 1].function_name[0] == '\0'){
			break;
		}
		if(sym_addr == sym_table[i].function_ptr){
			if(sym_table[i].function_size != 0){
				ret = sym_table[i].function_size;
			}
			*sym_name = sym_table[i].function_name;
			break;
		}
	}
	return ret;
}

/* fast_get_name_in_range: similar to 'fast_get_name' but search the symbol inside a range (function_ptr + function_size) */
int fast_get_name_in_range(void *sym_addr, unsigned char **sym_name){
	int ret = 0;
	*sym_name = NULL;

	for (u16 i = 0; ; i++) {
		if(sym_table[i + 1].function_name[0] == '\0'){
			break;
		}

		if (sym_table[i].function_ptr <= sym_addr && sym_addr < sym_table[i].function_ptr + sym_table[i].function_size){
			*sym_name = sym_table[i].function_name;
			break;
		}
	}
	return ret;
}

/* fast_run_cfg80211_cb: used in the standalone ELF dump, permits to invoke cfg80211 (and callbacks from 'net_device_ops') callbacks in a simpler way */
int fast_run_cfg80211_cb(enum fast_run_ops current_ops) {
	int ret = 0;
	struct wiphy *wiphy = NULL;
	struct wireless_dev *wdev = NULL;
	struct net_device *net_device = NULL;
	struct cfg80211_ops *cfg80211_ops = NULL;
	void (*abort_scan)(struct wiphy *wiphy, struct wireless_dev *wdev) = NULL;
	int (*del_virtual_intf)(struct wiphy *wiphy, struct wireless_dev *wdev) = NULL;
	int (*testmode_cmd)(struct wiphy *wiphy, struct wireless_dev *wdev, void *data, int len) = NULL;
	int (*cfg80211_stop_ap)(struct wiphy *wiphy, struct net_device *dev, unsigned int link_id) = NULL;
	int (*auth)(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_auth_request *req) = NULL;
	int (*assoc)(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_assoc_request *req) = NULL;
	int (*deauth)(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_deauth_request *req) = NULL;
	int (*cfg80211_start_ap)(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_ap_settings *settings) = NULL;
	int (*testmode_dump)(struct wiphy *wiphy, struct sk_buff *skb, struct netlink_callback *cb, void *data, int len) = NULL;
	int (*change_virtual_intf)(struct wiphy *wiphy, struct net_device *dev, enum nl80211_iftype type, struct vif_params *params) = NULL;
	struct wireless_dev *(*add_virtual_intf)(struct wiphy *wiphy, const char *name, unsigned char name_assign_type, enum nl80211_iftype type, struct vif_params *params) = NULL;

	net_device = director.net_device;
	if(net_device == NULL){
		printf("[!] net_device is NULL!\n");
		ret = -EINVAL;
		return ret;
	}
	if(current_ops == NDO_INIT){
		ret = netdevice_init(net_device);
	}
	if(current_ops == NDO_UNINIT){
		ret = netdevice_uninit(net_device);
	}
	if(current_ops == NDO_OPEN){
		ret = netdevice_open(net_device);
	}
	if(current_ops == NDO_STOP){
		ret = netdevice_stop(net_device);
	}
	if(current_ops == NDO_VALIDATE_ADDR){
		ret = netdevice_validate_address(net_device);
	}
	if(current_ops == NDO_SET_RX_MODE){
		ret = netdevice_set_rx_mode(net_device);
	}

	/* if the cfg80211 callbacks are required, check if 'cfg80211_ops' is valid */
	cfg80211_ops = director.cfg80211_ops;
	if(cfg80211_ops == NULL){
		printf("[!] cfg80211_ops is NULL!\n");
		ret = -EINVAL;
		return ret;
	}
	wiphy = director.wiphy;
	if(wiphy == NULL){
		printf("[!] wiphy is NULL!\n");
		ret = -EINVAL;
		return ret;
	}
	wdev = director.wdev;
	if(wdev == NULL && net_device->ieee80211_ptr != NULL){
		wdev = net_device->ieee80211_ptr;
		director.wdev = wdev;
	}
	if(wdev == NULL){
		printf("[!] wdev is NULL!\n");
		ret = -EINVAL;
		return ret;
	}

	if(current_ops == CFG80211_SCAN){
		ret = start_cfg80211_scan(wiphy, cfg80211_ops);
	}
	if(current_ops == CFG80211_ABORT_SCAN){

	}
	if(current_ops == CFG80211_ADD_INTERFACE){
		director.wdev = cfg80211_add_virtual_intf(wiphy, cfg80211_ops);
	}
	if(current_ops == CFG80211_CHANGE_INTERFACE){

	}
	if(current_ops == CFG80211_DELETE_INTERFACE){

	}
	if(current_ops == CFG80211_START_AP){

	}
	if(current_ops == CFG80211_STOP_AP){

	}
	if(current_ops == CFG80211_AUTH){

	}
	if(current_ops == CFG80211_ASSOC){

	}
	if(current_ops == CFG80211_DEAUTH){

	}
	if(current_ops == CFG80211_DISASSOC){

	}
	if(current_ops == CFG80211_CONNECT){

	}
	if(current_ops == CFG80211_GET_TX_POWER){

	}
	if(current_ops == CFG80211_SET_TX_POWER){

	}
	return ret;
}

/* fast_run_ieee80211_cb: used in the standalone ELF dump, permits to invoke ieee80211 callbacks in a simpler way */
int fast_run_ieee80211_cb(enum fast_run_ops current_op) {
	int ret = 0;
	struct ieee80211_ops *ops = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_hw *ieee80211_hw = NULL;
	struct ieee80211_bss_conf *bss_conf = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;
	struct ieee80211_vif *ieee80211_ap_vif = NULL;
	struct ieee80211_vif *ieee80211_mon_vif = NULL;
	int (*start)(struct ieee80211_hw *hw) = NULL;
	void (*stop)(struct ieee80211_hw *hw, bool suspend) = NULL;
	int (*suspend)(struct ieee80211_hw *hw, struct cfg80211_wowlan *wowlan) = NULL;
	int (*add_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	void (*cancel_hw_scan)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	void (*remove_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	int (*hw_scan)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_scan_request *req) = NULL;
	int (*start_ap)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_bss_conf *link_conf) = NULL;
	void (*stop_ap)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_bss_conf *link_conf) = NULL;
	int (*ampdu_action)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_ampdu_params *params) = NULL;
	void (*configure_filter)(struct ieee80211_hw *hw, unsigned int changed_flags, unsigned int *new_flags, u64 multicast) = NULL;
	/* additional wowlan structs */
	struct cfg80211_pkt_pattern wowlan_pattern = {};
	u8 wowlan_packet[WOWLAN_MAGIC_PATTERN_LEN] = {};

	/* for now use only 'ieee80211_ops' as reference */
	ops = director.ieee80211_ops;
	if(ops == NULL){
		printf("[!] ieee80211_ops is NULL!\n");
		ret = -EINVAL;
		return ret;
	}

	local = director.ieee80211_local;
	if(local == NULL || local->ieee80211_hw == NULL){
		printf("[!] local->ieee80211_hw is NULL!\n");
		ret = -EINVAL;
		return ret;
	}

	if(current_op == IEEE80211_START){
		start = ops->start;
		if(start != NULL){
			ret = start(local->ieee80211_hw);
		}
	}
	if(current_op == IEEE80211_STOP){
		ret = wifi_connection_stop(local);
		if (ret)
			return ret;
		stop = ops->stop;
		if(stop != NULL){
			stop(local->ieee80211_hw, false);
		}
	}
	if(current_op == IEEE80211_SUSPEND){
		suspend = ops->suspend;
		if(suspend != NULL){
			if(director.cfg80211_wowlan == NULL){
				ret = build_wowlan_magic_pattern(local, &wowlan_pattern, wowlan_packet, sizeof(wowlan_packet));
				if(ret != 0){
					return ret;
				}
				ret = fill_cfg80211_wowlan(WOWLAN_MAGIC_PKT, &wowlan_pattern, 1, NULL);
				if(ret != 0){
					return ret;
				}
			}
			ret = suspend(local->ieee80211_hw, director.cfg80211_wowlan);
		}
	}
	if(current_op == IEEE80211_ADD_INTERFACE){
		add_interface = ops->add_interface;
		if(add_interface != NULL){
			/* verify that local->ieee80211_vif isn't allocated or initialized */
			if(local->ieee80211_vif == NULL){
				if(director.struct_sizes.ieee80211_vif == 0){
					printf("[!] invalid size of ieee80211_vif!\n");
					ret = -EINVAL;
					return ret;
				}
				ieee80211_hw = local->ieee80211_hw;
				local->ieee80211_vif = calloc(1, director.struct_sizes.ieee80211_vif + ieee80211_hw->vif_data_size);
				ieee80211_vif = local->ieee80211_vif;
				ieee80211_vif->type = NL80211_IFTYPE_STATION;
				memset(&ieee80211_vif->bss_conf, 0, director.struct_sizes.ieee80211_bss_conf);
				bss_conf = &ieee80211_vif->bss_conf;
				bss_conf->vif = ieee80211_vif;
				bss_conf->link_id = 0;
				ether_addr_copy(bss_conf->addr, ieee80211_vif->addr);
				bss_conf->bssid = bss_conf->addr;
				for (u8 ac = 0; ac < IEEE80211_NUM_ACS; ac++){
					ieee80211_vif->hw_queue[ac] = ac;
				}
				ieee80211_vif->cab_queue = IEEE80211_INVAL_HW_QUEUE;
				rcu_assign_pointer(ieee80211_vif->link_conf[0], bss_conf);
				ieee80211_vif->valid_links = 0;
				ieee80211_vif->active_links = 0;
				ret = bss_conf_init(ieee80211_hw, ieee80211_vif);
				if (ret)
					return ret;
			}
			ret = add_interface(local->ieee80211_hw, local->ieee80211_vif);
		}
	}
	if(current_op == IEEE80211_REMOVE_INTERFACE){
		ret = wifi_connection_stop(local);
		if (ret)
			return ret;
		remove_interface = ops->remove_interface;
		if(remove_interface != NULL){
			if(local->ieee80211_vif == NULL){
				printf("[!] ieee80211_vif isn't initialized!\n");
				ret = -EINVAL;
				return ret;
			}
			remove_interface(local->ieee80211_hw, local->ieee80211_vif);
		}
	}
	if(current_op == IEEE80211_CONFIG){
		ret = handle_ieee80211_config(local->ieee80211_hw, ops);
	}
	if(current_op == IEEE80211_CONFIGURE_FILTER){
		ret = ieee80211_configure_rx_dump_filter(local, true);
	}
	if(current_op == IEEE80211_HW_SCAN){
		hw_scan = ops->hw_scan;
		cancel_hw_scan = ops->cancel_hw_scan;
		if(hw_scan != NULL && local->ieee80211_vif != NULL){
			ret = start_hw_scan(local, hw_scan, cancel_hw_scan);
			/* if the first scan is okay, start the scan thread */
			if(ret == 0){
				run_wifi_scan_thread();
			}
		}else{
			goto ieee80211_vif_not_initialized;
		}
	}
	if(current_op == IEEE80211_CANCEL_HW_SCAN){
		cancel_hw_scan = ops->cancel_hw_scan;
		if(cancel_hw_scan != NULL && local->ieee80211_vif != NULL){
			cancel_hw_scan(local->ieee80211_hw, local->ieee80211_vif);
		}else{
			goto ieee80211_vif_not_initialized;
		}
	}
	if(current_op == IEEE80211_START_AP){
		start_ap = ops->start_ap;
		/* initialize the AP interface */
		if(local->ieee80211_ap_vif == NULL){
			ieee80211_hw = local->ieee80211_hw;
			local->ieee80211_ap_vif = calloc(1, director.struct_sizes.ieee80211_vif + ieee80211_hw->vif_data_size);
			ieee80211_ap_vif = local->ieee80211_ap_vif;
			ieee80211_ap_vif->type = NL80211_IFTYPE_AP;
			memset(&ieee80211_ap_vif->bss_conf, 0, director.struct_sizes.ieee80211_bss_conf);
			bss_conf = &ieee80211_ap_vif->bss_conf;
			bss_conf->vif = ieee80211_ap_vif;
			bss_conf->link_id = 0;
			ether_addr_copy(bss_conf->addr, ieee80211_ap_vif->addr);
			bss_conf->bssid = bss_conf->addr;
			rcu_assign_pointer(ieee80211_ap_vif->link_conf[0], bss_conf);
			ieee80211_ap_vif->valid_links  |= BIT(0);
			ieee80211_ap_vif->active_links |= BIT(0);
			/* register the interface */
			add_interface = ops->add_interface;
			ret = add_interface(local->ieee80211_hw, local->ieee80211_ap_vif);
		}
		if(start_ap != NULL && local->ieee80211_ap_vif != NULL){
			if(bss_conf != NULL){
				free(bss_conf);
				bss_conf = NULL;
			}
			bss_conf = calloc(1, director.struct_sizes.ieee80211_bss_conf);
			fill_start_ap_req(bss_conf);
			ret = start_ap(local->ieee80211_hw, local->ieee80211_ap_vif, bss_conf);
		}else{
			goto ieee80211_vif_not_initialized;
		}
	}
	if(current_op == IEEE80211_STOP_AP){
		stop_ap = ops->stop_ap;
		if(stop_ap != NULL && local->ieee80211_ap_vif != NULL){
			if(bss_conf != NULL){
				free(bss_conf);
				bss_conf = NULL;
			}
			bss_conf = calloc(1, director.struct_sizes.ieee80211_bss_conf);
			fill_start_ap_req(bss_conf);
			stop_ap(local->ieee80211_hw, local->ieee80211_ap_vif, bss_conf);
		}else{
			goto ieee80211_vif_not_initialized;
		}
	}
	if(current_op == IEEE80211_MONITOR){
		add_interface = ops->add_interface;
		if(add_interface != NULL){
			/* verify that local->ieee80211_mon_vif isn't allocated or initialized */
			if(local->ieee80211_mon_vif == NULL){
				if(director.struct_sizes.ieee80211_vif == 0){
					printf("[!] invalid size of ieee80211_vif!\n");
					ret = -EINVAL;
					return ret;
				}
				ieee80211_hw = local->ieee80211_hw;
				local->ieee80211_mon_vif = calloc(1, director.struct_sizes.ieee80211_vif + ieee80211_hw->vif_data_size);
				ieee80211_mon_vif = local->ieee80211_mon_vif;
				ieee80211_mon_vif->type = NL80211_IFTYPE_MONITOR;
				memset(&ieee80211_mon_vif->bss_conf, 0, director.struct_sizes.ieee80211_bss_conf);
				bss_conf = &ieee80211_mon_vif->bss_conf;
				bss_conf->vif = ieee80211_mon_vif;
				bss_conf->link_id = 0;
				ether_addr_copy(bss_conf->addr, ieee80211_mon_vif->addr);
				bss_conf->bssid = bss_conf->addr;
				rcu_assign_pointer(ieee80211_mon_vif->link_conf[0], bss_conf);
				ieee80211_mon_vif->valid_links  |= BIT(0);
				ieee80211_mon_vif->active_links |= BIT(0);
			}
			ret = add_interface(local->ieee80211_hw, local->ieee80211_mon_vif);
		}
	}
	return ret;
	ieee80211_vif_not_initialized:
	printf("[!] ieee80211_vif is not initialized!\n");
	ret = -EINVAL;
	return ret;
}

/* elf_initialize_driver: used inside the tcc scripts for the standalone.elf for initializing the driver, calls 'start', 'add_interface' and so on.. */
int elf_initialize_driver(void){
	int ret = 0;

	/* decide if use 'ieee80211_ops' or 'cfg80211_ops' */
	if(director.ieee80211_ops != NULL){
		/* Start hardware before invoking callbacks which access channel state. */
		fast_run_ieee80211_cb(IEEE80211_START);
		/* invoke the 'add_interface' callback */
		fast_run_ieee80211_cb(IEEE80211_ADD_INTERFACE);
		/* invoke the 'config' callback */
		fast_run_ieee80211_cb(IEEE80211_CONFIG);
		/* invoke the 'configure_filter' callback */
		fast_run_ieee80211_cb(IEEE80211_CONFIGURE_FILTER);
		/* invoke a preliminary 'hw_scan' */
		fast_run_ieee80211_cb(IEEE80211_HW_SCAN);
	}else{
		/* invoke the 'ndo_validate_addr' callback */
		fast_run_cfg80211_cb(NDO_VALIDATE_ADDR);
		/* invoke the 'ndo_open' callback */
		fast_run_cfg80211_cb(NDO_OPEN);
		/* invoke the 'ndo_set_rx_mode' callback */
		fast_run_cfg80211_cb(NDO_SET_RX_MODE);
		/* invoke the 'scan' callback */
		fast_run_cfg80211_cb(CFG80211_SCAN);
	}
	return ret;
}

#ifdef __linux__
struct so_context {
	void *sym_addr;
	unsigned char **sym_name;
};

/* walk_shared_libraries: callback used for walking through the loaded libraries */
int walk_shared_libraries(struct dl_phdr_info *info, size_t info_size, void *data) {
        int ret = 0;
	uintptr_t min = 0;
	uintptr_t max = 0;
	uintptr_t seg_hi = 0;
	uintptr_t seg_lo = 0;
	u32 *sym_cnt = NULL;
	void *sym_addr = NULL;
	Elf(Dyn) *dyn = NULL;
	Elf(Phdr) *ph = NULL;
	Elf(Sym) *symtab = NULL;
	Elf(Sym) *tmp_sym = NULL;
	unsigned char *strtab = NULL;
	struct so_context *so_ctx = (struct so_context *)data;

	sym_addr = so_ctx->sym_addr;
	if(sym_addr == NULL){
		ret = -EINVAL;
		return ret;
	}

	min = UINTPTR_MAX;

	for (ElfW(Half) i = 0; i < info->dlpi_phnum; i++) {
		ph = &info->dlpi_phdr[i];
		if (ph->p_type != PT_LOAD){
			continue;
		}
		seg_lo = info->dlpi_addr + ph->p_vaddr;
		seg_hi = seg_lo + ph->p_memsz;

		if (seg_lo < min){
			min = seg_lo;
		}
		if (seg_hi > max){
			max = seg_hi;
		}
	}
	if(min == UINTPTR_MAX){
		min = 0;
		max = 0;
	}

	for (int i = 0; i < info->dlpi_phnum; i++){
		ph = &info->dlpi_phdr[i];
		if (ph->p_type == PT_DYNAMIC) {
			dyn = (Elf(Dyn) *)(uintptr_t)(info->dlpi_addr + ph->p_vaddr);
			break;
		}
	}

	if (dyn == NULL){
		printf("[!] dynamic segment not found!\n");
		return ret;
	}

	for (Elf(Dyn) *d = dyn; d->d_tag != DT_NULL; d++) {
		switch(d->d_tag){
			case DT_SYMTAB:
				if(d->d_un.d_ptr >= min && d->d_un.d_ptr < max){
					symtab = (Elf(Sym *))(uintptr_t)(d->d_un.d_ptr);
				}else{
					symtab = (Elf(Sym *))(uintptr_t)(info->dlpi_addr + d->d_un.d_ptr);
				}
			break;
			case DT_STRTAB:
				if(d->d_un.d_ptr >= min && d->d_un.d_ptr < max){
					strtab = (unsigned char *)(d->d_un.d_ptr);
				}else{
					strtab = (unsigned char *)(info->dlpi_addr + d->d_un.d_ptr);
				}
			break;
			case DT_HASH:
				if(d->d_un.d_ptr >= min && d->d_un.d_ptr < max){
					sym_cnt = (u32 *)(d->d_un.d_ptr);
				}else{
					sym_cnt = (u32 *)(info->dlpi_addr + d->d_un.d_ptr);
				}
			break;
			default:
			break;
		}
	}

	if(symtab == NULL || strtab == NULL || sym_cnt == NULL){
		return ret;
	}

	for (int i = 0; i < sym_cnt[1]; i++) {
		tmp_sym = &symtab[i];

		if (tmp_sym->st_shndx != SHN_UNDEF) {
			if((info->dlpi_addr + tmp_sym->st_value) <= sym_addr && sym_addr < (info->dlpi_addr + tmp_sym->st_value + tmp_sym->st_size)){
				*so_ctx->sym_name = (unsigned char *)(strtab + tmp_sym->st_name);
				return ret;
			}
		}
	}
	return ret;
}

/* fast_get_name_in_so: search the name of a symbol by walking in the shared objects, used mainly for the backtrace subsystem */
int fast_get_name_in_so(void *sym_addr, unsigned char **sym_name){
	int ret = 0;
	struct so_context shared_object_context = {
		.sym_addr = sym_addr,
		.sym_name = sym_name,
	};

	dl_iterate_phdr(walk_shared_libraries, &shared_object_context);
	return ret;
}
#endif

/* fast_execute_function: given a function name, search it inside the loaded modules in memory and execute it */
int fast_execute_function(unsigned char *function_name){
	int ret = 0;
	u8 found = false;
	int (LINUX_ABI *fn)(void) = NULL;
	unsigned char *ptr = NULL;
	unsigned char *current_function_name = NULL;

	/* call the function */
	for (unsigned int i = 0; i < SYM_ENTRIES; i++) {
		current_function_name = sym_table[i].function_name;

		if (current_function_name == NULL || current_function_name[0] == '\0'){
			continue;
		}
		if (strcmp(current_function_name, function_name) == 0) {
			found = true;
			fn = (int (LINUX_ABI *)(void))(uintptr_t)sym_table[i].function_ptr;
			ptr = (unsigned char *)fn;
			if (ptr[0] == 0xE8) {
				int32_t disp = *(int32_t *)(ptr + 1);
				void   *tgt  = ptr + 5 + disp;
			}

			/* mark the module execution */
			printf("--------------------------\n");
			ret = fn();
			printf("%s returned %d\n", function_name, ret);
		}
	}
	if(found == false){
		printf("[!] failed to execute function '%s'\n", function_name);
	}
	return ret;
}

/* utilities functions used for the cmd shell */
static int drv_parse_line(char *line, char **drv_arg, int max_args) {
	int   ret   = 0;
	char *token = NULL;
	token       = strtok(line, " \t\r\n");

	while (token && ret < max_args - 1) {
		drv_arg[ret++] = token;
		token            = strtok(NULL, " \t\r\n");
	}

	drv_arg[ret] = NULL;
	return ret;
}

/* drv_execute_cmd: given the callback name, get the associated function pointer from a callback table and invoke it */
static int drv_execute_cmd(struct module_controller *director, char *argv[], struct cmd_table_entry *dispatch_cmd_table) {
	int ret      = 0;
	int num_args = -1;

	/* basic sanity check */
	if (argv[0] == NULL || dispatch_cmd_table == NULL) {
		ret = -EINVAL;
		return ret;
	}

	/* get the total args for the cmd */
	for (u8 j = 0; argv[j] != NULL; j++) {
		num_args++;
	}

	/* compare both the cmd name and it's args */
	if (strcmp(argv[0], (const char *)dispatch_cmd_table->cmd_name) == 0) {
		/* special option: if cmd_args is set to '0xFF', bypass this
		 * parameter
		 */
		if (dispatch_cmd_table->cmd_args == 0xFF) {
			ret = dispatch_cmd_table->cmd_ptr(director, argv, dispatch_cmd_table->cmd_args);
			return ret;
		}
		/* execute the command with the correct parameters check */
		if (num_args == dispatch_cmd_table->cmd_args) {
			ret = dispatch_cmd_table->cmd_ptr(director, argv, dispatch_cmd_table->cmd_args);
			return ret;
		}
	}

	ret = -EDOM;
	return ret;
}

/*
 *      Main commands used by the cmd shell.
 */
int drv_cmd_exit(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;

	finalize_director();
	exit(ret);
}

int drv_cmd_reset(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;

	return ret;
}

/* drv_cmd_load: given a .ko name, load it into memory, this permits to load muultiple kernel modules into the process memory without executing them */
int drv_cmd_load(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;
	unsigned char *module_name = NULL;
	struct load_info *load_info = NULL;

	if(argv[1] == NULL){
		printf("> load <module.ko>!\n");
		ret = -EINVAL;
		return ret;
	}

	module_name = argv[1];
	if(strlen(module_name) < MODULE_INPUT_MIN_LEN){
		printf("[!] module %s too short!\n", module_name);
		ret = -EINVAL;
		return ret;
	}

	ret = initialize_director(director, &load_info);
	if (ret) {
		printf("[!] failed to initialize the main context!\n");
		return ret;
	}

	/* now call the main driver loading function */
	ret = module_init(module_name, NULL, load_info);
	return ret;
}

/* drv_cmd_unload: the opposite of 'load', given a kernel module name, unload it from the system */
int drv_cmd_unload(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;
	unsigned char *module_name = NULL;

	if(argv[1] == NULL){
		printf("> unload <module.ko>!\n");
		ret = -EINVAL;
		return ret;
	}

	return ret;
}

static int drv_resolve_driver_init(struct module_controller *director, unsigned char **resolved_name){
	int ret = 0;
	int name_len = 0;
	int suffix_len = 0;
	unsigned char *name = NULL;

	ret = -ENOENT;
	suffix_len = strlen("_driver_init");

	if(director == NULL){
		ret = -EINVAL;
		return ret;
	}

	director->driver_init_function = NULL;
	if(resolved_name != NULL){
		*resolved_name = NULL;
	}

	/* Keep the last matching initializer: modules are appended in load order,
	 * and the bus-specific driver module is normally loaded last. */
	for(u16 i = 0; i < SYM_ENTRIES; i++){
		name = sym_table[i].function_name;
		name_len = 0;

		if(name[0] == '\0' || sym_table[i].function_ptr == NULL){
			continue;
		}
		name_len = strnlen(name, sizeof(sym_table[i].function_name));
		if(name_len == sizeof(sym_table[i].function_name) || name_len <= suffix_len ||
		   strncmp(name, "__pfx_", 6) == 0 || strcmp(name + name_len - suffix_len, "_driver_init") != 0){
			continue;
		}

		director->driver_init_function = (int (LINUX_ABI *)(void))(uintptr_t)sym_table[i].function_ptr;
		if(resolved_name != NULL){
			*resolved_name = name;
		}
		ret = 0;
	}

	return ret;
}

/* drv_cmd_start: invoke the resolved kernel module init function and chain with the cfg80211/mac80211 callback invocation.
 * mac80211/cfg80211 callbacks are intentionally left to explicit ops commands,
 * since some USB drivers need a staged init before ops->start is safe.
 */
int drv_cmd_start(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;

	if(director->init_cb == NULL){
		ret = -EINVAL;
		return ret;
	}

	ret = director->init_cb();
	if(ret < 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to call the 'init_cb'!\n");
		#endif
		return ret;
	}
	/*
	 * this is due to a limitation present in many usb Wi-Fi dongle: usually they follows the multi-state usb schema, this means that they will detach
	 * and re-attach to the host system with the same or a different usb vid/pid pair, for avoiding any problem, add a tweak here where it is checked
	 * if the usb device is connected to the host system or not, if not, invoke again the 'probe' function of the kernel driver loaded in memory.
	 */
	ret = check_usb_presence();
	if(ret < 0){
		/* set a wait time of 5 seconds and then invoke again the 'init_cb' callback */
		usleep(5000000);
		ret = director->init_cb();
		if(ret < 0){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] failed to call the 'init_cb' during the second stage of the driver!\n");
			#endif
			return ret;
		}
	}

	ret = elf_initialize_driver();
	return ret;
}

/* drv_cmd_stop: stop the driver execution, different from the unload function */
int drv_cmd_stop(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;

	return ret;
}

/* drv_cmd_run: given a kernel module name registered in the system and the init function, run it,
 * this permits to loads different symbols from every loaded driver into the memory */
int drv_cmd_run(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;
	u8 found = false;
	void *tgt = NULL;
	int (LINUX_ABI *fn)(void) = NULL;
	unsigned char *ptr = NULL;
	unsigned char *module_name = NULL;
	unsigned char *module_function = NULL;
	unsigned char *current_function_name = NULL;

	if(argv[1] == NULL){
		printf("> run <function_name>!\n");
		ret = -EINVAL;
		return ret;
	}
	/* support also the format <kernel_object_name(.ko)> <function_name> */
	if(argv[2] != NULL){
		module_name = argv[1];
		module_function = argv[2];
	}else{
		module_function = argv[1];
	}

	/* call the function */
	for (unsigned int i = 0; i < SYM_ENTRIES; i++) {
		current_function_name = sym_table[i].function_name;

		if (current_function_name[0] == '\0'){
			continue;
		}
		if (strcmp((char *)module_function, current_function_name) == 0) {
			found = true;
			fn = (int (LINUX_ABI *)(void))(uintptr_t)sym_table[i].function_ptr;
			ptr = (unsigned char *)fn;
			if (ptr[0] == 0xE8) {
				s32 disp = *(s32 *)(ptr + 1);
				tgt  = ptr + 5 + disp;
			}

			/* mark the module execution */
			printf("--------------------------\n");
			#ifdef ENABLE_DBG_PRINT
			printf("[*] function at: 0x%08x\n", fn);
			#endif
			ret = fn();
			printf("%s returned %d\n", module_function, ret);
		}
	}
	if(found == false){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to execute function '%s'\n", module_function);
		#endif
	}
	return ret;
}

/* drv_cmd_set: set a context variable to a specific value, used for changing the runtime of the driver executor */
int drv_cmd_set(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;
	unsigned char *set_req = NULL;

	if(argv[1] == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("> set <req>\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	set_req = argv[1];

	#ifdef TCC_SCRIPTING
	if(strstr(set_req, "tcc") != NULL || strstr(set_req, "script") != NULL){
		/* enter in tcc scripting mode */
		drv_scripting_loop();
	}
	#endif
	return ret;
}

/* drv_cmd_get: permits to get data from the program */
int drv_cmd_get(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;
	unsigned char *get_req = NULL;

	if(argv[1] == NULL){
		printf("> get <req>\n");
		ret = -EINVAL;
		return ret;
	}

	get_req = argv[1];

	/* list every symbols in the current loaded system */
	if(strstr(get_req, "sym") != NULL){
		for(u32 sym_idx = 0; sym_table[sym_idx].function_ptr != NULL; sym_idx++){
			printf("sym %d name: %s address: 0x%08x", sym_idx, sym_table[sym_idx].function_name, sym_table[sym_idx].function_ptr);
			if(sym_table[sym_idx].function_size != 0){
				printf(" size: %d", sym_table[sym_idx].function_size);
			}
			printf("\n");
		}
	}
	/* get the current allocated memory number */
	if(strstr(get_req, "mem") != NULL){
		printf("[*] bytes used before module initialization: %d bytes\n", director->total_memory_used);
		dump_memory_allocation_list(director->module_allocation_list);
	}
	/* get the ELF/PE file dump */
	if(strstr(get_req, "exec") != NULL || strstr(get_req, "elf") != NULL || strstr(get_req, "pe") != NULL){
		#ifdef __linux__
		elf_generator_handler();
		#else
		pe_generator_handler();
		#endif
	}

	/* dump the structs and offsets of the actual loaded lkm */
	if(strstr(get_req, "conf") != NULL){
		ret = open("offsets.conf", O_RDWR | O_CREAT, 0777);
		lseek(ret, 0, SEEK_SET);
		unsigned char line[48] = {};

		for (int i = 0; ; i++) {
			const struct runtime_struct_layout *layout = &runtime_generated_structs.structs[i];

			if (layout == NULL || layout->struct_name == NULL){
				break;
			}
			if(layout->struct_size == 0){
				continue;
			}
			memset(line, 0, sizeof(line));
			snprintf(line, sizeof(line), "%s\n", layout->struct_name);
			write(ret, line, strlen(line));
			memset(line, 0, sizeof(line));
			snprintf(line, sizeof(line), "$size: %d\n", layout->struct_size);
			write(ret, line, strlen(line));

			for (int j = 0; j < layout->struct_field_num; j++) {
				memset(line, 0, sizeof(line));
				snprintf(line, sizeof(line), "%s: %u\n", layout->fields[j].field_name, layout->fields[j].field_offset);
				write(ret, line, strlen(line));
			}
			memset(line, 0, sizeof(line));
			write(ret, "\n", 1);
		}
		close(ret);
		ret = 0;
	}
	return ret;
}

/* handle_ieee80211_conf_tx: the 'conf_tx' callback in ieee80211_ops uses different prototypes based on kernel version, handle this */
int handle_ieee80211_conf_tx(struct ieee80211_hw *ieee80211_hw, struct ieee80211_vif *ieee80211_vif, struct ieee80211_ops *ops){
	int ret = 0;
	struct ieee80211_tx_queue_params *params = NULL;
	/*
	 * if the Linux kernel is > 6.0 use this prototype:
	 * 'int (*conf_tx)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, unsigned int link_id, u16 ac, const struct ieee80211_tx_queue_params *params);
	 * instead, if is <= 5.19 the prototype is:
	 * int (*conf_tx)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, u16 ac, const struct ieee80211_tx_queue_params *params);
	 */
	params = calloc(1, sizeof(struct ieee80211_tx_queue_params));
	if(director.kernel_major_version >= 6){
		int (*conf_tx)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, unsigned int link_id, u16 ac, struct ieee80211_tx_queue_params *params) = NULL;
		conf_tx = ops->conf_tx;
		if(conf_tx == NULL){
			ret = -EINVAL;
			return ret;
		}
		conf_tx(ieee80211_hw, ieee80211_vif, 0, 0, params);
	}else{
		int (*conf_tx)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, u16 ac, const struct ieee80211_tx_queue_params *params) = NULL;
		conf_tx = ops->conf_tx;
		if(conf_tx == NULL){
			ret = -EINVAL;
			return ret;
		}
		conf_tx(ieee80211_hw, ieee80211_vif, 0, params);
	}
	free(params);
	return ret;
}

/* handle_ieee80211_config: the 'configure' callback in ieee80211_ops uses different prototypes based on kernel version, handle this */
int handle_ieee80211_config(struct ieee80211_hw *ieee80211_hw, struct ieee80211_ops *ops){
	int ret = 0;
	struct cfg80211_chan_def *cfg80211_chan_def = NULL;
	struct ieee80211_local *local = NULL;

	if (ieee80211_hw == NULL || ops == NULL){
		ret = -EINVAL;
		return ret;
	}
	if(ops->add_chanctx != NULL){
		return ret;
	}
	local = director.ieee80211_local;
	if(local != NULL){
		cfg80211_chan_def = local->dflt_chandef;
	}
	if (cfg80211_chan_def == NULL || cfg80211_chan_def->chan == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[*] ops config: no channel selected; skipping channel change\n");
		#endif
		return ret;
	}

	if(director.kernel_major_version > 6 || (director.kernel_major_version == 6 && director.kernel_minor_version >= 17)){
		int (*config)(struct ieee80211_hw *hw, int radio_idx, u32 changed) = NULL;
		config = ops->config;
		if(config != NULL){
			ret = config(ieee80211_hw, -1, IEEE80211_CONF_CHANGE_CHANNEL);
		}
	}else{
		int (*config)(struct ieee80211_hw *hw, u32 changed) = NULL;
		config = ops->config;
		if(config != NULL){
			ret = config(ieee80211_hw, IEEE80211_CONF_CHANGE_CHANNEL);
		}
	}
	return ret;
}

/* drv_cmd_ops: permits to invoke various 'ops' struct registered by running the loaded kernel module, like 'ieee80211_ops' */
int drv_cmd_ops(struct module_controller *__director, char *argv[], int num_args) {
        int ret = 0;
	int intf_type = 0;
	bool enable = false;
	struct wiphy *wiphy = NULL;
	void *new_mac_address = NULL;
        unsigned char *ops_req = NULL;
	unsigned char *ap_ssid = NULL;
	unsigned char *curr_intf = NULL;
	struct ieee80211_ops *ops = NULL;
	struct cfg80211_ops *cfg_ops = NULL;
	struct ieee80211_local *local = NULL;
	struct net_device *net_device = NULL;
	struct wireless_dev *wireless_dev = NULL;
	struct ieee80211_hw *ieee80211_hw = NULL;
	struct ieee80211_vif_cfg *vif_cfg = NULL;
	struct netdev_hw_addr_list *mc_list = NULL;
	struct ieee80211_bss_conf *bss_conf = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;
	struct ieee80211_vif *ieee80211_ap_vif = NULL;
	struct ieee80211_vif *ieee80211_mon_vif = NULL;
	struct cfg80211_ap_settings *ap_settings = NULL;
	/* callbacks prototypes for mac80211 */
	int (*start)(struct ieee80211_hw *hw) = NULL;
	void (*stop)(struct ieee80211_hw *hw, bool suspend) = NULL;
	int (*suspend)(struct ieee80211_hw *hw, struct cfg80211_wowlan *wowlan) = NULL;
	int (*add_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	void (*cancel_hw_scan)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	void (*remove_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	u64 (*prepare_multicast)(struct ieee80211_hw *hw, struct netdev_hw_addr_list *mc_list) = NULL;
	void (*sw_scan_start)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, const u8 *mac_addr) = NULL;
	int (*hw_scan)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_scan_request *req) = NULL;
	int (*start_ap)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_bss_conf *link_conf) = NULL;
	void (*stop_ap)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_bss_conf *link_conf) = NULL;
	int (*ampdu_action)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_ampdu_params *params) = NULL;
	void (*configure_filter)(struct ieee80211_hw *hw, unsigned int changed_flags, unsigned int *new_flags, u64 multicast) = NULL;
	/* callbacks prototypes for cfg80211 */
	void (*abort_scan)(struct wiphy *wiphy, struct wireless_dev *wdev) = NULL;
	int (*del_virtual_intf)(struct wiphy *wiphy, struct wireless_dev *wdev) = NULL;
	int (*testmode_cmd)(struct wiphy *wiphy, struct wireless_dev *wdev, void *data, int len) = NULL;
	int (*cfg80211_stop_ap)(struct wiphy *wiphy, struct net_device *dev, unsigned int link_id) = NULL;
	int (*auth)(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_auth_request *req) = NULL;
	int (*assoc)(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_assoc_request *req) = NULL;
	int (*deauth)(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_deauth_request *req) = NULL;
	int (*cfg80211_start_ap)(struct wiphy *wiphy, struct net_device *dev, struct cfg80211_ap_settings *settings) = NULL;
	int (*testmode_dump)(struct wiphy *wiphy, struct sk_buff *skb, struct netlink_callback *cb, void *data, int len) = NULL;
	int (*change_virtual_intf)(struct wiphy *wiphy, struct net_device *dev, enum nl80211_iftype type, struct vif_params *params) = NULL;
	struct wireless_dev *(*add_virtual_intf)(struct wiphy *wiphy, const char *name, unsigned char name_assign_type, enum nl80211_iftype type, struct vif_params *params) = NULL;

        /* additional wowlan structs */
	struct cfg80211_pkt_pattern wowlan_pattern = {};
	u8 wowlan_packet[WOWLAN_MAGIC_PATTERN_LEN] = {};
	struct vif_params params = {
		.use_4addr = -1
	};

	if(argv[1] == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] ops <callback name>\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	ops_req = argv[1];

	ops = __director->ieee80211_ops;
	if(ops == NULL){
		cfg_ops = __director->cfg80211_ops;
		if(cfg_ops == NULL){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] ieee80211 and cfg80211 callbacks are NULL!\n");
			#endif
			ret = -EINVAL;
			return ret;
		}

		wiphy = __director->wiphy;
		if(wiphy == NULL && __director->rdev != NULL){
			wiphy = __director->rdev->wiphy;
		}
		net_device = __director->net_device;
		if(wiphy == NULL || net_device == NULL){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] cfg80211 wiphy or net_device is NULL!\n");
			#endif
			ret = -EINVAL;
			return ret;
		}
		__director->wiphy = wiphy;
		wireless_dev = net_device->ieee80211_ptr;
		goto cfg80211_ops;
	}

	local = __director->ieee80211_local;
	if(local == NULL || local->ieee80211_hw == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] local->ieee80211_hw is NULL!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	mac80211_ops:
	if(strcmp(ops_req, "start") == 0){
		start = ops->start;
		if(start != NULL){
			ret = start(local->ieee80211_hw);
			return ret;
		}
	}
	if(strcmp(ops_req, "stop") == 0){
		ret = wifi_connection_stop(local);
		if (ret)
			return ret;
		stop = ops->stop;
		if(stop != NULL){
			stop(local->ieee80211_hw, false);
		}
	}
	if(strcmp(ops_req, "suspend") == 0){
		suspend = ops->suspend;
		if(suspend != NULL){
			if(director.cfg80211_wowlan == NULL){
				ret = build_wowlan_magic_pattern(local, &wowlan_pattern, wowlan_packet, sizeof(wowlan_packet));
				if(ret != 0){
					return ret;
				}
				ret = fill_cfg80211_wowlan(WOWLAN_MAGIC_PKT, &wowlan_pattern, 1, NULL);
				if(ret != 0){
					return ret;
				}
			}
			ret = suspend(local->ieee80211_hw, director.cfg80211_wowlan);
		}
	}
	if(strcmp(ops_req, "add_interface") == 0){
		curr_intf = argv[2];
		add_interface = ops->add_interface;
		if(add_interface != NULL){
			/* verify that local->ieee80211_vif isn't allocated or initialized */
			if(local->ieee80211_vif == NULL){
				if(director.struct_sizes.ieee80211_vif == 0){
					#ifdef ENABLE_DBG_PRINT
					printf("[!] invalid size of ieee80211_vif!\n");
					#endif
					ret = -EINVAL;
					return ret;
				}
				ieee80211_hw = local->ieee80211_hw;
				local->ieee80211_vif = calloc(1, director.struct_sizes.ieee80211_vif + ieee80211_hw->vif_data_size);
				ieee80211_vif = local->ieee80211_vif;
				if(curr_intf == NULL){
					ieee80211_vif->type = NL80211_IFTYPE_STATION;
				}else{
					if(strstr(curr_intf, "sta") != NULL){
						ieee80211_vif->type = NL80211_IFTYPE_STATION;
					}
					if(strstr(curr_intf, "ap") != NULL){
						ieee80211_vif->type = NL80211_IFTYPE_AP;
					}
					if(strstr(curr_intf, "ocb") != NULL){
						ieee80211_vif->type = NL80211_IFTYPE_OCB;
					}
					if(strstr(curr_intf, "hoc") != NULL){
						ieee80211_vif->type = NL80211_IFTYPE_ADHOC;
					}
					if(strstr(curr_intf, "wds") != NULL){
						ieee80211_vif->type = NL80211_IFTYPE_WDS;
					}
					if(strstr(curr_intf, "monitor") != NULL){
						ieee80211_vif->type = NL80211_IFTYPE_MONITOR;
					}
					if(strstr(curr_intf, "mesh") != NULL){
						ieee80211_vif->type = NL80211_IFTYPE_MESH_POINT;
					}
					if(strstr(curr_intf, "nan") != NULL){
						ieee80211_vif->type = NL80211_IFTYPE_NAN;
					}
				}
				memset(&ieee80211_vif->bss_conf, 0, director.struct_sizes.ieee80211_bss_conf);
				bss_conf = &ieee80211_vif->bss_conf;
				bss_conf->vif = ieee80211_vif;
				bss_conf->link_id = 0;
				memcpy(ieee80211_vif->addr, (u8[]){0x02, 0x00, 0x00, 0x52, 0xc0, 0x01}, ETH_ALEN);
				ether_addr_copy(bss_conf->addr, ieee80211_vif->addr);
				bss_conf->bssid = bss_conf->addr;
				for (u8 ac = 0; ac < IEEE80211_NUM_ACS; ac++){
					ieee80211_vif->hw_queue[ac] = ac;
				}
				ieee80211_vif->cab_queue = IEEE80211_INVAL_HW_QUEUE;
				rcu_assign_pointer(ieee80211_vif->link_conf[0], bss_conf);
				ieee80211_vif->valid_links = 0;
				ieee80211_vif->active_links = 0;
				ret = bss_conf_init(ieee80211_hw, ieee80211_vif);
				if (ret)
					return ret;
			}
			ret = add_interface(local->ieee80211_hw, local->ieee80211_vif);
		}
	}
	if(strcmp(ops_req, "remove_interface") == 0){
		ret = wifi_connection_stop(local);
		if (ret)
			return ret;
		remove_interface = ops->remove_interface;
		if(remove_interface != NULL){
			if(local->ieee80211_vif == NULL){
				#ifdef ENABLE_DBG_PRINT
				printf("[!] ieee80211_vif isn't initialized!\n");
				#endif
				ret = -EINVAL;
				return ret;
			}
			remove_interface(local->ieee80211_hw, local->ieee80211_vif);
		}
	}
	if(strcmp(ops_req, "config") == 0){
		/* the 'config' callback changes from Linux version 6 to version 7, so handle it in a separate function */
		ret = handle_ieee80211_config(local->ieee80211_hw, ops);
	}
	if(strcmp(ops_req, "configure_filter") == 0){
		ret = ieee80211_configure_rx_dump_filter(local, true);
	}
	if(strcmp(ops_req, "connect") == 0){
		if(num_args < 3 || argv[2] == NULL){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] connect: ops connect <ssid> [password]\n");
			#endif
			ret = -EINVAL;
			return ret;
		}
		stop_wifi_scan_thread();
		ret = wifi_auth_connection_start(local, argv[2], num_args >= 4 ? argv[3] : NULL);
	}
	if(strcmp(ops_req, "disconnect") == 0){
		return wifi_connection_stop(local);
	}
	if(strcmp(ops_req, "rx") == 0){
		enable = true;

		if(num_args >= 3 && argv[2] != NULL){
			if(strcmp(argv[2], "off") == 0 || strcmp(argv[2], "0") == 0){
				enable = false;
			}else if(strcmp(argv[2], "on") != 0 && strcmp(argv[2], "1") != 0){
				#ifdef ENABLE_DBG_PRINT
				printf("[!] rx: ops rx [on|off|status]\n");
				#endif
				ret = -EINVAL;
				return ret;
			}
		}
		ret = ieee80211_configure_rx_dump_filter(local, enable);
		atomic_store(&__director->rx_dump_enabled, enable);
		if(enable){
			atomic_store(&__director->rx_dump_sequence, 0);
		}
		#ifdef ENABLE_DBG_PRINT
		printf("[*] RX hexdump %s\n", enable ? "enabled" : "disabled");
		#endif
	}
	if(strcmp(ops_req, "conf_tx") == 0){
		/* handle in the same way of 'config' */
		ret = handle_ieee80211_conf_tx(local->ieee80211_hw, local->ieee80211_vif, ops);
	}
	if(strcmp(ops_req, "prepare_multicast") == 0){
		prepare_multicast = ops->prepare_multicast;
		if(prepare_multicast != NULL){
			if(mc_list == NULL){
				mc_list = calloc(1, sizeof(struct netdev_hw_addr_list));
			}
			ret = prepare_multicast(local->ieee80211_hw, mc_list);
		}
	}
	if(strcmp(ops_req, "sta_add") == 0){

	}
	if(strcmp(ops_req, "sta_remove") == 0){

	}
	if(strcmp(ops_req, "scan") == 0 || strcmp(ops_req, "hw_scan") == 0){
		hw_scan = ops->hw_scan;
		cancel_hw_scan = ops->cancel_hw_scan;
		if(hw_scan != NULL && local->ieee80211_vif != NULL){
			ret = start_hw_scan(local, hw_scan, cancel_hw_scan);
			/* if the first scan was fine, create the scan thread */
			if(ret == 0){
				run_wifi_scan_thread();
			}
		}else{
			goto ieee80211_vif_not_initialized;
		}
	}
	if(strcmp(ops_req, "sw_scan") == 0){
		sw_scan_start = ops->sw_scan_start;
		if(sw_scan_start != NULL){
			sw_scan_start(local->ieee80211_hw, local->ieee80211_vif, NULL);
		}
	}
	if(strcmp(ops_req, "cancel_scan") == 0){
		cancel_hw_scan = ops->cancel_hw_scan;
		if(cancel_hw_scan != NULL && local->ieee80211_vif != NULL){
			cancel_hw_scan(local->ieee80211_hw, local->ieee80211_vif);
		}else{
			goto ieee80211_vif_not_initialized;
		}
	}
	if(strcmp(ops_req, "start_ap") == 0){
		start_ap = ops->start_ap;
		ap_ssid  = argv[2];
		if(ap_ssid != NULL){
			director.ap_ssid     = ap_ssid;
			director.ap_ssid_len = strlen(ap_ssid);
		}else{
			director.ap_ssid     = "test_ap";
			director.ap_ssid_len = sizeof("test_ap");
		}
		change_virtual_intf = cfg_ops->change_virtual_intf;
		/* initialize the AP interface */
		if(local->ieee80211_ap_vif == NULL){
			ieee80211_hw = local->ieee80211_hw;
			local->ieee80211_ap_vif = calloc(1, director.struct_sizes.ieee80211_vif + ieee80211_hw->vif_data_size);
			ieee80211_ap_vif = local->ieee80211_ap_vif;
			ieee80211_ap_vif->type = NL80211_IFTYPE_AP;
			memset(&ieee80211_ap_vif->bss_conf, 0, director.struct_sizes.ieee80211_bss_conf);
			bss_conf = &ieee80211_ap_vif->bss_conf;
			bss_conf->vif = ieee80211_ap_vif;
			bss_conf->link_id = 0;
			memcpy(ieee80211_ap_vif->addr, (u8[]){0x72, 0x88, 0xcb, 0xbc, 0x05, 0x4c}, ETH_ALEN);
			ether_addr_copy(bss_conf->addr, ieee80211_ap_vif->addr);
			bss_conf->bssid = bss_conf->addr;
			rcu_assign_pointer(ieee80211_ap_vif->link_conf[0], bss_conf);
			ieee80211_ap_vif->valid_links  |= BIT(0);
			ieee80211_ap_vif->active_links |= BIT(0);
			vif_cfg = &ieee80211_ap_vif->cfg;
			vif_cfg->assoc = true;
			vif_cfg->idle = false;
			strncpy(vif_cfg->ssid, director.ap_ssid, director.ap_ssid_len);
			vif_cfg->ssid_len = director.ap_ssid_len;
			ether_addr_copy(vif_cfg->ap_addr, ieee80211_ap_vif->addr);
			/* register the interface */
			add_interface = ops->add_interface;
			ret = add_interface(local->ieee80211_hw, ieee80211_ap_vif);
		}
		if(start_ap != NULL && local->ieee80211_ap_vif != NULL){
			ret = start_ap(local->ieee80211_hw, ieee80211_ap_vif, bss_conf);
		}else{
			goto ieee80211_vif_not_initialized;
		}
	}
	if(strcmp(ops_req, "stop_ap") == 0){
		stop_ap = ops->stop_ap;
		if(stop_ap != NULL && local->ieee80211_ap_vif != NULL){
			if(bss_conf != NULL){
				free(bss_conf);
				bss_conf = NULL;
			}
			bss_conf = calloc(1, director.struct_sizes.ieee80211_bss_conf);
			fill_start_ap_req(bss_conf);
			stop_ap(local->ieee80211_hw, local->ieee80211_ap_vif, bss_conf);
		}else{
			goto ieee80211_vif_not_initialized;
		}
	}
	/* monitor mode is a new virtual interface with the type == NL80211_IFTYPE_MONITOR */
	if(strcmp(ops_req, "monitor") == 0){
		add_interface = ops->add_interface;
		if(add_interface != NULL){
			/* verify that local->ieee80211_mon_vif isn't allocated or initialized */
			if(local->ieee80211_mon_vif == NULL){
                                if(director.struct_sizes.ieee80211_vif == 0){
					#ifdef ENABLE_DBG_PRINT
					printf("[!] invalid size of ieee80211_vif!\n");
					#endif
					ret = -EINVAL;
					return ret;
                                }
				ieee80211_hw = local->ieee80211_hw;
				local->ieee80211_mon_vif = calloc(1, director.struct_sizes.ieee80211_vif + ieee80211_hw->vif_data_size);
				ieee80211_mon_vif = local->ieee80211_mon_vif;
				ieee80211_mon_vif->type = NL80211_IFTYPE_MONITOR;
				memset(&ieee80211_mon_vif->bss_conf, 0, director.struct_sizes.ieee80211_bss_conf);
				bss_conf = &ieee80211_mon_vif->bss_conf;
				bss_conf->vif = ieee80211_mon_vif;
				bss_conf->link_id = 0;
				ether_addr_copy(bss_conf->addr, ieee80211_mon_vif->addr);
				bss_conf->bssid = bss_conf->addr;
				rcu_assign_pointer(ieee80211_mon_vif->link_conf[0], bss_conf);
				ieee80211_mon_vif->valid_links  |= BIT(0);
				ieee80211_mon_vif->active_links |= BIT(0);
			}
			ret = add_interface(local->ieee80211_hw, local->ieee80211_mon_vif);
		}
	}
	return ret;

	/* handle here the hardmac driver callbacks */
	cfg80211_ops:
	if(strcmp(ops_req, "add_interface") == 0){
		curr_intf = argv[2];
		add_virtual_intf = cfg_ops->add_virtual_intf;
		if(add_virtual_intf != NULL){
			if(curr_intf == NULL){
				intf_type  = NL80211_IFTYPE_STATION;
			}else{
				if(strstr(curr_intf, "sta") != NULL){
					intf_type = NL80211_IFTYPE_STATION;
				}
				if(strstr(curr_intf, "ap") != NULL){
					intf_type = NL80211_IFTYPE_AP;
				}
				if(strstr(curr_intf, "ocb") != NULL){
					intf_type = NL80211_IFTYPE_OCB;
				}
				if(strstr(curr_intf, "hoc") != NULL){
					intf_type = NL80211_IFTYPE_ADHOC;
				}
				if(strstr(curr_intf, "wds") != NULL){
					intf_type = NL80211_IFTYPE_WDS;
				}
				if(strstr(curr_intf, "monitor") != NULL){
					intf_type  = NL80211_IFTYPE_MONITOR;
				}
				if(strstr(curr_intf, "mesh") != NULL){
					intf_type  = NL80211_IFTYPE_MESH_POINT;
				}
				if(strstr(curr_intf, "nan") != NULL){
					intf_type = NL80211_IFTYPE_NAN;
				}
			}
			net_device = director.net_device;
			params.flags = 0;
			params.use_4addr = 0;
			if(net_device != NULL){
				memcpy(params.macaddr, director.net_device, ETH_ALEN);
			}else{
				memset(params.macaddr, 0xFF, ETH_ALEN);
			}
			wireless_dev = add_virtual_intf(wiphy, num_args >= 3 ? curr_intf : "wlan1", 0, intf_type, &params);
			if(IS_ERR(wireless_dev)){
				#ifdef ENABLE_DBG_PRINT
				printf("[!] failed to create a new wireless interface!\n");
				#endif
				ret = -EINVAL;
				return ret;
			}
			director.wdev = wireless_dev;
		}
	}
	if(strcmp(ops_req, "remove_interface") == 0){
		del_virtual_intf = cfg_ops->del_virtual_intf;
		if(del_virtual_intf != NULL && wireless_dev != NULL){
			ret = del_virtual_intf(wiphy, wireless_dev);
			return ret;
		}
	}
	if(strcmp(ops_req, "switch") == 0 || strcmp(ops_req, "change_interface") == 0){
		curr_intf = argv[2];
		change_virtual_intf = cfg_ops->change_virtual_intf;
		if(change_virtual_intf != NULL){
			if(curr_intf == NULL){
				intf_type  = NL80211_IFTYPE_STATION;
			}else{
				if(strstr(curr_intf, "sta") != NULL){
					intf_type = NL80211_IFTYPE_STATION;
				}
				if(strstr(curr_intf, "ap") != NULL){
					intf_type = NL80211_IFTYPE_AP;
				}
				if(strstr(curr_intf, "ocb") != NULL){
					intf_type = NL80211_IFTYPE_OCB;
				}
				if(strstr(curr_intf, "hoc") != NULL){
					intf_type = NL80211_IFTYPE_ADHOC;
				}
				if(strstr(curr_intf, "wds") != NULL){
					intf_type = NL80211_IFTYPE_WDS;
				}
				if(strstr(curr_intf, "monitor") != NULL){
					intf_type  = NL80211_IFTYPE_MONITOR;
				}
				if(strstr(curr_intf, "mesh") != NULL){
					intf_type  = NL80211_IFTYPE_MESH_POINT;
				}
				if(strstr(curr_intf, "nan") != NULL){
					intf_type = NL80211_IFTYPE_NAN;
				}
			}
			ret = change_virtual_intf(wiphy, net_device, intf_type, &params);
		}
	}
	/* simulate the 'net_device' open callback */
	if(strcmp(ops_req, "open") == 0){
		ret = netdevice_open(net_device);
		return ret;
	}
	if(strcmp(ops_req, "stop") == 0){
		ret = netdevice_stop(net_device);
		return ret;
	}
	/* simulate the 'net_device' init callback */
	if(strcmp(ops_req, "init") == 0){
		ret = netdevice_init(net_device);
		return ret;
	}
	if(strcmp(ops_req, "uninit") == 0){
		ret = netdevice_uninit(net_device);
		return ret;
	}
	if(strcmp(ops_req, "set_rx_mode") == 0){
		ret = netdevice_set_rx_mode(net_device);
		return ret;
	}
	if(strcmp(ops_req, "mac") == 0){
		new_mac_address = (void *)argv[2];
		ret = netdevice_set_mac_address(net_device, new_mac_address);
		return ret;
	}
	if(strcmp(ops_req, "rx") == 0){
		enable = true;

		if(num_args >= 3 && argv[2] != NULL){
			if(strcmp(argv[2], "off") == 0 || strcmp(argv[2], "0") == 0){
				enable = false;
			}else if(strcmp(argv[2], "on") != 0 && strcmp(argv[2], "1") != 0){
				#ifdef ENABLE_DBG_PRINT
				printf("[!] rx: ops rx [on|off|status]\n");
				#endif
				ret = -EINVAL;
				return ret;
			}
		}
		atomic_store(&__director->rx_dump_enabled, enable);
		if(enable){
			atomic_store(&__director->rx_dump_sequence, 0);
		}
		#ifdef ENABLE_DBG_PRINT
		printf("[*] RX hexdump %s\n", enable ? "enabled" : "disabled");
		#endif
	}
	if(strcmp(ops_req, "start_ap") == 0){
		ap_ssid = argv[2];
		if(ap_ssid == NULL){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] usage: ops start_ap <ssid_name>!\n");
			#endif
			ret = -EINVAL;
			return ret;
		}
		director.ap_ssid = ap_ssid;
		director.ap_ssid_len = strlen(ap_ssid);
		change_virtual_intf = cfg_ops->change_virtual_intf;
		if(change_virtual_intf != NULL){
			ret = change_virtual_intf(wiphy, net_device, NL80211_IFTYPE_AP, &params);
			if(ret != 0){
				return ret;
			}
		}
		/* allocate the 'ap_settings' struct */
		ap_settings = calloc(1, director.struct_sizes.cfg80211_ap_settings);
		if(ap_settings == NULL){
			ret = -ENOMEM;
			return ret;
		}
		ret = fill_cfg80211_start_ap_req(ap_settings);
		cfg80211_start_ap = cfg_ops->start_ap;
		if(ret == 0 && cfg80211_start_ap != NULL){
			ret = cfg80211_start_ap(wiphy, net_device, ap_settings);
		}
		free(ap_settings);
		ap_settings = NULL;
	}
	if(strcmp(ops_req, "stop_ap") == 0){
		cfg80211_stop_ap = cfg_ops->stop_ap;
		if(cfg80211_stop_ap != NULL){
			ret = cfg80211_stop_ap(wiphy, net_device, 0);
		}
	}
	if(strcmp(ops_req, "scan") == 0 || strcmp(ops_req, "hw_scan") == 0){
		ret = start_cfg80211_scan(wiphy, cfg_ops);
		return ret;
	}
	if(strcmp(ops_req, "abort_scan") == 0){

	}
	if(strcmp(ops_req, "auth") == 0){

	}
	if(strcmp(ops_req, "deauth") == 0){

	}
	if(strcmp(ops_req, "assoc") == 0){

	}
	if(strcmp(ops_req, "connect") == 0){

	}
	if(strcmp(ops_req, "disconnect") == 0){

	}
	/* implement also the 'testmode' callbacks */
	if(strcmp(ops_req, "testmode_cmd") == 0){

	}
	if(strcmp(ops_req, "testmode_dump") == 0){

	}
	if(strcmp(ops_req, "monitor") == 0){

	}
	if(strcmp(ops_req, "set_antenna") == 0){

	}
	if(strcmp(ops_req, "get_antenna") == 0){

	}
	return ret;

	ieee80211_vif_not_initialized:
	#ifdef ENABLE_DBG_PRINT
	printf("[!] ieee80211_vif is not initialized!\n");
	#endif
	ret = -EINVAL;
	return ret;
}

#ifdef TCC_SCRIPTING
/* tcc_add_runtime_symbols: used during the standalone ELF entry stub, it registers the symbols from the symbol table to the
 * tcc scripting engine */
static int tcc_add_runtime_symbols(TCCState *tcc_ctx){
	int ret = 0;
	bool duplicate = false;

	if(tcc_ctx == NULL){
		ret = -EINVAL;
		return ret;
	}
	for(int j = 0; sym_table[j].function_ptr != NULL && sym_table[j].function_name[0] != '\0'; j++){
		duplicate = false;

		if(strcmp(sym_table[j].function_name, "main") == 0){
			continue;
		}
		for(int i = 0; i < j; i++){
			if(strcmp(sym_table[i].function_name, sym_table[j].function_name) == 0){
				duplicate = true;
				break;
			}
		}
		if(duplicate){
			continue;
		}
		ret = tcc_add_symbol(tcc_ctx, sym_table[j].function_name, sym_table[j].function_ptr);
		if(ret < 0){
			return ret;
		}
	}
	return ret;
}

/* drv_integrate_scripting: initilize the tcc scripting engine, this feature will permit to write high level C
 * from the TUI and apply the patch at runtime to the loaded kernel modules. */
int drv_integrate_scripting(void){
	int ret = 0;

	director.tcc_ctx = tcc_new();
	if(director.tcc_ctx == NULL){
		ret = -ENOMEM;
		return ret;
	}
	/* step 1: disable boring tcc script warnings */
	tcc_set_options(director.tcc_ctx, "-w");
	/* step 2: set tcc output type, compilation in memory */
	tcc_set_output_type(director.tcc_ctx, TCC_OUTPUT_MEMORY);
	/* step 3: use the sym_table for adding automatically the symbols to tcc */
	ret = tcc_add_runtime_symbols(director.tcc_ctx);
	if(ret < 0){
		tcc_delete(director.tcc_ctx);
		director.tcc_ctx = NULL;
		return ret;
	}
	pthread_mutex_init(&director.tcc_mutex, NULL);
	memset(director.runtime_tcc_ctx, 0, sizeof(director.runtime_tcc_ctx));
	director.runtime_tcc_ctx_count = 0;
	return ret;
}

/* drv_execute_code: accept input code from the user and execute it */
int drv_execute_code(unsigned char *incoming_code){
	int ret = 0;

	if(director.tcc_ctx == NULL || incoming_code == NULL){
		ret = -EINVAL;
		return ret;
	}

	ret = tcc_compile_string(director.tcc_ctx, incoming_code);
	if(ret < 0){
		return ret;
	}
	ret = tcc_run(director.tcc_ctx, 0, NULL);
	return ret;
}

/* drv_scripting_loop: open the stdin and read the code */
int drv_scripting_loop(void) {
	int ret = 0;
	int current_line_len = 0;
	char *current_line = NULL;

	fflush(stdout);
	fflush(stderr);
	while(true) {
		ret = getline(&current_line, &current_line_len, stdin);
		if(ret < 0) {
			free(current_line);
			break;
                }
		director.tcc_memarea = realloc(director.tcc_memarea, (director.tcc_memarea ? strlen(director.tcc_memarea) : 0) + current_line_len + sizeof("\n"));
		if (director.tcc_memarea == NULL) {
			free(current_line);
			return NULL;
		}
		strcat(director.tcc_memarea, current_line);
		free(current_line);
		current_line = NULL;
	}
	return ret;
}

/* drv_shutdown_scripting: deinitialize the scripting engine */
int drv_shutdown_scripting(void){
	int ret = 0;

	if(director.tcc_ctx != NULL){
		tcc_delete(director.tcc_ctx);
		director.tcc_ctx = NULL;
	}

	for(u8 i = 0; i < director.runtime_tcc_ctx_count; i++){
		if(director.runtime_tcc_ctx[i] != NULL){
			tcc_delete(director.runtime_tcc_ctx[i]);
			director.runtime_tcc_ctx[i] = NULL;
		}
	}
	director.runtime_tcc_ctx_count = 0;

	if(director.tcc_memarea != NULL){
		free(director.tcc_memarea);
		director.tcc_memarea = NULL;
	}
	return ret;
}
#endif

/* elf_crash_handler: invoked on Linux when a crash happens. Keep it signal-safe:
 * the normal cleanup path uses pthread/libusb/free and can crash again here. */
void elf_crash_handler(int sig){
	atomic_store_explicit(&director.wifi_scan_thread_stop, true, memory_order_relaxed);
	atomic_store_explicit(&director.libusb_thread_stop, true, memory_order_relaxed);
	signal(sig, SIG_DFL);
	raise(sig);
}

#ifdef TCC_SCRIPTING
/* invoke_tcc_scripting: compile and execute the passed C strings in memory */
int invoke_tcc_scripting(unsigned char *c_string_data, int c_string_len){
	int ret = 0;
	char *source = NULL;
	TCCState *tcc_ctx = NULL;

	if(c_string_data == NULL || c_string_len <= 0 || c_string_len > BUFF_SIZE){
		ret = -EINVAL;
		return ret;
	}

	source = calloc((size_t)c_string_len + 1, sizeof(char));
	if(source == NULL){
		ret = -ENOMEM;
		return ret;
	}
	memcpy(source, c_string_data, (size_t)c_string_len);

	tcc_ctx = tcc_new();
	if(tcc_ctx == NULL){
		free(source);
		ret = -ENOMEM;
		return ret;
	}
	tcc_set_output_type(tcc_ctx, TCC_OUTPUT_MEMORY);
	ret = tcc_add_runtime_symbols(tcc_ctx);
	if(ret < 0){
		tcc_delete(tcc_ctx);
		free(source);
		return ret;
	}
	pthread_mutex_lock(&director.tcc_mutex);
	if(director.runtime_tcc_ctx_count >= MAX_TCC_RUNTIME_CONTEXTS){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] maximum runtime TCC contexts reached!\n");
		#endif
		ret = -ENOMEM;
		goto out_unlock;
	}
	ret = tcc_compile_string(tcc_ctx, source);
	if(ret < 0){
		goto out_unlock;
	}
	ret = tcc_run(tcc_ctx, 0, NULL);
	if(ret < 0){
		goto out_unlock;
	}
	director.runtime_tcc_ctx[director.runtime_tcc_ctx_count] = tcc_ctx;
	director.runtime_tcc_ctx_count++;
	tcc_ctx = NULL;

	out_unlock:
	pthread_mutex_unlock(&director.tcc_mutex);
	if(tcc_ctx != NULL){
		tcc_delete(tcc_ctx);
	}
	free(source);
	return ret;
}
#else
/* invoke_tcc_scripting: if 'TCC_SCRIPTING' isn't enabled, this is just an empty stub */
int invoke_tcc_scripting(unsigned char *c_string_data, int c_string_len){
	int ret = 0;

	ret = -EINVAL;
	return ret;
}
#endif

#ifdef __linux__
/* elf_socket_recv_full: handler code for receiving the tcc scripting data throught the IPC socket */
static int elf_socket_recv_full(int fd, char *buffer, size_t length){
	int ret = 0;
	int received = 0;
	struct pollfd pfd = {};

	while(received < length){
		ret = recv(fd, buffer + received, length - received, 0);
		if(ret > 0){
			received += ret;
			continue;
		}
		if(ret == 0){
			if(received == 0){
				return ret;
			}
			ret = -ECONNRESET;
			return ret;
		}
		if(errno == EINTR){
			continue;
		}
		if(errno == EAGAIN || errno == EWOULDBLOCK){
			pfd.fd = fd;
			pfd.events = POLLIN;
			ret = poll(&pfd, 1, -1);
			if(ret < 0 && errno != EINTR){
				ret = -EIO;
				return ret;
			}
			continue;
		}
		ret = -EIO;
		return ret;
	}
	return received;
}

/* elf_handle_driver_protocol: protocol handler for the standalone ELF */
int elf_handle_driver_protocol(struct module_controller *director, char *socket_buffer_data, int socket_buffer_len){
	int ret = 0;
	u32 msg_len = 0;
	unsigned char *console_cmd = NULL;
	unsigned char *tcc_high_string = NULL;
	struct exsul_protocol_hdr *hdr = NULL;

	/*
	 * protocol description:
	 * two different handlers:
	 * 1) TCC scripting: transfer high level C strings which get compiled and injected in the process memory.
	 * 2) console management: transfer command strings which gets executed as command, for example "ops start" invoke the ieee80211_ops start callback.
	 */

	if(socket_buffer_data == NULL || socket_buffer_len < sizeof(struct exsul_protocol_hdr)){
		ret = -EINVAL;
		return ret;
	}
	hdr = (struct exsul_protocol_hdr *)socket_buffer_data;
	msg_len = ((u32)hdr->len_hi << 16) | hdr->len;
	if(msg_len > BUFF_SIZE || socket_buffer_len < sizeof(struct exsul_protocol_hdr) + msg_len){
		ret = -EINVAL;
		return ret;
	}
	if(hdr->type == STANDALONE_TCC_SCRIPTING){
		#ifdef TCC_SCRIPTING
		tcc_high_string = (char *)socket_buffer_data + sizeof(struct exsul_protocol_hdr);
		ret = invoke_tcc_scripting(tcc_high_string, (int)msg_len);
		#else
		#ifdef ENABLE_DBG_PRINT
		printf("[!] tcc scripting not implemented!\n");
		#endif
		ret = -EINVAL;
		return ret;
		#endif
	}
	if(hdr->type == STANDALONE_CONSOLE_MGMT){
		console_cmd = (char *)socket_buffer_data + sizeof(struct exsul_protocol_hdr);
	}
	return ret;
}

/* elf_entry_socket: generate the socket interface for ipc communication,
 * heavily inspired by 'https://github.com/nguyenchiemminhvu/LinuxIPC/blob/main/Socket/unix_server.c' */
int elf_entry_socket(struct module_controller *director){
	int ret = 0;
	int number_fds = 0;
	int sock_client = 0;
	int packet_len = 0;
	u32 msg_len = 0;
	struct exsul_protocol_hdr hdr = {};
	char sock_buffer[sizeof(struct exsul_protocol_hdr) + BUFF_SIZE] = {};
	struct sockaddr_un addr_server = {};
	struct pollfd poll_fds[MAXIMUN_SOCKET_CONNECTIONS] = {};

	director->socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(director->socket_fd < 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to initialize socket!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	director->socket_flags = fcntl(director->socket_fd, F_GETFL);
	if(director->socket_flags < 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to get socket flags!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	ret = fcntl(director->socket_fd, F_SETFL, director->socket_flags | O_NONBLOCK);
	if(ret < 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to set socket flags!\n");
		#endif
		ret = -EINVAL;
		goto socket_close;
	}
	memset(&addr_server, 0, sizeof(struct sockaddr_un));
	addr_server.sun_family = AF_UNIX;
	strncpy(addr_server.sun_path, SOCKET_PATH, sizeof(addr_server.sun_path) - 1);
	unlink(SOCKET_PATH);

	ret = bind(director->socket_fd, (struct sockaddr *)&addr_server, sizeof(addr_server));
	if(ret < 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] error binding the server!\n");
		#endif
		ret = -EINVAL;
		goto socket_close;
	}

	ret = listen(director->socket_fd, MAXIMUN_SOCKET_CONNECTIONS);
	if(ret < 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to listed on the socket!\n");
		#endif
		ret = -EINVAL;
		goto socket_close;
	}

	/* now if everything is okay, the server will be start polling for new connections */
	memset(poll_fds, 0, sizeof(struct pollfd) * MAXIMUN_SOCKET_CONNECTIONS);
	poll_fds[0].fd = director->socket_fd;
	poll_fds[0].events = POLLIN;

	number_fds = 1;
	while (true){
		ret = poll(poll_fds, number_fds, -1);

		if (ret < 0){
			#ifdef ENABLE_DBG_PRINT
			printf("[!] error while polling!\n");
			#endif
			ret = -EIO;
			goto socket_close;
		}
		for (int i = 0; i < number_fds; i++){
			if (poll_fds[i].revents & POLLIN){
				if (poll_fds[i].fd == director->socket_fd){
					sock_client = accept(director->socket_fd, NULL, NULL);
					if (sock_client > 0){
						if (number_fds < MAXIMUN_SOCKET_CONNECTIONS){
							poll_fds[number_fds].fd = sock_client;
							poll_fds[number_fds].events = POLLIN;
							number_fds++;
						}else{
							close(sock_client);
						}
					}
				}else{
					memset(&hdr, 0, sizeof(hdr));
					memset(sock_buffer, 0, sizeof(sock_buffer));
					ret = elf_socket_recv_full(poll_fds[i].fd, (char *)&hdr, sizeof(hdr));
					if (ret > 0){
						msg_len = ((u32)hdr.len_hi << 16) | hdr.len;
						if(msg_len > BUFF_SIZE){
							ret = -EINVAL;
						}else{
							memcpy(sock_buffer, &hdr, sizeof(hdr));
							if(msg_len > 0){
								ret = elf_socket_recv_full(poll_fds[i].fd,
											   sock_buffer + sizeof(hdr),
											   msg_len);
							}
							if(ret > 0 || msg_len == 0){
								packet_len = sizeof(hdr) + msg_len;
								ret = elf_handle_driver_protocol(director, sock_buffer, packet_len);
							}
						}
					}else if (ret <= 0){
						#ifdef ENABLE_DBG_PRINT
						printf("[!] client %d disconnected!\n", poll_fds[i].fd);
						#endif
						close(poll_fds[i].fd);
						poll_fds[i] = poll_fds[number_fds - 1];
						number_fds--;
						i--;
					}
				}
			}
		}
	}
	close(director->socket_fd);
	unlink(SOCKET_PATH);
	return ret;

	socket_close:
	close(director->socket_fd);
	return ret;
}

/* elf_entry_stub: main entry point of the standalone ELF */
int elf_entry_stub(int argc, char *argv[]){
	int ret = 0;
	unsigned char *driver_init_name = NULL;

	/* this part of code is similar to 'main' and 'initialize_director', we must re-allocate/re-initialize every object */
	signal(SIGSEGV, elf_crash_handler);
	signal(SIGABRT, elf_crash_handler);
	signal(SIGILL,  elf_crash_handler);

	director.wifi_scan_thread = 0;
	director.libusb_event_thread = 0;
	director.internal_cb_table = &sym_table;
	director.envp = NULL;
	director.info = NULL;
	director.rfkill = NULL;
	director.usb_dev = NULL;
	director.usb_device_ptr = NULL;
	director.usb_interface = NULL;
	director.usb_device_id = NULL;
	director.ieee80211_local = NULL;
	director.rdev = NULL;
	director.wiphy = NULL;
	director.wdev = NULL;
	director.net_device = NULL;
	director.ieee80211_ops = NULL;
	director.cfg80211_ops = NULL;
	director.active_scan_req = NULL;
	director.connection_fsm = NULL;
	director.connection_runtime_initialized = false;
	memset(director.kmem_array, 0, sizeof(director.kmem_array));

	/* init libusb and create the main usb rx thread */
	ret = usb_runtime_init();
	if(ret != 0){
		return ret;
	}
	atomic_store(&director.libusb_thread_stop, false);
	pthread_create(&director.libusb_event_thread, NULL, libusb_thread, NULL);

	director.rfkill = NULL;
	director.rfkill = calloc(1, sizeof(struct rfkill));

	workqueue_init(&director.system_wq, "system_wq");
	system_wq = &director.system_wq;

	workqueue_init(&director.system_percpu_wq, "system_percpu_wq");
	system_percpu_wq = &director.system_percpu_wq;

	workqueue_init(&director.system_dfl_wq, "system_dfl_wq");
	system_dfl_wq = &director.system_dfl_wq;

	workqueue_init(&director.system_freezable_wq, "system_freezable_wq");
	system_freezable_wq = &director.system_freezable_wq;

	tasklet_subsystem_reset_runtime_state();
	tasklet_subsystem_init(MAX_SPAWNED_TASKLETS);

        /* initialize usb i/o mutex */
	pthread_mutex_init(&director.urb_anchor_mutex, NULL);
	pthread_mutex_init(&director.usb_io_lock, NULL);
	pthread_cond_init(&director.usb_io_idle, NULL);
	director.usb_io_depth = 0;

	/* initialize skb queue mutex */
	pthread_mutex_init(&director.skb_queue_lock, NULL);

	/* initialize the scan values */
	pthread_mutex_init(&director.scan_cache_lock, NULL);
	connection_runtime_init();
	pthread_mutex_init(&director.wiphy_work_queue_mutex, NULL);
	atomic_store(&director.scan_generation, 0);
	atomic_store(&director.scan_result_epoch, 0);
	atomic_store(&director.scan_in_progress, false);
	atomic_store(&director.wifi_scan_thread_started, false);
	atomic_store(&director.wifi_scan_thread_stop, false);
	director.wifi_scan_thread = 0;
	atomic_store(&director.rx_dump_enabled, false);
	atomic_store(&director.rx_dump_sequence, 0);

	/* init the timer locks */
	pthread_mutex_init(&director.timer_lock, NULL);
	pthread_cond_init(&director.timer_cond, NULL);
	director.timer_once = PTHREAD_ONCE_INIT;

	/* if enabled, initialize also the tcc scripting layer */
	#ifdef TCC_SCRIPTING
	drv_integrate_scripting();
	#endif

	/* invoke now the 'ops start' callback, this will initialize the driver and call the registered mac80211/cfg80211 callbacks */
	ret = drv_cmd_start(&director, NULL, 0);
	if(ret < 0){
		goto end;
	}
	/* jump to the ipc server creation and poll */
	elf_entry_socket(&director);

	end:
	if(director.rfkill != NULL){
		free(director.rfkill);
	}

	destroy_workqueue(&director.system_wq);
	system_wq = NULL;
	destroy_workqueue(&director.system_percpu_wq);
	system_percpu_wq = NULL;
	destroy_workqueue(&director.system_dfl_wq);
	system_dfl_wq = NULL;
	destroy_workqueue(&director.system_freezable_wq);
	system_freezable_wq = NULL;
	exit(ret);
}

/* elf_generator_handler: main function for getting the standalone ELF file,
 * it dumps the loaded memory regions, creates a legit symbol table and set the entry point to the address of 'elf_entry_stub' */
int elf_generator_handler(void){
	int ret = 0;
	int counter = 0;
	int pht_counter = 0;
	int elf_dump_fd = 0;
	int sym_table_size = 0;
	int string_table_size = 1;
	int current_symbol_name_len = 0;
	int sym_count = 0;
	int dynamic_pht_idx = -1;
	u32 phdrs = 0;
	u32 flags = 0;
	u32 seg_prot = 0;
	u64 vaddr = 0;
	u64 current_off = 0;
	u64 shstrtab_off = 0;
	u64 strtab_off = 0;
	u64 symtab_off = 0;
	u64 shoff = 0;
	u64 dyn_off = 0;
	u64 page_off = 0;
	u64 seg_off = 0;
	u64 name_off = 1;
	u64 dynamic_vaddr = 0;
	u64 dynamic_foff = 0;
	u64 dynamic_filesz = 0;
	u64 zero_val = 0;
	u64 null_off = 0;
	u64 load_dynamic_foff = 0;
	enum {
		SEC_NULL	= 0,
		SEC_SHSTRTAB	= 1,
		SEC_STRTAB	= 2,
		SEC_SYMTAB	= 3,
		SEC_DYNAMIC	= 4,
		SEC_COUNT	= 5
	};

	/* add also the .dynamic section to the code */
	char shstrtab[] = "\0.shstrtab\0.strtab\0.symtab\0.dynamic\0";

	Elf(Ehdr) header = {};
	Elf(Shdr) section_header = {};
	Elf(Sym)  symbol_table = {};
	Elf(Dyn) new_null = {};
	Elf(Dyn) flags1_entry = {};
	Elf(Dyn) *dyn = NULL;
	char **envp = NULL;
	Elf(auxv_t) *auxv = NULL;
	Elf(Phdr) *phdr_base =  NULL;
	struct allocation_list *head = NULL;

	head = director.module_allocation_list;
	if(head == NULL){
		ret = -EINVAL;
		return ret;
	}

	if(director.loaded_lkm_num == 0){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] no modules loaded!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	if(director.envp == NULL){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] envp not saved!\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	envp = director.envp;
	while (*envp != NULL){
		envp++;
	}
	envp++;

	for (auxv = (Elf(auxv_t) *)envp; auxv->a_type != AT_NULL; auxv++){
		if (auxv->a_type == AT_PHDR){
			phdr_base = (Elf(Phdr) *)(uintptr_t)auxv->a_un.a_val;
		}
		if(auxv->a_type == AT_PHNUM){
			phdrs = (u32)auxv->a_un.a_val;
		}
	}

	if(phdr_base != NULL && phdrs != 0){
		for(u32 k = 0; k < phdrs; k++){
			if(phdr_base[k].p_memsz == 0 || (phdr_base[k].p_type != PT_DYNAMIC && phdr_base[k].p_type != PT_LOAD && phdr_base[k].p_type != PT_INTERP)){
				continue;
			}
			seg_prot = 0;
			if(phdr_base[k].p_flags & PF_R){
				seg_prot |= PROT_READ;
			}
			if(phdr_base[k].p_flags & PF_W){
				seg_prot |= PROT_WRITE;
			}
			if(phdr_base[k].p_flags & PF_X){
				seg_prot |= PROT_EXEC;
			}
			director.module_allocation_list = new_allocation(
				ALLOCATION_TYPE_MALLOC,
				(void *)(uintptr_t)phdr_base[k].p_vaddr,
				phdr_base[k].p_memsz,
				seg_prot,
				phdr_base[k].p_type,
				director.module_allocation_list);
		}
	}

	elf_dump_fd = open(director.module_name, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if(elf_dump_fd < 0){
		ret = -EIO;
		return ret;
	}

	head = director.module_allocation_list;
	counter = 0;
	while(head != NULL){
		if(head->alloc_ptr != NULL && head->alloc_len != 0){
			counter++;
		}
		head = head->alloc_next;
	}
	head = director.module_allocation_list;

	for(sym_count = 0; sym_table[sym_count].function_name[0] != '\0'; sym_count++){
		string_table_size += (int)strlen(sym_table[sym_count].function_name) + 1;
	}
	sym_table_size = (sym_count + 1) * sizeof(Elf(Sym));

	if(counter == 0){
		close(elf_dump_fd);
		ret = -EINVAL;
		return ret;
	}

	Elf(Phdr) program_headers[counter + 1];
	Elf(Shdr) section_headers[SEC_COUNT];
	Elf(Sym)  symbol_entries[sym_count + 1];
	char      string_table[string_table_size];

	memset(program_headers, 0,	sizeof(program_headers));
	memset(section_headers, 0,	sizeof(section_headers));
	memset(symbol_entries, 0,	sizeof(symbol_entries));
	memset(string_table, 0,		sizeof(string_table));

	for(int k = 0; k < sym_count; k++){
		current_symbol_name_len = strlen(sym_table[k].function_name) + 1;

		memcpy(&string_table[name_off], sym_table[k].function_name, current_symbol_name_len);

		symbol_entries[k + 1].st_name  = name_off;
		symbol_entries[k + 1].st_info  = ELF64_ST_INFO(STB_GLOBAL, STT_FUNC);
		symbol_entries[k + 1].st_other = STV_DEFAULT;
		symbol_entries[k + 1].st_shndx = SHN_ABS;
		symbol_entries[k + 1].st_value = (u64)(uintptr_t)sym_table[k].function_ptr;
		symbol_entries[k + 1].st_size  = sym_table[k].function_size;

		name_off += current_symbol_name_len;
	}

	current_off = sizeof(Elf(Ehdr)) + ((counter + 1) * sizeof(Elf(Phdr)));

	program_headers[0].p_type   = PT_LOAD;
	program_headers[0].p_offset = 0;
	program_headers[0].p_vaddr  = 0x200000ULL;
	program_headers[0].p_paddr  = 0x200000ULL;
	program_headers[0].p_filesz = current_off;
	program_headers[0].p_memsz  = current_off;
	program_headers[0].p_flags  = PF_R;
	program_headers[0].p_align  = 0x1000;

	pht_counter = 1;
	head = director.module_allocation_list;

	while(head != NULL){
		if(head->alloc_ptr == NULL || head->alloc_len == 0){
			head = head->alloc_next;
			continue;
		}
		flags = 0;
		if(head->alloc_flags & PROT_READ){
			flags |= PF_R;
		}
		if(head->alloc_flags & PROT_WRITE){
			flags |= PF_W;
		}
		if(head->alloc_flags & PROT_EXEC){
			flags |= PF_X;
		}
		vaddr = (u64)(uintptr_t)head->alloc_ptr;
		page_off = vaddr & 0xfffULL;
		seg_off = (((current_off) + ((0x1000ULL) - 1)) & ~((0x1000ULL) - 1));
		if((seg_off & 0xfffULL) != page_off){
			seg_off += ((page_off - (seg_off & 0xfffULL)) & 0xfffULL);
		}

		program_headers[pht_counter].p_type   = head->alloc_inmem_type;
		program_headers[pht_counter].p_offset = seg_off;
		program_headers[pht_counter].p_vaddr  = vaddr;
		program_headers[pht_counter].p_paddr  = vaddr;
		program_headers[pht_counter].p_memsz  = head->alloc_len;
		program_headers[pht_counter].p_filesz = head->alloc_len;
		program_headers[pht_counter].p_align  = 0x1000;
		program_headers[pht_counter].p_flags  = flags;

		if(head->alloc_inmem_type == PT_DYNAMIC){
			dynamic_pht_idx = pht_counter;
		}

		current_off = seg_off + head->alloc_len;
		pht_counter++;
		head = head->alloc_next;
	}

	if(dynamic_pht_idx >= 0){
		dynamic_vaddr  = program_headers[dynamic_pht_idx].p_vaddr;
		dynamic_foff   = program_headers[dynamic_pht_idx].p_offset;
		dynamic_filesz = program_headers[dynamic_pht_idx].p_filesz;
	}

	shstrtab_off			= (((current_off) + ((8) - 1)) & ~((8) - 1));
	strtab_off			= shstrtab_off  + sizeof(shstrtab);
	symtab_off			= (((strtab_off + string_table_size) + ((8) - 1)) & ~((8) - 1));
	shoff				= (((symtab_off + sym_table_size) + ((8) - 1)) & ~((8) - 1));

	header.e_ident[0]		= 0x7f;
	header.e_ident[1]		= 'E';
	header.e_ident[2]		= 'L';
	header.e_ident[3]		= 'F';
	header.e_ident[4]		= ELFCLASS64;
	header.e_ident[5]		= ELFDATA2LSB;
	header.e_ident[6]		= EV_CURRENT;
	header.e_ident[7]		= ELFOSABI_SYSV;

	header.e_machine		= EM_CURRENT_ARCH;
	header.e_entry			= elf_entry_stub;
	header.e_phnum			= counter + 1;
	header.e_shoff			= shoff;
	header.e_phoff			= sizeof(Elf(Ehdr));
	header.e_shnum			= SEC_COUNT;
	header.e_phentsize		= sizeof(Elf(Phdr));
	header.e_shentsize		= sizeof(Elf(Shdr));
	header.e_type			= ET_EXEC;
	header.e_version		= EV_CURRENT;
	header.e_shstrndx		= SEC_SHSTRTAB;
	header.e_ehsize			= sizeof(Elf(Ehdr));
	header.e_flags			= 0;

	memset(&section_header, 0, sizeof(section_header));
	section_header.sh_name		= 1;
	section_header.sh_type		= SHT_STRTAB;
	section_header.sh_flags		= 0;
	section_header.sh_addr		= 0;
	section_header.sh_offset	= shstrtab_off;
	section_header.sh_size		= sizeof(shstrtab);
	section_header.sh_link		= 0;
	section_header.sh_info		= 0;
	section_header.sh_addralign	= 1;
	section_header.sh_entsize	= 0;
	section_headers[SEC_SHSTRTAB]	= section_header;

	memset(&section_header, 0, sizeof(section_header));
	section_header.sh_name		= 11;
	section_header.sh_type		= SHT_STRTAB;
	section_header.sh_flags		= 0;
	section_header.sh_addr		= 0;
	section_header.sh_offset	= strtab_off;
	section_header.sh_size		= string_table_size;
	section_header.sh_link		= 0;
	section_header.sh_info		= 0;
	section_header.sh_addralign	= 1;
	section_header.sh_entsize	= 0;
	section_headers[SEC_STRTAB]	= section_header;

	memset(&section_header, 0, sizeof(section_header));
	section_header.sh_name		= 19;
	section_header.sh_type		= SHT_SYMTAB;
	section_header.sh_flags		= 0;
	section_header.sh_addr		= 0;
	section_header.sh_offset	= symtab_off;
	section_header.sh_size		= sym_table_size;
	section_header.sh_link		= SEC_STRTAB;
	section_header.sh_info		= 1;
	section_header.sh_addralign	= 8;
	section_header.sh_entsize	= sizeof(Elf(Sym));
	section_headers[SEC_SYMTAB]	= section_header;

	if(dynamic_pht_idx >= 0){
		memset(&section_header, 0, sizeof(section_header));
		section_header.sh_name		= 27;
		section_header.sh_type		= SHT_DYNAMIC;
		section_header.sh_flags		= SHF_WRITE | SHF_ALLOC;
		section_header.sh_addr		= dynamic_vaddr;
		section_header.sh_offset	= dynamic_foff;
		section_header.sh_size		= dynamic_filesz;
		section_header.sh_link		= 0;
		section_header.sh_info		= 0;
		section_header.sh_addralign	= 8;
		section_header.sh_entsize	= sizeof(Elf(Dyn));
		section_headers[SEC_DYNAMIC]	= section_header;
	}

	lseek(elf_dump_fd, 0, SEEK_SET);
	if(write(elf_dump_fd, &header, sizeof(header)) != sizeof(header)){
		close(elf_dump_fd);
		ret = -EIO;
		return ret;
	}

	lseek(elf_dump_fd, header.e_phoff, SEEK_SET);
	if(write(elf_dump_fd, program_headers, sizeof(program_headers)) != sizeof(program_headers)){
		close(elf_dump_fd);
		ret = -EIO;
		return ret;
	}

	pht_counter = 1;
	head = director.module_allocation_list;
	while(head != NULL){
		if(head->alloc_ptr == NULL || head->alloc_len == 0){
			head = head->alloc_next;
			continue;
		}

		lseek(elf_dump_fd, program_headers[pht_counter].p_offset, SEEK_SET);
		if(write(elf_dump_fd, head->alloc_ptr, head->alloc_len) != (ssize_t)head->alloc_len){
			close(elf_dump_fd);
			ret = -EIO;
			return ret;
		}

		pht_counter++;
		head = head->alloc_next;
	}

	if(dynamic_pht_idx >= 0){
		dyn = (Elf(Dyn) *)(uintptr_t)dynamic_vaddr;
		zero_val = 0;
		load_dynamic_foff = 0;

		for(int i = 0; i < (int)header.e_phnum; i++){
			if(program_headers[i].p_type == PT_LOAD && dynamic_vaddr >= program_headers[i].p_vaddr && dynamic_vaddr < program_headers[i].p_vaddr + program_headers[i].p_filesz){
				load_dynamic_foff = program_headers[i].p_offset + (dynamic_vaddr - program_headers[i].p_vaddr);
				break;
			}
		}
		if(load_dynamic_foff == 0){
			load_dynamic_foff = dynamic_foff;
		}

		for(; dyn->d_tag != DT_NULL; dyn++){
			dyn_off = (u64)(uintptr_t)dyn - dynamic_vaddr;
			if(dyn->d_tag == DT_DEBUG || dyn->d_tag == DT_INIT_ARRAYSZ || dyn->d_tag == DT_FINI_ARRAYSZ){
				u64 val_foff = load_dynamic_foff + dyn_off + offsetof(Elf(Dyn), d_un);
				lseek(elf_dump_fd, (off_t)val_foff, SEEK_SET);
				write(elf_dump_fd, &zero_val, sizeof(zero_val));
			}
		}
		null_off = (u64)(uintptr_t)dyn - dynamic_vaddr;
		memset(&flags1_entry, 0, sizeof(flags1_entry));
		memset(&new_null,     0, sizeof(new_null));
		flags1_entry.d_tag        = DT_FLAGS_1;
		flags1_entry.d_un.d_val   = DF_1_NOW;
		lseek(elf_dump_fd, (off_t)(load_dynamic_foff + null_off), SEEK_SET);
		write(elf_dump_fd, &flags1_entry, sizeof(flags1_entry));
		lseek(elf_dump_fd, (off_t)(load_dynamic_foff + null_off + sizeof(Elf(Dyn))), SEEK_SET);
		write(elf_dump_fd, &new_null, sizeof(new_null));
	}

	lseek(elf_dump_fd, shstrtab_off, SEEK_SET);
	if(write(elf_dump_fd, shstrtab, sizeof(shstrtab)) != sizeof(shstrtab)){
		close(elf_dump_fd);
		ret = -EIO;
		return ret;
	}

	lseek(elf_dump_fd, strtab_off, SEEK_SET);
	if(write(elf_dump_fd, string_table, string_table_size) != string_table_size){
		close(elf_dump_fd);
		ret = -EIO;
		return ret;
	}

	lseek(elf_dump_fd, symtab_off, SEEK_SET);
	if(write(elf_dump_fd, symbol_entries, sym_table_size) != sym_table_size){
		close(elf_dump_fd);
		ret = -EIO;
		return ret;
	}

	lseek(elf_dump_fd, shoff, SEEK_SET);
	if(write(elf_dump_fd, section_headers, sizeof(section_headers)) != sizeof(section_headers)){
		close(elf_dump_fd);
		ret = -EIO;
		return ret;
	}

	close(elf_dump_fd);
	return ret;
}
#else
/* Arbitrary module mappings cannot be represented reliably as PE sections because section RVAs are 32-bit.
 * Keep them in an overlay and restore them at their original addresses before invoking the driver. */
#define PE_DUMP_VERSION                 1
#define PE_STANDALONE_MARKER            0x45585045U

static const u8 pe_dump_magic[8] = {'E', 'X', 'S', 'U', 'L', 'P', 'E', '1'};
static const u8 pe_dump_footer_magic[8] = {'E', 'X', 'S', 'U', 'L', 'P', 'F', '1'};
static volatile u32 pe_standalone_mode;

#pragma pack(push, 1)
struct pe_dump_header {
        u8 magic[8];
        u32 version;
        u32 region_count;
        u64 region_table_offset;
};

struct pe_dump_region {
        u64 address;
        u64 size;
        u64 file_offset;
        u32 protection;
        u32 reserved;
};

struct pe_dump_footer {
        u8 magic[8];
        u32 version;
        u32 region_count;
        u64 header_offset;
};
#pragma pack(pop)

static u64 pe_align_value(u64 value, u64 alignment){
        if(alignment == 0){
                return value;
        }
        return (value + alignment - 1) & ~(alignment - 1);
}

static int pe_write_at(HANDLE file, u64 offset, const void *buffer, u64 length){
	int ret = 0;
        DWORD written = 0;
        DWORD chunk = 0;
        LARGE_INTEGER file_offset = {};
        const u8 *cursor = buffer;

        file_offset.QuadPart = offset;
        if(!SetFilePointerEx(file, file_offset, NULL, FILE_BEGIN)){
                ret = -EIO;
		return ret;
        }
        while(length != 0){
                chunk = length > 0x40000000ULL ? 0x40000000UL : (DWORD)length;
                if(!WriteFile(file, cursor, chunk, &written, NULL) || written != chunk){
                        ret = -EIO;
			return ret;
                }
                cursor += written;
                length -= written;
        }
        return ret;
}

static int pe_read_at(HANDLE file, u64 offset, void *buffer, u64 length){
	int ret = 0;
        DWORD received = 0;
        DWORD chunk = 0;
        LARGE_INTEGER file_offset = {};
        u8 *cursor = buffer;

        file_offset.QuadPart = offset;
        if(!SetFilePointerEx(file, file_offset, NULL, FILE_BEGIN)){
                ret = -EIO;
		return ret;
        }
        while(length != 0){
                chunk = length > 0x40000000ULL ? 0x40000000UL : (DWORD)length;
                if(!ReadFile(file, cursor, chunk, &received, NULL) || received != chunk){
                        ret = -EIO;
			return ret;
                }
                cursor += received;
                length -= received;
        }
        return ret;
}

static DWORD pe_region_page_protection(u32 protection){
        if(protection & PROT_EXEC){
                if(protection & PROT_WRITE){
                        return PAGE_EXECUTE_READWRITE;
                }
                return (protection & PROT_READ) ? PAGE_EXECUTE_READ : PAGE_EXECUTE;
        }
        if(protection & PROT_WRITE){
                return PAGE_READWRITE;
        }
        return (protection & PROT_READ) ? PAGE_READONLY : PAGE_NOACCESS;
}

/* pe_restore_dumped_regions: restore relocated module text and data before any persisted function pointer is used. */
static int pe_restore_dumped_regions(void){
        int ret = 0;
        u32 restored_count = 0;
        DWORD old_protection = 0;
        char executable_path[MAX_PATH] = {};
        HANDLE file = INVALID_HANDLE_VALUE;
        LARGE_INTEGER file_size = {};
        LPVOID mapped_region = NULL;
        struct pe_dump_header header = {};
        struct pe_dump_footer footer = {};
        struct pe_dump_region *regions = NULL;

        if(GetModuleFileNameA(NULL, executable_path, sizeof(executable_path)) == 0){
                ret = -EIO;
		return ret;
        }
        file = CreateFileA(executable_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(file == INVALID_HANDLE_VALUE){
                ret = -EIO;
		return ret;
        }
        if(!GetFileSizeEx(file, &file_size) || file_size.QuadPart < sizeof(footer)){
                ret = -EIO;
                goto out;
        }
        ret = pe_read_at(file, (u64)file_size.QuadPart - sizeof(footer), &footer, sizeof(footer));
        if(ret != 0 || memcmp(footer.magic, pe_dump_footer_magic, sizeof(footer.magic)) != 0 ||
           footer.version != PE_DUMP_VERSION || footer.header_offset >= (u64)file_size.QuadPart){
                printf("[!] invalid or missing Exsul PE dump footer!\n");
                ret = -EINVAL;
                goto out;
        }
        ret = pe_read_at(file, footer.header_offset, &header, sizeof(header));
        if(ret != 0 || memcmp(header.magic, pe_dump_magic, sizeof(header.magic)) != 0 ||
           header.version != PE_DUMP_VERSION || header.region_count != footer.region_count ||
           header.region_table_offset > (u64)file_size.QuadPart){
                printf("[!] invalid Exsul PE dump header!\n");
                ret = -EINVAL;
                goto out;
        }
        if(header.region_count == 0 || header.region_count > (SIZE_MAX / sizeof(*regions))){
                ret = -EINVAL;
                goto out;
        }
        regions = calloc(header.region_count, sizeof(*regions));
        if(regions == NULL){
                ret = -ENOMEM;
                goto out;
        }
        ret = pe_read_at(file, header.region_table_offset, regions, (u64)header.region_count * sizeof(*regions));
        if(ret != 0){
                goto out;
        }

        director.module_allocation_list = NULL;
        for(u32 i = 0; i < header.region_count; i++){
                if(regions[i].address == 0 || regions[i].size == 0 ||
                   regions[i].file_offset > (u64)file_size.QuadPart ||
                   regions[i].size > (u64)file_size.QuadPart - regions[i].file_offset){
                        ret = -EINVAL;
                        goto restore_fail;
                }
                mapped_region = VirtualAlloc((LPVOID)(uintptr_t)regions[i].address, (SIZE_T)regions[i].size,
                                             MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                if(mapped_region != (LPVOID)(uintptr_t)regions[i].address){
                        if(mapped_region != NULL){
                                VirtualFree(mapped_region, 0, MEM_RELEASE);
                        }
                        printf("[!] cannot restore PE region at %p (%llu bytes), error %lu\n",
                               (void *)(uintptr_t)regions[i].address, (unsigned long long)regions[i].size,
                               (unsigned long)GetLastError());
                        ret = -ENOMEM;
                        goto restore_fail;
                }
                restored_count++;
                ret = pe_read_at(file, regions[i].file_offset, mapped_region, regions[i].size);
                if(ret != 0){
                        goto restore_fail;
                }
                if(!VirtualProtect(mapped_region, (SIZE_T)regions[i].size,
                                   pe_region_page_protection(regions[i].protection), &old_protection)){
                        ret = -EIO;
                        goto restore_fail;
                }
                if(regions[i].protection & PROT_EXEC){
                        FlushInstructionCache(GetCurrentProcess(), mapped_region, (SIZE_T)regions[i].size);
                }
                director.module_allocation_list = new_allocation(ALLOCATION_TYPE_MMAP, mapped_region,
                        (int)regions[i].size, (u8)regions[i].protection, PT_LOAD, director.module_allocation_list);
        }
        goto out;

	restore_fail:
        for(u32 i = 0; i < restored_count; i++){
                if(regions[i].address != 0){
                        VirtualFree((LPVOID)(uintptr_t)regions[i].address, 0, MEM_RELEASE);
                }
        }
        director.module_allocation_list = NULL;
	out:
        if(regions != NULL){
                free(regions);
        }
        if(file != INVALID_HANDLE_VALUE){
                CloseHandle(file);
        }
        return ret;
}

/* pe_entry_stub: entry point for the PE dumped driver */
static int pe_entry_stub(void){
        int ret = 0;
        void *driver_init = NULL;

	driver_init = director.driver_init_function;
        ret = pe_restore_dumped_regions();
        if(ret != 0){
                return ret;
        }
        director.internal_cb_table = &sym_table;
        director.envp = NULL;
        director.info = NULL;
        director.rfkill = NULL;
        director.usb_dev = NULL;
        director.usb_device_ptr = NULL;
        director.usb_interface = NULL;
        director.usb_device_id = NULL;
        director.ieee80211_local = NULL;
        director.rdev = NULL;
        director.net_device = NULL;
        director.ieee80211_ops = NULL;
        director.active_scan_req = NULL;
        director.connection_fsm = NULL;
	director.connection_runtime_initialized = false;
        director.libusb_event_thread = 0;
        memset(director.kmem_array, 0, sizeof(director.kmem_array));

        system_wq = NULL;
        system_percpu_wq = NULL;
        system_dfl_wq = NULL;
        system_freezable_wq = NULL;
        ret = usb_runtime_init();
        if(ret != 0){
                return ret;
        }
        atomic_store(&director.libusb_thread_stop, false);
        pthread_create(&director.libusb_event_thread, NULL, libusb_thread, NULL);

        director.rfkill = calloc(1, sizeof(struct rfkill));
        if(director.rfkill == NULL){
                ret = -ENOMEM;
		return ret;
        }
        workqueue_init(&director.system_wq, "system_wq");
        system_wq = &director.system_wq;
        workqueue_init(&director.system_percpu_wq, "system_percpu_wq");
        system_percpu_wq = &director.system_percpu_wq;
        workqueue_init(&director.system_dfl_wq, "system_dfl_wq");
        system_dfl_wq = &director.system_dfl_wq;
        workqueue_init(&director.system_freezable_wq, "system_freezable_wq");
        system_freezable_wq = &director.system_freezable_wq;

        pthread_mutex_init(&director.urb_anchor_mutex, NULL);
        pthread_mutex_init(&director.usb_io_lock, NULL);
        pthread_cond_init(&director.usb_io_idle, NULL);
        director.usb_io_depth = 0;
        pthread_mutex_init(&director.skb_queue_lock, NULL);
        pthread_mutex_init(&director.scan_cache_lock, NULL);
	connection_runtime_init();
        pthread_mutex_init(&director.wiphy_work_queue_mutex, NULL);
        atomic_store(&director.scan_generation, 0);
        atomic_store(&director.scan_result_epoch, 0);
        atomic_store(&director.scan_in_progress, false);
	atomic_store(&director.wifi_scan_thread_started, false);
	atomic_store(&director.wifi_scan_thread_stop, false);
	director.wifi_scan_thread = 0;
	atomic_store(&director.rx_dump_enabled, false);
	atomic_store(&director.rx_dump_sequence, 0);
        pthread_mutex_init(&director.timer_lock, NULL);
        pthread_cond_init(&director.timer_cond, NULL);
        director.timer_once = PTHREAD_ONCE_INIT;

	#ifdef TCC_SCRIPTING
        director.tcc_ctx = NULL;
        memset(director.runtime_tcc_ctx, 0, sizeof(director.runtime_tcc_ctx));
        director.runtime_tcc_ctx_count = 0;
        drv_integrate_scripting();
	#endif
	/* invoke now the 'ops start' callback, this will initialize the driver and call the registered mac80211/cfg80211 callbacks */
	ret = drv_cmd_start(&director, NULL, 0);
	if(ret < 0){
		goto end;
	}

	end:
	if(director.rfkill != NULL){
		free(director.rfkill);
	}

	destroy_workqueue(&director.system_wq);
	system_wq = NULL;
	destroy_workqueue(&director.system_percpu_wq);
	system_percpu_wq = NULL;
	destroy_workqueue(&director.system_dfl_wq);
	system_dfl_wq = NULL;
	destroy_workqueue(&director.system_freezable_wq);
	system_freezable_wq = NULL;
	exit(ret);
}

/* pe_generator_handler: generate the PE dump, invoked by the command 'get pe' */
int pe_generator_handler(void){
        int ret = 0;
        u32 region_count = 0;
        u32 section_count = 0;
        u64 raw_cursor = 0;
        u64 dump_size = 0;
        u64 raw_section_size = 0;
        u64 overlay_offset = 0;
        u64 data_cursor = 0;
        u64 final_size = 0;
        u64 size_of_code = 0;
        u64 size_of_initialized_data = 0;
        u8 *base_addr = NULL;
        u8 *headers   = NULL;
	struct allocation_list *head = NULL;
        IMAGE_DOS_HEADER *dos_header = NULL;
        IMAGE_NT_HEADERS64 *nt_header = NULL;
	IMAGE_SECTION_HEADER *sections = NULL;
        IMAGE_NT_HEADERS64 *dump_nt_header = NULL;
        IMAGE_SECTION_HEADER *dump_sections = NULL;
       	LARGE_INTEGER end_position = {};
        struct pe_dump_header dump_header = {};
        struct pe_dump_footer dump_footer = {};
        struct pe_dump_region *regions = NULL;

	HANDLE dump_file = INVALID_HANDLE_VALUE;
        if(director.module_allocation_list == NULL || director.loaded_lkm_num == 0){
                printf("[!] no modules loaded!\n");
                ret = -EINVAL;
		return ret;
        }

        base_addr = (BYTE *)GetModuleHandleW(NULL);
        if(base_addr == NULL){
                ret = -EINVAL;
		return ret;
        }
        dos_header = (IMAGE_DOS_HEADER *)base_addr;
        if(dos_header->e_magic != IMAGE_DOS_SIGNATURE){
                printf("[!] current executable has no valid DOS header!\n");
                ret = -EINVAL;
		return ret;
        }
        nt_header = (IMAGE_NT_HEADERS64 *)(base_addr + dos_header->e_lfanew);
        if(nt_header->Signature != IMAGE_NT_SIGNATURE || nt_header->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC){
                printf("[!] current executable has no valid PE32+ header!\n");
                ret = -EINVAL;
		return ret;
        }
        for(head = director.module_allocation_list; head != NULL; head = head->alloc_next){
                if(head->alloc_ptr != NULL && head->alloc_len > 0){
                        region_count++;
                }
        }
        if(region_count == 0){
                ret = -EINVAL;
		return ret;
        }

        section_count = nt_header->FileHeader.NumberOfSections;
        headers = calloc(1, nt_header->OptionalHeader.SizeOfHeaders);
        regions = calloc(region_count, sizeof(*regions));
        if(headers == NULL || regions == NULL){
                ret = -ENOMEM;
                goto out;
        }
        memcpy(headers, base_addr, nt_header->OptionalHeader.SizeOfHeaders);
        dump_nt_header = (IMAGE_NT_HEADERS64 *)(headers + dos_header->e_lfanew);
        sections = IMAGE_FIRST_SECTION(nt_header);
        dump_sections = IMAGE_FIRST_SECTION(dump_nt_header);
        raw_cursor = pe_align_value(dump_nt_header->OptionalHeader.SizeOfHeaders, dump_nt_header->OptionalHeader.FileAlignment);

        for(u32 i = 0; i < section_count; i++){
                dump_size = sections[i].Misc.VirtualSize;
                if(dump_size < sections[i].SizeOfRawData){
                        dump_size = sections[i].SizeOfRawData;
                }
                if((u64)sections[i].VirtualAddress + dump_size > nt_header->OptionalHeader.SizeOfImage){
                        dump_size = nt_header->OptionalHeader.SizeOfImage - sections[i].VirtualAddress;
                }
                if(dump_size == 0){
                        dump_sections[i].PointerToRawData = 0;
                        dump_sections[i].SizeOfRawData = 0;
                        continue;
                }
                raw_section_size = pe_align_value(dump_size, dump_nt_header->OptionalHeader.FileAlignment);
                if(raw_cursor > UINT32_MAX || raw_section_size > UINT32_MAX - raw_cursor){
                        printf("[!] rebuilt PE image exceeds the 32-bit raw file offset limit!\n");
                        ret = -EFBIG;
                        goto out;
                }
                dump_sections[i].PointerToRawData = (DWORD)raw_cursor;
                dump_sections[i].SizeOfRawData = (DWORD)raw_section_size;
                raw_cursor += raw_section_size;
                if(dump_sections[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA){
                        dump_sections[i].Characteristics &= ~IMAGE_SCN_CNT_UNINITIALIZED_DATA;
                        dump_sections[i].Characteristics |= IMAGE_SCN_CNT_INITIALIZED_DATA;
                }
                if(dump_sections[i].Characteristics & IMAGE_SCN_CNT_CODE){
                        size_of_code += dump_sections[i].SizeOfRawData;
                }else{
                        size_of_initialized_data += dump_sections[i].SizeOfRawData;
                }
        }

        dump_nt_header->OptionalHeader.ImageBase = (ULONGLONG)(uintptr_t)base_addr;
        dump_nt_header->OptionalHeader.SizeOfCode = (DWORD)size_of_code;
        dump_nt_header->OptionalHeader.SizeOfInitializedData = (DWORD)size_of_initialized_data;
        dump_nt_header->OptionalHeader.SizeOfUninitializedData = 0;
        dump_nt_header->OptionalHeader.CheckSum = 0;
        dump_nt_header->OptionalHeader.DllCharacteristics &= ~(IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE | IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA);
        dump_nt_header->FileHeader.Characteristics |= IMAGE_FILE_RELOCS_STRIPPED;
        memset(&dump_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC], 0, sizeof(IMAGE_DATA_DIRECTORY));
        memset(&dump_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT], 0, sizeof(IMAGE_DATA_DIRECTORY));
        memset(&dump_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_SECURITY], 0, sizeof(IMAGE_DATA_DIRECTORY));
        memset(&dump_nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG], 0, sizeof(IMAGE_DATA_DIRECTORY));

        fast_get_symbol("_driver_init", &director.driver_init_function);
        pe_standalone_mode = PE_STANDALONE_MARKER;
        dump_file = CreateFileA("dumped_driver.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(dump_file == INVALID_HANDLE_VALUE){
                ret = -EIO;
                goto out;
        }
        ret = pe_write_at(dump_file, 0, headers, dump_nt_header->OptionalHeader.SizeOfHeaders);
        if(ret != 0){
                goto out;
        }
        for(u32 i = 0; i < section_count; i++){
                dump_size = sections[i].Misc.VirtualSize;
                if(dump_size < sections[i].SizeOfRawData){
                        dump_size = sections[i].SizeOfRawData;
                }
                if((u64)sections[i].VirtualAddress + dump_size > nt_header->OptionalHeader.SizeOfImage){
                        dump_size = nt_header->OptionalHeader.SizeOfImage - sections[i].VirtualAddress;
                }
                if(dump_size != 0){
                        ret = pe_write_at(dump_file, dump_sections[i].PointerToRawData, base_addr + sections[i].VirtualAddress, dump_size);
                        if(ret != 0){
                                goto out;
                        }
                }
        }

        overlay_offset = raw_cursor;
        memcpy(dump_header.magic, pe_dump_magic, sizeof(dump_header.magic));
        dump_header.version = PE_DUMP_VERSION;
        dump_header.region_count = region_count;
        dump_header.region_table_offset = overlay_offset + sizeof(dump_header);
        data_cursor = pe_align_value(dump_header.region_table_offset + ((u64)region_count * sizeof(*regions)), 16);
        head = director.module_allocation_list;
        for(u32 i = 0; i < region_count; i++){
                while(head != NULL && (head->alloc_ptr == NULL || head->alloc_len <= 0)){
                        head = head->alloc_next;
                }
                if(head == NULL){
                        ret = -EINVAL;
                        goto out;
                }
                regions[i].address = (u64)(uintptr_t)head->alloc_ptr;
                regions[i].size = (u64)head->alloc_len;
                regions[i].file_offset = data_cursor;
                regions[i].protection = head->alloc_flags;
                data_cursor = pe_align_value(data_cursor + regions[i].size, 16);
                head = head->alloc_next;
        }
        memcpy(dump_footer.magic, pe_dump_footer_magic, sizeof(dump_footer.magic));
        dump_footer.version = PE_DUMP_VERSION;
        dump_footer.region_count = region_count;
        dump_footer.header_offset = overlay_offset;

        ret = pe_write_at(dump_file, overlay_offset, &dump_header, sizeof(dump_header));
        if(ret == 0){
                ret = pe_write_at(dump_file, dump_header.region_table_offset, regions, (u64)region_count * sizeof(*regions));
        }
        for(u32 i = 0; ret == 0 && i < region_count; i++){
                ret = pe_write_at(dump_file, regions[i].file_offset, (void *)(uintptr_t)regions[i].address, regions[i].size);
        }
        if(ret != 0){
                goto out;
        }
        final_size = data_cursor + sizeof(dump_footer);
        ret = pe_write_at(dump_file, data_cursor, &dump_footer, sizeof(dump_footer));
        if(ret != 0){
                goto out;
        }
        end_position.QuadPart = final_size;
        if(!SetFilePointerEx(dump_file, end_position, NULL, FILE_BEGIN) || !SetEndOfFile(dump_file)){
                ret = -EIO;
                goto out;
        }

	out:
        pe_standalone_mode = 0;
        if(dump_file != INVALID_HANDLE_VALUE){
                CloseHandle(dump_file);
        }
        if(headers != NULL){
                free(headers);
        }
        if(regions != NULL){
                free(regions);
        }
        return ret;
}
#endif

#ifdef __linux__
/* crash_handler: returns a custom file format when it crash,
 * output file format is still wip, it should contains both the callback table and the offset table. */
void crash_handler(int sig){
	int nptrs = 0;
	int dump_fd = 0;
	int dump_fd_offset_count = 0;
	int sym_table_section_size = 0;
	u8  section_number = 0;
	time_t current_crash_time = 0;
	struct tm tm = {};
	u8 dumped_filename_buffer[128];
	struct crashdump_section crash_sections[CRASH_SECTION_MAX];
	char **backtrace_strings = NULL;
	void *buffer[BACKTRACE_MAX_DEPTH];

        #ifdef ENABLE_UNWIND_DEBUGGING
	unwind_backtrace();
        #else
	if(sig != SIGABRT){
		/* if we can use the memory allocation primitives, display the current stacktrace. */
		printf("[!] getting backtrace...\n");
		nptrs = backtrace(buffer, BACKTRACE_MAX_DEPTH);
		backtrace_strings = backtrace_symbols(buffer, nptrs);
		if (backtrace_strings == NULL) {
			printf("[!] failed to obtain backtrace..\n");
			goto after_backtrace;
		}else{
			for (int j = 0; j < nptrs; j++){
				printf("[%d] %s\n", j, backtrace_strings[j]);
			}
			free(backtrace_strings);
		}
	}
        #endif
	after_backtrace:
	if(sig != SIGABRT){
		current_crash_time = time(NULL);
		/* taken from 'https://stackoverflow.com/questions/1442116/how-can-i-get-the-date-and-time-values-in-a-c-program' */
		tm = *localtime(&current_crash_time);
		snprintf(dumped_filename_buffer,
			sizeof(dumped_filename_buffer),
			DUMP_FILENAME,
			tm.tm_year + 1900,
			tm.tm_mon + 1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec
		);
	}else{
		/* seems that when a SIGABORT is invoked, getting the current time makes my program crash again, this is just a temporary solution */
		strncpy(dumped_filename_buffer, "sigabort_crash.bin", sizeof("sigabort_crash.bin"));
	}

	dump_fd = open(dumped_filename_buffer, O_RDWR | O_CREAT, 0777);
	/* write the header: magic number, section numbers and the section table */
	write(dump_fd, DUMP_MAGIC, sizeof(DUMP_MAGIC));
	dump_fd_offset_count += sizeof(DUMP_MAGIC);
	lseek(dump_fd, dump_fd_offset_count, SEEK_SET);
	section_number = 1;
	write(dump_fd, &section_number, sizeof(section_number));
	dump_fd_offset_count += sizeof(section_number);
	lseek(dump_fd, dump_fd_offset_count, SEEK_SET);
	/* create the section table (for now has only 1 entry) */
	crash_sections[CRASH_SECTION_SYMBOL_TABLE].start_offset = dump_fd_offset_count + (sizeof(struct crashdump_section) * CRASH_SECTION_MAX);
	dump_fd_offset_count += sizeof(struct crashdump_section) * CRASH_SECTION_MAX;
	lseek(dump_fd, dump_fd_offset_count, SEEK_SET);

	/* copy the cb table into the new file */
	for (int j = 0; sym_table[j].function_ptr != NULL; j++) {
		lseek(dump_fd, dump_fd_offset_count, SEEK_SET);
		write(dump_fd, sym_table[j].function_name, strlen(sym_table[j].function_name));
		dump_fd_offset_count += strlen(sym_table[j].function_name);
		lseek(dump_fd, dump_fd_offset_count, SEEK_SET);
		write(dump_fd, &sym_table[j].function_ptr, sizeof(sym_table[j].function_ptr));
		dump_fd_offset_count += sizeof(sym_table[j].function_ptr);
		sym_table_section_size += sizeof(sym_table[j]);
	}
	/* return at the start of the section header */
	lseek(dump_fd, 0 + sizeof(DUMP_MAGIC) + sizeof(section_number), SEEK_SET);
	crash_sections[CRASH_SECTION_SYMBOL_TABLE].length = sym_table_section_size;
	write(dump_fd, &crash_sections, sizeof(struct crashdump_section) * CRASH_SECTION_MAX);
	close(dump_fd);
	printf("[%s_handler] dumped crash to '%s'\n", (sig == SIGINT) ? "sigint" : "crash", dumped_filename_buffer);
	if(sig != SIGABRT){
		finalize_director();
	}

	elf_generator_handler();
	exit(0);
}
#else
/* crash_handler: Windows version */
void crash_handler(int sig){

}
#endif

static void trim_inplace(unsigned char *line){
	unsigned char *end = NULL;
	unsigned char *start = NULL;

	if (line == NULL){
		goto quit;
	}

	start = line;

	while(*start && isspace((unsigned char)*start)){
		start++;
	}

	if(start != line){
		memmove(line, start, strlen(start) + 1);
	}

	/* exit if line is blank */
	if (*line == '\0'){
		goto quit;
	}

	end = line + strlen(line) - 1;
	while (end >= line && isspace((unsigned char)*end)) {
		*end = '\0';
		end--;
	}
	quit:
}

/* find_struct_by_name: helper function used for searching a strut given it's name */
static struct runtime_struct_layout *find_struct_by_name(struct runtime_struct *runtime_struct, unsigned char *name){
	if(name == NULL || runtime_struct == NULL){
		return NULL;
	}

	for (int i = 0; ; i++) {
		if (runtime_struct->structs[i].struct_name == NULL){
			break;
		}

		if (strcmp(runtime_struct->structs[i].struct_name, name) == 0){
			return &runtime_struct->structs[i];
		}
	}
	return NULL;
}

static struct struct_field *find_field_by_name(struct runtime_struct_layout *runtime_struct, unsigned char *name){
	if (runtime_struct == NULL || name == NULL){
		return NULL;
	}

	for (int i = 0; i < runtime_struct->struct_field_num; i++) {
		if (strcmp(runtime_struct->fields[i].field_name, name) == 0){
			return &runtime_struct->fields[i];
		}
	}
	return NULL;
}

/* handle_offset_config: parse the offset configuration and patch the offsets at runtime */
int handle_offset_config(unsigned char *offset_config_name){
	int ret = 0;
	int fd = 0;
	int nread = 0;
	int total = 0;
	int file_size = 0;
	unsigned char *cursor = NULL;
	unsigned char *config_buffer = NULL;
	struct runtime_struct_layout *current_layout = NULL;
	char line[48] = {};
	struct stat config_stats = {};

	fd = open(offset_config_name, O_RDONLY);
	if(fd < 0){
		ret = -EINVAL;
		return ret;
	}

	fstat(fd, &config_stats);

	file_size = config_stats.st_size;

	if(file_size == 0){
		ret = -EINVAL;
		close(fd);
		return ret;
	}

	config_buffer = malloc((size_t)file_size + 1);
	if (config_buffer == NULL){
		close(fd);
		ret = -ENOMEM;
		return ret;
	}

	/* copy the config file into the new allocated buffer */
	while (total < file_size) {
		nread = read(fd, config_buffer + total, file_size - total);
		if (nread < 0) {
			if (errno == EINTR){
				continue;
			}
			goto fail;
		}
		if (nread == 0){
			break;
		}
		total += nread;
	}
	config_buffer[total] = '\0';
	close(fd);
	cursor = config_buffer;

	while (*cursor) {
		unsigned char *line_start = cursor;
		unsigned char *line_end = NULL;
		unsigned char *separator = NULL;
		size_t len = 0;

		line_end = strchr(cursor, '\n');
		if (line_end) {
			*line_end = '\0';
			cursor = line_end + 1;
		}else{
			cursor += strlen(cursor);
		}

		len = strlen(line_start);
		if (len > 0 && line_start[len - 1] == '\r'){
			line_start[len - 1] = '\0';
		}

		trim_inplace(line_start);
		if (line_start[0] == '\0'){
			continue;
		}

		separator = strchr(line_start, ':');

		if (separator == NULL) {
			current_layout = find_struct_by_name(&runtime_generated_structs, line_start);
			continue;
		}

		if (current_layout == NULL) {
			continue;
		}

		*separator = '\0';
		{
			unsigned char *name_part = line_start;
			unsigned char *value_part = separator + 1;
			unsigned long v = 0;

			trim_inplace(name_part);
			trim_inplace(value_part);
			{
				unsigned char *endp = NULL;
				v = strtoul(value_part, &endp, 10);
				if (errno != 0 || endp == value_part || *endp != '\0') {
					errno = EINVAL;
					goto fail;
				}
			}

			if (strcmp(name_part, "$size") == 0) {
			if (v > 32767) {
				errno = EINVAL;
				goto fail;
			}

			current_layout->struct_size = (u16)v;
			continue;
		}
		{
			struct struct_field *field = NULL;

			if (v > 65535) {
				errno = EINVAL;
				goto fail;
			}

			field = find_field_by_name(current_layout, name_part);
			if (field == NULL) {
				continue;
			}
			field->field_offset = (unsigned short)v;
		}
		}
	}

	/* if everything went okay, set a special flags in the global context */
	director.offset_init_by_conf = true;

	fail:
	if(config_buffer != NULL){
		free(config_buffer);
	}
	if(fd > 0){
		close(fd);
	}
	return ret;
}

/* entry point: takes the args and start the interactive shell */
int main(int argc, char *argv[], char *envp[]) {
	int ret = 0;
	int fd = 0;
	char line[64] = {};
	unsigned char *drv_arg[5] = {};
	unsigned char *param_name = NULL;
	unsigned char *param_value = NULL;
	unsigned char *function_name = NULL;
	unsigned char *offset_config_name = NULL;

	struct cmd_table_entry dispatch_cmd_table[] = {
		[DRV_CMD_EXIT] =
		{
			.cmd_name = "exit",
			.cmd_args = 0,
			.cmd_ptr  = drv_cmd_exit,
		},
		[DRV_CMD_RESET] =
		{
			.cmd_name = "reset",
			.cmd_args = 0,
			.cmd_ptr  = drv_cmd_reset,
                },
		[DRV_CMD_LOAD] =
		{
			.cmd_name = "load",
			.cmd_args = 1,
			.cmd_ptr  = drv_cmd_load,
		},
		[DRV_CMD_UNLOAD] =
		{
			.cmd_name = "unload",
			.cmd_args = 1,
			.cmd_ptr  = drv_cmd_unload,
		},
		[DRV_CMD_START] =
		{
			.cmd_name = "start",
			.cmd_args = 0,
			.cmd_ptr  = drv_cmd_start,
		},
		[DRV_CMD_STOP] =
		{
			.cmd_name = "stop",
			.cmd_args = 1,
			.cmd_ptr  = drv_cmd_stop,
		},
		[DRV_CMD_RUN] =
		{
			.cmd_name = "run",
			.cmd_args = 1,
			.cmd_ptr = drv_cmd_run,
		},
		[DRV_CMD_SET] = {
			.cmd_name = "set",
			.cmd_args = 1,
			.cmd_ptr  = drv_cmd_set,
		},
		[DRV_CMD_GET] = {
			.cmd_name = "get",
			.cmd_args = 1,
			.cmd_ptr  = drv_cmd_get,
		},
		[DRV_CMD_OPS] = {
			.cmd_name = "ops",
			.cmd_args = 0xFF,
			.cmd_ptr  = drv_cmd_ops,
		},
	};
	/* register now the signal handler */
	signal(SIGSEGV, crash_handler);
	signal(SIGABRT, crash_handler);
	signal(SIGILL,  crash_handler);

	#ifdef _WIN64
	if(pe_standalone_mode == PE_STANDALONE_MARKER){
		ret = pe_entry_stub();
		if(ret != 0){
			return ret;
		}
		goto command_loop;
	}
	#endif

	director.internal_cb_table = &sym_table;
	director.envp = envp;
	ret = usb_runtime_init();
	if(ret != 0){
		return ret;
	}

	atomic_store(&director.libusb_thread_stop, false);
	pthread_create(&director.libusb_event_thread, NULL, libusb_thread, NULL);

	if(argc < 2){
		printf("./%s <module_name> ... <function_name>!\n", argv[0]);
		ret = -EINVAL;
		return ret;
	}

	/* start by searching for the offsets.conf file */
	for(u8 argv_idx = 1; argv[argv_idx] != NULL; argv_idx++){
		if(argv_idx != 1 && strstr(argv[argv_idx], ".conf") != NULL && strstr(argv[argv_idx], ".ko") == NULL){
			offset_config_name = argv[argv_idx];
			ret = handle_offset_config(offset_config_name);
			#ifdef ENABLE_DBG_PRINT
			struct runtime_struct *runtime_structs = NULL;
			struct struct_field *current_struct_fields = NULL;
			struct runtime_struct_layout *runtime_struct_layout = NULL;

			runtime_structs = &runtime_generated_structs;
			for (u16 counter = 0; counter < ARRAY_SIZE(runtime_structs->structs); counter++) {
				runtime_struct_layout = &runtime_structs->structs[counter];
				if(runtime_struct_layout->struct_field_num == 0){
					continue;
				}
				current_struct_fields = runtime_struct_layout->fields;
				for (u8 k = 0; k < runtime_struct_layout->struct_field_num; k++) {
					if(current_struct_fields[k].field_name == NULL){
						break;
					}
					printf("  - %s->%s offset: %d original struct size: %d\n",
						runtime_struct_layout->struct_name,
						current_struct_fields[k].field_name,
						current_struct_fields[k].field_offset,
						runtime_struct_layout->struct_size
					);
				}
			}
			#endif
			break;
		}
	}

	/* continue by searching for any other module param, mark it by searching for the '=' tag */
	for(u8 argv_idx = 1; argv[argv_idx] != NULL; argv_idx++){
		if(argv_idx != 1 && strstr(argv[argv_idx], "=") != NULL && strstr(argv[argv_idx], ".ko") == NULL){
			param_name  = strtok(argv[argv_idx], "=");
			param_value = strtok(NULL, "=");
			/* store the param name and its value for later use */
			
		}
	}
	/* add a fast loading option: if the user insert as argv 1 or more '.ko', load them in the order given */
	if(strstr(argv[1], ".ko") != NULL){
		for(u8 argv_idx = 1; argv[argv_idx] != NULL; argv_idx++){
			/* add a new fast option: if there is only 1 name without .ko, assume it is the function that needs to be executed */
			if(argv_idx != 1 && strstr(argv[argv_idx], ".ko") == NULL && strstr(argv[argv_idx], ".conf") == NULL && strstr(argv[argv_idx], "=") == NULL){
				function_name = argv[argv_idx];
				continue;
			}
			/* skip the .conf file */
			if(argv_idx != 1 && strstr(argv[argv_idx], ".ko") == NULL && strstr(argv[argv_idx], ".conf") != NULL){
				continue;
			}
			/* skip also the parameter */
			if(argv_idx != 1 && strstr(argv[argv_idx], ".ko") == NULL && strstr(argv[argv_idx], "=") != NULL){
				continue;
			}
			fd = open(argv[argv_idx], O_RDONLY);
			if(fd < 0){
				printf("[!] file '%s' not found!\n", argv[argv_idx]);
				break;
			}else{
				close(fd);
			}
			printf("[*] loading %s\n", argv[argv_idx]);
			ret = fast_drv_load(&director, argv[argv_idx]);
		}
		/* call the function if inserted by the user */
		if(function_name != NULL){
			ret = fast_execute_function(function_name);
		}
	}

	command_loop:
	while (true) {
		printf("> ");
		fflush(stdout);

		if (!fgets(line, sizeof(line), stdin)) {
			printf("\n");
			break;
		}

		ret = drv_parse_line(line, drv_arg, 5);
		if (ret == 0) {
			continue;
		}

		for (u8 counter = 0; counter < ARRAY_SIZE(dispatch_cmd_table); counter++) {
			ret = drv_execute_cmd(&director, drv_arg, &dispatch_cmd_table[counter]);
			if (ret == 0) {
				/* cmd executed correctly */
				break;
			}
		}
	}

	finalize_director();
	return ret;
}

#include <cb_table.h>
