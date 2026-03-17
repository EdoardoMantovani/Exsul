/*
 * Made by Edoardo Mantovani, March 2026
 * Simple program for detecting if the endpoints of the realtek usb wifi device will fail or not in rtw89
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <linux/usb/ch9.h>
#include <libusb-1.0/libusb.h>
#include <asm-generic/errno.h>
#include <asm-generic/param.h>
#include <asm-generic/errno-base.h>

#define u8	unsigned char
#define u16	unsigned short
#define u32	unsigned int

#define RTW89_USB_VID 0x0bda
#define RTW89_USB_PID 0xc832

#define RTW89_MAX_ENDPOINT_NUM	9
#define RTW89_MAX_BULKOUT_NUM	7

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
 * struct usb_host_endpoint - host-side endpoint descriptor and queue
 * @desc: descriptor for this endpoint, wMaxPacketSize in native byteorder
 * @ss_ep_comp: SuperSpeed companion descriptor for this endpoint
 * @ssp_isoc_ep_comp: SuperSpeedPlus isoc companion descriptor for this endpoint
 * @eusb2_isoc_ep_comp: eUSB2 isoc companion descriptor for this endpoint
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
	struct usb_endpoint_descriptor			desc;
	struct usb_ss_ep_comp_descriptor		ss_ep_comp;
	struct usb_ssp_isoc_ep_comp_descriptor		ssp_isoc_ep_comp;

	void						*hcpriv;
	struct ep_device				*ep_dev;	/* For sysfs info */

	unsigned char *extra;   /* Extra descriptors */
	int extralen;
	int enabled;
	int streams;
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
};

static int rtw89_usb_parse(void *unused, struct usb_interface *intf){
	struct usb_host_interface *host_interface = &intf->altsetting[0];
	struct usb_interface_descriptor *intf_desc = &host_interface->desc;
	struct usb_endpoint_descriptor *endpoint = NULL;
	int num_out_pipes = 0;
	u8 num;
	u8 rtwusb_in_pipe = 0;
	u8 rtwusb_out_pipe[RTW89_MAX_BULKOUT_NUM];

	if (intf_desc->bNumEndpoints > RTW89_MAX_ENDPOINT_NUM) {
		printf("found %d endpoints, expected %d max\n",
			  intf_desc->bNumEndpoints, RTW89_MAX_ENDPOINT_NUM);
		return -EINVAL;
	}

	for (int i = 0; i < intf_desc->bNumEndpoints; i++) {
		endpoint = &host_interface->endpoint[i].desc;
		num = usb_endpoint_num(endpoint);

		if (usb_endpoint_dir_in(endpoint) &&
		    usb_endpoint_xfer_bulk(endpoint)) {
			if (rtwusb_in_pipe) {
				printf("found more than 1 bulk in endpoint\n");
				return -EINVAL;
			}

			rtwusb_in_pipe = num;
		}

		if (usb_endpoint_dir_out(endpoint) &&
		    usb_endpoint_xfer_bulk(endpoint)) {
			if (num_out_pipes >= RTW89_MAX_BULKOUT_NUM) {
				printf("found more than %d bulk out endpoints\n", RTW89_MAX_BULKOUT_NUM);
				return -EINVAL;
			}

			rtwusb_out_pipe[num_out_pipes++] = num;
		}
	}

	if (num_out_pipes < 1) {
		printf("no bulk out endpoints found\n");
		return -EINVAL;
	}

	printf("!!! Everything fine !!!\n");
	return 0;
}

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

	for(u8 z = 0; z < intf->altsetting->desc.bNumEndpoints; z++){
		printf("[DUMP] endpoint %d length %d\n", z, intf->altsetting->endpoint[z].desc.bLength);
		printf("[DUMP] endpoint %d desc: %d\n", z, intf->altsetting->endpoint[z].desc.bDescriptorType);
		printf("[DUMP] endpoint %d ep_addr: %d\n", z, intf->altsetting->endpoint[z].desc.bEndpointAddress);
		printf("[DUMP] endpoint %d attributes: %d\n", z, intf->altsetting->endpoint[z].desc.bmAttributes);
		printf("[DUMP] endpoint %d max pkt size: %d\n", z, intf->altsetting->endpoint[z].desc.wMaxPacketSize);
		printf("[DUMP] endpoint %d interval: %d\n", z, intf->altsetting->endpoint[z].desc.bInterval);
		printf("[DUMP] endpoint %d refresh: %d\n", z, intf->altsetting->endpoint[z].desc.bRefresh);
		printf("[DUMP] endpoint %d sync address: %d\n", z, intf->altsetting->endpoint[z].desc.bSynchAddress);
	}
}

int parse_usb_and_alloc_intf(libusb_device_handle *handler, struct usb_interface **in_intf){
	int ret = 0;
	u8 in = 0;
	u8 ep_is_bulk = 0;
	u8 ep_is_intr = 0;
	u8 ep_is_ctrl = 0;
	u8 ep_is_isoc = 0;
	u8 total_ep_num = 0;
	libusb_device *dev = NULL;
	struct usb_interface *intf_ptr = NULL;
	const struct libusb_interface *intf = NULL;
	struct libusb_config_descriptor *cfg = NULL;
	const struct libusb_interface_descriptor *alt = NULL;
	const struct libusb_endpoint_descriptor *current_ep = NULL;

	dev = libusb_get_device(handler);

	if(dev == NULL){
		printf("[!] failed to acquire libusb_device!\n");
		ret = -EINVAL;
		return ret;
	}
	ret = libusb_get_active_config_descriptor(dev, &cfg);
	if(ret < 0){
		printf("[!] failed to get libusb_config_descriptor!\n");
		ret = -EINVAL;
		return ret;
	}
	printf("[*] number of interfaces detected: %d\n", cfg->bNumInterfaces);

	intf_ptr = (struct usb_interface *)malloc(sizeof(struct usb_interface));
	if(intf_ptr == NULL){
		printf("[!] failed to allocate usb_interface!\n");
		ret = -ENOMEM;
		libusb_free_config_descriptor(cfg);
		return ret;
	}
	memset(intf_ptr, 0, sizeof(struct usb_interface));

	intf_ptr->altsetting = (struct usb_host_interface *)malloc(sizeof(struct usb_host_interface));
	if(intf_ptr->altsetting == NULL){
		printf("[!] failed to allocate usb_host_interface!\n");
		free(intf_ptr);
		ret = -ENOMEM;
		libusb_free_config_descriptor(cfg);
		return ret;
	}
	memset(intf_ptr->altsetting, 0, sizeof(struct usb_host_interface));

	*in_intf = intf_ptr;

	for(u8 j = 0; j < cfg->bNumInterfaces; j++){
		intf = &cfg->interface[j];
		if(intf == NULL){
			printf("[!] libusb_interface is null...aborting!\n");
			ret = -EINVAL;
			libusb_free_config_descriptor(cfg);
			return ret;
		}else{
			printf("[*] interface %d has %d altsettings!\n", j, intf->num_altsetting);
		}

		intf_ptr->num_altsetting = intf->num_altsetting;
		intf_ptr->cur_altsetting = intf_ptr->altsetting;

		for(u8 k = 0; k < intf->num_altsetting; k++){
			alt = &intf->altsetting[k];
			if(alt == NULL){
				printf("[!] altsetting is null!\n");
				ret = -EINVAL;
				libusb_free_config_descriptor(cfg);
				return ret;
			}
			printf("[*] total number of endpoints: 0x%02x\n", alt->bNumEndpoints);
			intf_ptr->altsetting->desc.bLength = alt->bLength;
			intf_ptr->altsetting->desc.bDescriptorType = alt->bDescriptorType;
			intf_ptr->altsetting->desc.bInterfaceNumber = alt->bInterfaceNumber;
			intf_ptr->altsetting->desc.bAlternateSetting = alt->bAlternateSetting;
			intf_ptr->altsetting->desc.bNumEndpoints = alt->bNumEndpoints;
			intf_ptr->altsetting->desc.bInterfaceClass = alt->bInterfaceClass;
			intf_ptr->altsetting->desc.bInterfaceSubClass = alt->bInterfaceSubClass;
			intf_ptr->altsetting->desc.bInterfaceProtocol = alt->bInterfaceProtocol;
			intf_ptr->altsetting->desc.iInterface = alt->iInterface;

			total_ep_num = alt->bNumEndpoints;

			intf_ptr->altsetting->endpoint = calloc(total_ep_num, sizeof(struct usb_host_endpoint));
			if(intf_ptr->altsetting->endpoint == NULL){
				printf("[!] failed to allocate endpoints!\n");
				ret = -ENOMEM;
				libusb_free_config_descriptor(cfg);
				return ret;
			}

			for(u8 z = 0; z < alt->bNumEndpoints; z++){
				current_ep = &alt->endpoint[z];

				in = ((current_ep->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN);
				ep_is_bulk = ((current_ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK);
				ep_is_intr = ((current_ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT);
				ep_is_ctrl = ((current_ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_CONTROL);
				ep_is_isoc = ((current_ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_ISOC);
				printf("endpoint %d address: 0x%02x %s type ", z, current_ep->bEndpointAddress, in ? "IN" : "OUT");
				if(ep_is_bulk == true){
					printf("bulk\n");
				}else if(ep_is_intr == true){
					printf("interrupt\n");
				}else if(ep_is_ctrl == true){
					printf("control\n");
				}else if(ep_is_isoc == true){
					printf("isochronous\n");
				}

				intf_ptr->altsetting->endpoint[z].desc.bLength = current_ep->bLength;
				intf_ptr->altsetting->endpoint[z].desc.bDescriptorType = current_ep->bDescriptorType;
				intf_ptr->altsetting->endpoint[z].desc.bEndpointAddress = current_ep->bEndpointAddress;
				intf_ptr->altsetting->endpoint[z].desc.bmAttributes = current_ep->bmAttributes;
				intf_ptr->altsetting->endpoint[z].desc.wMaxPacketSize = current_ep->wMaxPacketSize;
				intf_ptr->altsetting->endpoint[z].desc.bInterval = current_ep->bInterval;
				intf_ptr->altsetting->endpoint[z].desc.bRefresh = current_ep->bRefresh;
				intf_ptr->altsetting->endpoint[z].desc.bSynchAddress = current_ep->bSynchAddress;
			}
		}
	}

	dump_usb_host_interface(*in_intf);
	/* *really final step*: check if the rtw89 function accept the converted kernel struct or not. */
	rtw89_usb_parse(NULL, *in_intf);
	libusb_free_config_descriptor(cfg);
	return ret;
}

int main(int argc, char *argv[]){
	int ret = 0;
	int usb_interface_number = 0;
	struct usb_interface *intf = NULL;
	libusb_device_handle *handler = NULL;

	libusb_init(NULL);

	handler = libusb_open_device_with_vid_pid(NULL, RTW89_USB_VID, RTW89_USB_PID);
	if(handler == NULL){
		printf("[!] device not present in the system!\n");
		goto end;
	}

	usb_interface_number = parse_usb_and_alloc_intf(handler, &intf);

	end:
	if(handler != NULL){
		libusb_close(handler);
	}
	libusb_exit(NULL);
	return ret;
}

