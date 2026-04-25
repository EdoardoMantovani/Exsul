/* made by Edoardo Mantovani, 2026 */
#include <layer.h>
#include <ieee80211.h>

#define SYM_ENTRIES	 4096
struct internal_cb_table sym_table[SYM_ENTRIES];
struct module_controller director;

struct cmd_table_entry {
	u8 cmd_name[18];
	u8 cmd_args;
	int (*cmd_ptr)(struct module_controller *, char **, int);
};

/* include here the headers for the various supported architectures */
#include <arm64/arm64.h>
#include <x86_64/x86_64.h>

/* structures and definitions used for representing the command table used by
 * the main program.
 */
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
		printf("[!] failed to get the usb_table_size symbol!\n");
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
		printf("[!] usb device table symbol isn't aligned with the size of \'struct usb_device_id\', aborting...\n");
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
		printf("[DUMP] endpoint %d length %d\n", k, intf->altsetting->endpoint[k].desc.bLength);
		printf("[DUMP] endpoint %d desc: %d\n", k, intf->altsetting->endpoint[k].desc.bDescriptorType);
		printf("[DUMP] endpoint %d ep_addr: %d\n", k, intf->altsetting->endpoint[k].desc.bEndpointAddress);
		printf("[DUMP] endpoint %d attributes: %d\n", k, intf->altsetting->endpoint[k].desc.bmAttributes);
		printf("[DUMP] endpoint %d max pkt size: %d\n", k, intf->altsetting->endpoint[k].desc.wMaxPacketSize);
		printf("[DUMP] endpoint %d interval: %d\n", k, intf->altsetting->endpoint[k].desc.bInterval);
		printf("[DUMP] endpoint %d refresh: %d\n", k, intf->altsetting->endpoint[k].desc.bRefresh);
		printf("[DUMP] endpoint %d sync address: %d\n", k, intf->altsetting->endpoint[k].desc.bSynchAddress);
	}
}
#endif

int parse_usb_and_alloc_intf(libusb_device_handle *handler, struct usb_interface **in_intf) {
        int ret                                         = 0;
        int int_number                                  = 0;
        u32 struct_usb_interface_size                   = 0;
        u32 struct_usb_device_size                      = 0;
        u32 struct_device_size                          = 0;
        u8  num_altsetting                              = 0;
        u8  num_ep                                      = 0;
        libusb_device                       *libusb_dev = NULL;
	struct libusb_config_descriptor     *cfg        = NULL;
	const struct libusb_interface       *libusb_intf= NULL;
	struct usb_interface                *intf_ptr   = NULL;
	struct usb_device                   *udev_ptr   = NULL;
	const struct libusb_interface_descriptor *alt   = NULL;
	const struct libusb_endpoint_descriptor  *ep    = NULL;

	struct libusb_device_descriptor dev_desc	= {};

	if (handler == NULL || in_intf == NULL) {
                return -EINVAL;
        }

        *in_intf = NULL;

        libusb_dev = libusb_get_device(handler);
        if (libusb_dev == NULL) {
                printf("[!] failed to acquire libusb_device!\n");
                return -EINVAL;
        }

	ret = libusb_get_active_config_descriptor(libusb_dev, &cfg);
        if (ret < 0) {
                printf("[!] failed to get libusb_config_descriptor!\n");
                return -EINVAL;
        }

	if (cfg->bNumInterfaces == 0) {
                printf("[!] device has no interfaces!\n");
                ret = -ENODEV;
                goto fail_free_cfg;
        }

        struct_usb_interface_size = director.struct_sizes.usb_interface;
        struct_usb_device_size    = director.struct_sizes.usb_device;
        struct_device_size        = director.struct_sizes.device;

	if (struct_usb_interface_size == 0) {
                printf("[!] failed to retrieve size for struct usb_interface!\n");
                ret = -EINVAL;
                goto fail_free_cfg;
        }
        if (struct_usb_device_size == 0) {
                printf("[!] failed to retrieve size for struct usb_device!\n");
                ret = -EINVAL;
                goto fail_free_cfg;
        }
        if (struct_device_size == 0) {
                printf("[!] failed to retrieve size for struct device!\n");
                ret = -EINVAL;
                goto fail_free_cfg;
        }

	intf_ptr = (struct usb_interface *)calloc(1, struct_usb_interface_size);
        if (intf_ptr == NULL) {
                printf("[!] failed to allocate usb_interface!\n");
                ret = -ENOMEM;
                goto fail_free_cfg;
        }

	libusb_intf = &cfg->interface[0];
	num_altsetting = (u8)libusb_intf->num_altsetting;

#ifdef ADVANCED_USB_DEBUGGING
        printf("[*] interface 0 has %d altsetting(s)\n", num_altsetting);
#endif

        if (num_altsetting == 0) {
                printf("[!] interface 0 has no altsettings!\n");
                ret = -ENODEV;
                goto fail_free_intf;
        }

        intf_ptr->altsetting = (struct usb_host_interface *)calloc(num_altsetting, sizeof(struct usb_host_interface));
        if (intf_ptr->altsetting == NULL) {
                printf("[!] failed to allocate usb_host_interface array!\n");
                ret = -ENOMEM;
                goto fail_free_intf;
        }
	intf_ptr->num_altsetting = num_altsetting;

        for (u8 k = 0; k < num_altsetting; k++) {
                alt = &libusb_intf->altsetting[k];

                num_ep = alt->bNumEndpoints;

		#ifdef ADVANCED_USB_DEBUGGING
                printf("[*] altsetting %u: bInterfaceNumber=%u bNumEndpoints=%u\n",
                       k, alt->bInterfaceNumber, num_ep);
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
                printf("[!] failed to allocate usb_device!\n");
                ret = -ENOMEM;
                goto fail_free_endpoints;
        }

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
		printf("[!] failed to get libusb device descriptor, defaulting bNumConfigurations=1\n");
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

/* main function for detecting the attached usb dongle, requires a full 'usb_dev_list' table */
int detect_usb_device(const struct usb_device_id *id_table, struct usb_interface **intf_out, const struct usb_device_id **matched_id_out){
	int ret = 0;
	struct usb_interface *usb_intf = NULL;

	for (struct usb_device_id *p = id_table; p && p->idVendor && p->idProduct; p++) {
		director.usb_dev = libusb_open_device_with_vid_pid(NULL, p->idVendor, p->idProduct);
		if (director.usb_dev == NULL){
			continue;
		}

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
			libusb_close(director.usb_dev);
			director.usb_dev = NULL;
			continue;
		}

		*intf_out = usb_intf;
		*matched_id_out = p;

		return ret;
	}
	ret = -ENODEV;
	return ret;
}

int invoke_usb_probe(struct usb_driver *new_driver, struct usb_device_id *id, struct usb_interface *intf){
	int ret = 0;
	int (*probe) (struct usb_interface *intf, const struct usb_device_id *id) = NULL;
	probe = new_driver->probe;

	/* in normal conditions here a mutex would have been implemented, but in this case there is no necessity */
	probe(intf, id);

	return ret;
}

/* Read the DW_AT_data_member_location of a DWARF member DIE. */
static Dwarf_Unsigned dwarf_get_member_offset(Dwarf_Debug dbg, Dwarf_Die member_die){
	Dwarf_Error err = 0;
	Dwarf_Attribute attr = 0;
	Dwarf_Unsigned offset = 0;
	Dwarf_Unsigned exprlen = 0;
	char *block = NULL;

	if (dwarf_attr(member_die, DW_AT_data_member_location, &attr, &err) != DW_DLV_OK)
		return 0;
	if (dwarf_formudata(attr, &offset, &err) != DW_DLV_OK) {
		if (dwarf_formexprloc(attr, &exprlen, &block, &err) == DW_DLV_OK && exprlen > 0 && block) {
			if (block[0] == DW_OP_plus_uconst || block[0] == DW_OP_constu){
				offset = read_uleb128(block + 1, (size_t)exprlen - 1);
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
						if (dwarf_offdie(dbg, type_off, &type_die, &err) == DW_DLV_OK) {
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
		sibling = dwarf_siblingof(dbg, cur, &sib, &err);
		dwarf_dealloc(dbg, cur, DW_DLA_DIE);
		if (sibling != DW_DLV_OK){
			break;
		}
		cur = sib;
	}

	ret = -ENOENT;
	return ret;
}

int dwarf_search_field(unsigned char *field_input_name, Dwarf_Debug dbg, Dwarf_Die struct_die){
	int ret = 0;
	ret = dwarf_search_field_recursive(field_input_name, dbg, struct_die, 0);
	return ret;
}

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

		ret = dwarf_siblingof(dbg, cur, &sib, &err);
		dwarf_dealloc(dbg, cur, DW_DLA_DIE);
		if (ret != DW_DLV_OK){
			break;
		}
		cur = sib;
	}
	ret = 0;
	return ret;
}

int dwarf_loader_parser(unsigned char *struct_to_search, unsigned char *field_to_search, unsigned short *field_offset, unsigned short *out_size){
	int ret = 0;
	int fd = -1;
	Dwarf_Debug dbg = 0;
	Dwarf_Error err = 0;

	Dwarf_Unsigned cu_header_len = 0;
	Dwarf_Unsigned abbrev_off = 0;
	Dwarf_Unsigned next_cu = 0;
	Dwarf_Half version = 0;
	Dwarf_Half addr_size = 0;

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

	ret = dwarf_init(fd, DW_DLC_READ, NULL, NULL, &dbg, &err);
	if (ret != DW_DLV_OK) {
		close(fd);
		ret = -EINVAL;
		return ret;
	}

	while (dwarf_next_cu_header(dbg, &cu_header_len, &version, &abbrev_off, &addr_size, &next_cu, &err) == DW_DLV_OK) {
		cu_die = 0;
		ret = dwarf_siblingof(dbg, NULL, &cu_die, &err);
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

int parse_module_offset_table(void){
	int ret = 0;
	u8 counter = 0;
	struct runtime_struct *runtime_structs = NULL;
	struct runtime_struct_layout *runtime_struct_layout = NULL;
	struct struct_field *current_struct_fields = NULL;

	runtime_structs = &runtime_generated_structs;

	if(runtime_structs == NULL){
		printf("[!] no runtime struct data!\n");
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
			if(ret > 0 && director.global_conf.show_struct_offset == true){
				printf("  - %s->%s offset: %d original struct size: %d\n",
					runtime_struct_layout->struct_name,
					current_struct_fields[k].field_name,
					current_struct_fields[k].field_offset,
					runtime_struct_layout->struct_size
				);
			}
		}
	}

	dwarf_finish(director.dbg, &director.err);
	return ret;
}

#ifdef ENABLE_DBG_PRINT
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

/* start of the core kernel functions related to the loading of lkm */
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

static char *get_next_modinfo(const struct load_info *info, const char *tag,
			      char *prev) {
	unsigned char *p      = NULL;
	unsigned int   taglen = strlen(tag);
	Elf(Shdr) *infosec    = &info->sechdrs[info->index.info];
	unsigned long size    = infosec->sh_size;

	char *modinfo = (char *)info->hdr + infosec->sh_offset;

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
	unsigned int i		   = 0;
	Elf(Shdr) *shdr		   = NULL;
	Elf(Shdr) *strhdr	   = NULL;
	int	     err	   = 0;
	unsigned int num_mod_secs  = 0;
	unsigned int mod_idx	   = 0;
	unsigned int num_info_secs = 0;
	unsigned int info_idx	   = 0;
	unsigned int num_sym_secs  = 0;
	unsigned int sym_idx	   = 0;

	if (info->hdr->e_shentsize != sizeof(Elf(Shdr))) {
		printf("Invalid ELF section header size\n");
		goto no_exec;
	}

	info->sechdrs = (void *)info->hdr + info->hdr->e_shoff;

	if (info->hdr->e_shstrndx == SHN_UNDEF ||
	    info->hdr->e_shstrndx >= info->hdr->e_shnum) {
		printf("Invalid ELF section name index: %d || e_shstrndx (%d) "
		       ">= e_shnum (%d)\n",
		       info->hdr->e_shstrndx, info->hdr->e_shstrndx,
		       info->hdr->e_shnum);
		goto no_exec;
	}

	strhdr = &info->sechdrs[info->hdr->e_shstrndx];

	if (strhdr->sh_offset == 0x00) {
		printf("sh offset of string header points to 0!\n");
		goto no_exec;
	}

	info->secstrings = (void *)info->hdr + strhdr->sh_offset;

	if (strhdr->sh_size == 0) {
		printf("empty section name table\n");
		goto no_exec;
	}
	if (info->secstrings[strhdr->sh_size - 1] != '\0') {
		printf("ELF Spec violation: section name table isn't null "
		       "terminated\n");
		goto no_exec;
	}

	if (info->sechdrs[0].sh_type != SHT_NULL ||
	    info->sechdrs[0].sh_size != 0 || info->sechdrs[0].sh_addr != 0) {
		printf("ELF Spec violation: section 0 type(%d)!=SH_NULL or "
		       "non-zero len or addr\n",
		       info->sechdrs[0].sh_type);
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

			if (shdr->sh_link == SHN_UNDEF ||
			    shdr->sh_link >= info->hdr->e_shnum) {
				printf("Invalid ELF sh_link!=SHN_UNDEF(%d) or "
				       "(sh_link(%d) >= hdr->e_shnum(%d)\n",
				       shdr->sh_link, shdr->sh_link,
				       info->hdr->e_shnum);
				goto no_exec;
			}

			num_sym_secs++;
			sym_idx = i;
			fallthrough;
		default:
			if (strcmp(info->secstrings + shdr->sh_name,
				   ".gnu.linkonce.this_module") == 0) {
				num_mod_secs++;
				mod_idx = i;
#ifdef ENABLE_DBG_PRINT
				printf("detected .gnu.linkonce.this_module\n");
#endif
			} else if (strcmp(info->secstrings + shdr->sh_name,
					  ".modinfo") == 0) {
				num_info_secs++;
				info_idx = i;
#ifdef ENABLE_DBG_PRINT
				printf("detected .modinfo\n");
#endif
			}
			if (shdr->sh_flags & SHF_ALLOC) {
				if (shdr->sh_name >= strhdr->sh_size) {
					printf("Invalid ELF section name in "
					       "module (section %u type %u)\n",
					       i, shdr->sh_type);
					return -ENOEXEC;
				}
			}
			break;
		}
	}

	if (num_info_secs > 1) {
		#ifdef ENABLE_DBG_PRINT
		printf("Only one .modinfo section must exist.\n");
		#endif
		goto no_exec;
	} else if (num_info_secs == 1) {
		/* Try to find a name early so we can log errors with a module
		 * name */
		info->index.info = info_idx;
		info->name	 = get_modinfo(info, "name");
	}

	info->index.sym = sym_idx;
	shdr		= &info->sechdrs[sym_idx];
	info->index.str = shdr->sh_link;
	info->strtab	= (unsigned char *)info->hdr +
		       info->sechdrs[info->index.str].sh_offset;

	if (num_mod_secs != 1) {
		#ifdef ENABLE_DBG_PRINT
		printf("module %s: Only one .gnu.linkonce.this_module section "
		       "must exist.\n",
		       info->name
			   ?: "(missing .modinfo section or name field)");
		#endif
		goto no_exec;
	}

	shdr = &info->sechdrs[mod_idx];

	if (shdr->sh_type == SHT_NOBITS) {
		#ifdef ENABLE_DBG_PRINT
		printf("module %s: .gnu.linkonce.this_module section must have "
		       "a size set\n",
		       info->name
			   ?: "(missing .modinfo section or name field)");
		#endif
		goto no_exec;
	}

	if (!(shdr->sh_flags & SHF_ALLOC)) {
		#ifdef ENABLE_DBG_PRINT
		printf("module %s: .gnu.linkonce.this_module must occupy "
		       "memory during process execution\n",
		       info->name
			   ?: "(missing .modinfo section or name field)");
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

	return 0;

no_exec:
	return -ENOEXEC;
}

static int rewrite_section_headers(struct load_info *info, int flags) {
	Elf(Shdr) *shdr		 = NULL;
	info->sechdrs[0].sh_addr = 0;

	for (unsigned int i = 1; i < info->hdr->e_shnum; i++) {
		shdr	      = &info->sechdrs[i];
		shdr->sh_addr = (size_t)info->hdr + shdr->sh_offset;
	}

	info->sechdrs[info->index.vers].sh_flags &= ~(unsigned long)SHF_ALLOC;
	info->sechdrs[info->index.info].sh_flags &= ~(unsigned long)SHF_ALLOC;

	return 0;
}

static int early_mod_check(struct load_info *info, int flags) {
	int err = 0;
	err	= rewrite_section_headers(info, flags);

	return err;
}

static inline bool within_module_mem_type(unsigned long	       addr,
					  const struct module *mod,
					  enum mod_mem_type    type) {
	unsigned long base = 0;
	unsigned long size;

	base = (unsigned long)mod->mem[type].base;
	size = mod->mem[type].size;
	return addr - base < size;
}

int module_enforce_rwx_sections(Elf(Ehdr) * hdr, Elf(Shdr) * sechdrs,
				char *secstrings, struct module *mod) {
	const unsigned long shf_wx = SHF_WRITE | SHF_EXECINSTR;

	for (int i = 0; i < hdr->e_shnum; i++) {
		if ((sechdrs[i].sh_flags & shf_wx) == shf_wx) {
			#ifdef ENABLE_DBG_PRINT
			printf("%s: section %s (index %d) has invalid "
			       "WRITE|EXEC flags\n",
			       mod->name, secstrings + sechdrs[i].sh_name, i);
			#endif
			return -ENOEXEC;
		}
	}
	return 0;
}

static unsigned int find_sec(const struct load_info *info, const char *name) {
	Elf(Shdr) *shdr = NULL;
	for (int i = 1; i < info->hdr->e_shnum; i++) {
		shdr = &info->sechdrs[i];
		/* Alloc bit cleared means "ignore it." */
		if ((shdr->sh_flags & SHF_ALLOC) &&
		    strcmp(info->secstrings + shdr->sh_name, name) == 0) {
			return i;
		}
	}
	return 0;
}

bool module_init_section(const char *name) { return strstarts(name, ".init"); }

bool module_exit_section(const char *name) { return strstarts(name, ".exit"); }

bool module_init_layout_section(const char *sname) {
	return module_init_section(sname);
}

long module_get_offset_and_type(struct module *mod, enum mod_mem_type type,
				Elf(Shdr) * sechdr, unsigned int      section) {
	long offset = 0;
	long mask   = 0;

	mask = ((unsigned long)(type)&SH_ENTSIZE_TYPE_MASK)
	       << SH_ENTSIZE_TYPE_SHIFT;

	mod->mem[type].size += 0;
	offset = ALIGN(mod->mem[type].size, sechdr->sh_addralign ?: 1);
	mod->mem[type].size = offset + sechdr->sh_size;

	#ifdef ENABLE_DBG_PRINT
	printf("[*] %s: offset: %d section size: %d final size: %d\n", __FUNCTION__, offset, sechdr->sh_size, mod->mem[type].size);
	#endif
	return offset | mask;
}

static void __layout_sections(struct module *mod, struct load_info *info,
			      bool is_init) {
	unsigned int	  m = 0;
	unsigned int	  i = 0;
	enum mod_mem_type type;
	Elf(Shdr) *s	  = NULL;
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

			s->sh_entsize =
			    module_get_offset_and_type(mod, type, s, i);
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
	printf("Core section allocation order for %s:\n", mod->name);
#endif
	__layout_sections(mod, info, false);

#ifdef ENABLE_DBG_PRINT
	printf("Init section allocation order for %s:\n", mod->name);
#endif
	__layout_sections(mod, info, true);
	return;
}

static int module_memory_alloc(struct module *mod, enum mod_mem_type type) {
	unsigned int size = PAGE_ALIGN(mod->mem[type].size);
	unsigned int mem_flags = PROT_READ | PROT_WRITE;
	void	    *ptr;

	mod->mem[type].size = size;
#ifdef ENABLE_DBG_PRINT
	printf("[*] module memory alloc %d bytes for type '%s'\n", size, mem_type_to_string(type));
#endif
	if(type == MOD_TEXT || type == MOD_INIT_TEXT){
		mem_flags |= PROT_EXEC;
	}
	ptr = mmap(NULL, size, mem_flags,
		   MAP_PRIVATE | MAP_ANON | MAP_32BIT, -1, 0x00);

	if (ptr == MAP_FAILED) {
		return -ENOMEM;
	}
	director.module_allocation_list = new_allocation(ALLOCATION_TYPE_MMAP, ptr, size, mem_flags, PT_LOAD, director.module_allocation_list);
	memset(ptr, 0, size);
	mod->mem[type].base = ptr;

	return 0;
}

static void module_memory_free(struct module *mod, enum mod_mem_type type,
			       bool unload_codetags) {
	void *ptr = mod->mem[type].base;

	if (mod_mem_type_is_core_data(type)) {
		return;
	}
}

static int move_module(struct module *mod, struct load_info *info) {
	int		  ret  = ENOMEM;
	int		  i    = 0;
	enum mod_mem_type t    = 0;
	enum mod_mem_type type = 0;
	void		 *dest = NULL;
	Elf(Shdr) *shdr	       = NULL;

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
	printf("Final section addresses for %s:\n", mod->name);
#endif
	for (i = 0; i < info->hdr->e_shnum; i++) {
		dest = NULL;
		shdr = &info->sechdrs[i];
		type = shdr->sh_entsize >> SH_ENTSIZE_TYPE_SHIFT;

		if (!(shdr->sh_flags & SHF_ALLOC)) {
			continue;
		}

		dest = mod->mem[type].base +
		       (shdr->sh_entsize & SH_ENTSIZE_OFFSET_MASK);

		if (shdr->sh_type != SHT_NOBITS) {
			memcpy(dest, (void *)shdr->sh_addr, shdr->sh_size);
		}
		shdr->sh_addr = (unsigned long)dest;
#ifdef ENABLE_DBG_PRINT
		printf("\t0x%lx 0x%.8lx %s\n", (long)shdr->sh_addr,
		       (long)shdr->sh_size, info->secstrings + shdr->sh_name);
#endif
	}

	return 0;
out_enomem:
	for (t--; t >= 0; t--) {
		module_memory_free(mod, t, true);
	}
	return ret;
}

static bool is_core_symbol(const Elf(Sym) * src, const Elf(Shdr) * sechdrs,
			   unsigned int shnum, unsigned int pcpundx) {
	const Elf(Shdr) * sec;
	enum mod_mem_type type;

	if (src->st_shndx == SHN_UNDEF || src->st_shndx >= shnum ||
	    !src->st_name) {
		return false;
	}

	sec  = sechdrs + src->st_shndx;
	type = sec->sh_entsize >> SH_ENTSIZE_TYPE_SHIFT;
	if (!(sec->sh_flags & SHF_ALLOC) || mod_mem_type_is_init(type)) {
		return false;
	}

	return true;
}

void layout_symtab(struct module *mod, struct load_info *info) {
	Elf(Shdr) *symsect = (Elf(Shdr) *)(info->sechdrs + info->index.sym);
	Elf(Shdr) *strsect = (Elf(Shdr) *)(info->sechdrs + info->index.str);

	Elf(Sym) *src				= NULL;
	unsigned int	      i			= 0;
	unsigned int	      nsrc		= 0;
	unsigned int	      ndst		= 0;
	unsigned int	      strtab_size	= 0;
	struct module_memory *mod_mem_data	= &mod->mem[MOD_DATA];
	struct module_memory *mod_mem_init_data = &mod->mem[MOD_INIT_DATA];

	symsect->sh_flags |= SHF_ALLOC;
	symsect->sh_entsize = module_get_offset_and_type(
	    mod, MOD_INIT_DATA, symsect, info->index.sym);
	#ifdef ENABLE_DBG_PRINT
	printf("\t%s\n", info->secstrings + symsect->sh_name);
	#endif

	src  = (void *)info->hdr + symsect->sh_offset;
	nsrc = symsect->sh_size / sizeof(*src);

	for (ndst = i = 0; i < nsrc; i++) {
		if (i == 0 ||
		    is_core_symbol(src + i, info->sechdrs, info->hdr->e_shnum,
				   info->index.pcpu)) {
			strtab_size +=
			    strlen(&info->strtab[src[i].st_name]) + 1;
			ndst++;
		}
	}

	info->symoffs = ALIGN(mod_mem_data->size, symsect->sh_addralign ?: 1);
	info->stroffs = mod_mem_data->size =
	    info->symoffs + ndst * sizeof(Elf(Sym));
	mod_mem_data->size += strtab_size;
	info->core_typeoffs = mod_mem_data->size;
	mod_mem_data->size += ndst * sizeof(char);

	strsect->sh_flags |= SHF_ALLOC;
	strsect->sh_entsize = module_get_offset_and_type(
	    mod, MOD_INIT_DATA, strsect, info->index.str);
	#ifdef ENABLE_DBG_PRINT
	printf("\t%s\n", info->secstrings + strsect->sh_name);
	#endif

	mod_mem_init_data->size =
	    ALIGN(mod_mem_init_data->size, __alignof__(struct mod_kallsyms));
	info->mod_kallsyms_init_off = mod_mem_init_data->size;

	mod_mem_init_data->size += sizeof(struct mod_kallsyms);
	info->init_typeoffs = mod_mem_init_data->size;
	mod_mem_init_data->size += nsrc * sizeof(char);
}

static struct module *layout_and_allocate(struct load_info *info, int flags) {
	struct module *mod = NULL;
	unsigned int   ndx = 0;
	int	       err = 0;

	err = module_frob_arch_sections(info->hdr, info->sechdrs,
					info->secstrings, info->mod);

	if (err < 0) {
		return NULL;
	}

	err = module_enforce_rwx_sections(info->hdr, info->sechdrs,
					  info->secstrings, info->mod);

	if (err < 0) {
		return NULL;
	}

	info->sechdrs[info->index.pcpu].sh_flags &= ~(unsigned long)SHF_ALLOC;

	ndx = find_sec(info, ".data..ro_after_init");

	if (ndx) {
		#ifdef ENABLE_DBG_PRINT
		printf("section %s found!\n", ".data..ro_after_init");
		#endif
		info->sechdrs[ndx].sh_flags |= SHF_RO_AFTER_INIT;
	}

	ndx = find_sec(info, "__jump_table");

	if (ndx) {
		#ifdef ENABLE_DBG_PRINT
		printf("section %s found!\n", "__jump_table");
		#endif
		info->sechdrs[ndx].sh_flags |= SHF_RO_AFTER_INIT;
	}

	layout_sections(info->mod, info);

	layout_symtab(info->mod, info);

	err = move_module(info->mod, info);
	if (err) {
		printf("failed to move module!\n");
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

static void *section_objs(const struct load_info *info, const char *name,
			  size_t object_size, unsigned int *num) {
	unsigned int sec = find_sec(info, name);

	/* Section 0 has sh_addr 0 and sh_size 0. */
	*num = info->sechdrs[sec].sh_size / object_size;
	return (void *)info->sechdrs[sec].sh_addr;
}

static void *section_addr(const struct load_info *info, const char *name) {
	/* Section 0 has sh_addr 0. */
	return (void *)info->sechdrs[find_sec(info, name)].sh_addr;
}

void *find_runtime_section(struct module *mod, struct load_info *info,
			   unsigned char *section_name) {
	int		  ret  = 0;
	unsigned int	  i    = 0;
	enum mod_mem_type type = 0;
	void		 *dest = NULL;
	Elf(Shdr) *shdr	       = NULL;

	for (i = 0; i < info->hdr->e_shnum; i++) {
		dest = NULL;
		shdr = &info->sechdrs[i];
		type = shdr->sh_entsize >> SH_ENTSIZE_TYPE_SHIFT;

		if (!(shdr->sh_flags & SHF_ALLOC)) {
			continue;
		}

		dest = mod->mem[type].base +
		       (shdr->sh_entsize & SH_ENTSIZE_OFFSET_MASK);

		if (strcmp((unsigned char *)(info->secstrings + shdr->sh_name),
			   section_name) == 0) {
			return (void *)shdr->sh_addr;
		}
	}
out_enomem:
	return NULL;
}

static int find_module_sections(struct module *mod, struct load_info *info) {
	int ret = 0;
	mod->kp = section_objs(info, "__param", sizeof(*mod->kp), &mod->num_kp);
	mod->syms =
	    section_objs(info, "__ksymtab", sizeof(*mod->syms), &mod->num_syms);
	mod->crcs     = section_addr(info, "__kcrctab");
	mod->gpl_syms = section_objs(
	    info, "__ksymtab_gpl", sizeof(*mod->gpl_syms), &mod->num_gpl_syms);
	mod->gpl_crcs = section_addr(info, "__kcrctab_gpl");

#ifdef CONFIG_CONSTRUCTORS
	mod->ctors =
	    section_objs(info, ".ctors", sizeof(*mod->ctors), &mod->num_ctors);
	if (!mod->ctors) {
		mod->ctors = section_objs(info, ".init_array",
					  sizeof(*mod->ctors), &mod->num_ctors);
	} else if (find_sec(info, ".init_array")) {
		printf("%s: has both .ctors and .init_array.\n", mod->name);
		ret = -EINVAL;
		return ret;
	}
#endif

	mod->noinstr_text_start =
	    section_objs(info, ".noinstr.text", 1, &mod->noinstr_text_size);

	mod->extable = section_objs(info, "__ex_table", sizeof(*mod->extable),
				    &mod->num_exentries);

	if (section_addr(info, "__obsparm")) {
		#ifdef ENABLE_DBG_PRINT
		printf("%s: Ignoring obsolete parameters\n", mod->name);
		#endif
	}

	#ifdef ENABLE_DYNDBG
	mod->dyndbg_info.descs =
	    section_objs(info, "__dyndbg", sizeof(*mod->dyndbg_info.descs),
			 &mod->dyndbg_info.num_descs);
	mod->dyndbg_info.classes = section_objs(
	    info, "__dyndbg_classes", sizeof(*mod->dyndbg_info.classes),
	    &mod->dyndbg_info.num_classes);
	#endif
	return ret;
}

static int simplify_symbols(struct module *mod, struct load_info *info) {
	Elf(Shdr) *symsec = (Elf(Shdr) *)&info->sechdrs[info->index.sym];
	Elf(Sym) *sym	  = (void *)symsec->sh_addr;
	unsigned long	      secbase	 = 0;
	unsigned int	      i		 = 0;
	int		      ret	 = 0;
	struct kernel_symbol *ksym	 = NULL;
	void		     *dlopen_ctx = NULL;
	void                 *trampoline = NULL;
	unsigned char	     *name	 = NULL;

	for (i = 1; i < symsec->sh_size / sizeof(Elf(Sym)); i++) {
		name = (unsigned char *)(info->strtab + sym[i].st_name);

		switch (sym[i].st_shndx) {
		case SHN_COMMON:
			if (!strncmp(name, "__gnu_lto", 9)) {
				break;
			}
#ifdef ENABLE_DBG_PRINT
			printf("Common symbol: %s\n", name);
#endif
			ret = -ENOEXEC;
			break;
		case SHN_ABS:
#ifdef ENABLE_DBG_PRINT
			printf("Absolute symbol:  0x%08lx %s\n",
			       (long)sym[i].st_value, name);
#endif
			break;
		case SHN_UNDEF:
			void *target = NULL;
			for (int j = 0; sym_table[j].function_ptr != NULL;
			     j++) {
				if (strcmp(
					(char *)name,
					(char *)sym_table[j].function_name) ==
				    0) {
					target = sym_table[j].function_ptr;
					break;
				}
			}
			#ifdef __linux__
			if (target == NULL) {
				dlopen_ctx = dlopen(NULL, 0);
				target	   = dlsym(dlopen_ctx, (char *)name);
			}
			#endif
			if (target == NULL) {
#ifdef ENABLE_DBG_PRINT
				printf("[!] missing function: %s\n", name);
#endif
				ret = -ENOEXEC;
				break;
			}
#if defined(__x86_64__)
			/* only trampoline functions; objects are another story
			 */
			if (ELF64_ST_TYPE(sym[i].st_info) == STT_FUNC) {
				trampoline = x86_64_make_trampoline(target);
				if (trampoline == NULL) {
					#ifdef ENABLE_DBG_PRINT
					printf(
					    "[!] trampoline alloc failed for %s\n",
					    name);
					#endif
					ret = -ENOMEM;
					break;
				}
				sym[i].st_value = (Elf64_Addr)(uintptr_t)trampoline;
#ifdef ENABLE_DBG_PRINT
				printf("- resolved %-24s target=%p tramp=%p\n",
				       name, target, trampoline);
#endif
			}else{
				sym[i].st_value = (Elf64_Addr)(uintptr_t)target;
#ifdef ENABLE_DBG_PRINT
				printf("- resolved %-24s target=%p\n",
				       name, target);
#endif
			}
#else
			sym[i].st_value = (Elf64_Addr)(uintptr_t)target;
#endif
			break;
		default:
			if (sym[i].st_shndx == info->index.pcpu) {
				secbase = (unsigned long)(mod->percpu);
			} else {
				secbase =
				    info->sechdrs[sym[i].st_shndx].sh_addr;
			}
			sym[i].st_value += secbase;
                        /*
                         * if we have a function which is defined inside this module, add it to the internal symbol table, this will permit
                         * to resolve the relocations when we will have multi-modules dependencies
                         */
                        if ((ELF64_ST_TYPE(sym[i].st_info) == STT_FUNC || ELF64_ST_TYPE(sym[i].st_info) == STT_OBJECT) && strcmp(name, "cleanup_module") != 0 && strcmp(name, "init_module") != 0) {
                                for (int j = 0; sym_table[j].function_ptr != NULL; j++){
                                        if((j + 1 ) >= ARRAY_SIZE(sym_table)){
                                                break;
                                        }
                                        if(sym_table[j + 1].function_ptr == NULL){
                                                /* insert the function and it's name */
						sym_table[j + 1].function_ptr  = sym[i].st_value;
						memset(sym_table[j + 1].function_name, 0, sizeof(sym_table[j].function_name));
						if(strlen(name) < sizeof(sym_table[j].function_name)){
							strncpy(sym_table[j + 1].function_name, name, strlen(name));
						}else{
							strncpy(sym_table[j + 1].function_name, name, sizeof(sym_table[j].function_name));
						}
						sym_table[j + 1].function_size = sym[i].st_size;
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
	int err = 0;

	for (unsigned int i = 1; i < info->hdr->e_shnum; i++) {
		unsigned int infosec = info->sechdrs[i].sh_info;

		if (infosec >= info->hdr->e_shnum) {
			continue;
		}

		if (!(info->sechdrs[infosec].sh_flags & SHF_ALLOC)) {
			continue;
		}

		if (info->sechdrs[i].sh_flags & SHF_RELA_LIVEPATCH) {
#ifdef ENABLE_DBG_PRINT
			printf("LIVEPATCH not supported!\n");
#endif
			return err;
		}

		if (info->sechdrs[i].sh_type == SHT_REL) {
			err = apply_relocate(info->sechdrs, info->strtab,
					     info->index.sym, i, mod);
		} else if (info->sechdrs[i].sh_type == SHT_RELA) {
			err = apply_relocate_add(info->sechdrs, info->strtab,
						 info->index.sym, i, mod);
		}

		if (err < 0) {
			break;
		}
	}
	return err;
}

static char elf_type(const Elf(Sym) * sym, const struct load_info *info) {
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
	if (strstarts(info->secstrings + sechdrs[sym->st_shndx].sh_name,
		      ".debug")) {
		return 'n';
	}
	return '?';
}

void add_kallsyms(struct module *mod, const struct load_info *info) {
	unsigned int i	    = 0;
	unsigned int ndst   = 0;
	ssize_t	     ret    = 0;
	const Elf(Sym) *src = NULL;
	Elf(Sym) *dst	    = NULL;
	char *s		    = NULL;

	Elf(Shdr) *symsec	     = &info->sechdrs[info->index.sym];
	unsigned long strtab_size    = 0;
	void	     *data_base	     = mod->mem[MOD_DATA].base;
	void	     *init_data_base = mod->mem[MOD_INIT_DATA].base;

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

void sort_extable(struct exception_table_entry *start,
		  struct exception_table_entry *finish) {
	/* do nothing */
}

static int post_relocation(struct module *mod, const struct load_info *info) {
	sort_extable(mod->extable, mod->extable + mod->num_exentries);

	add_kallsyms(mod, info);
	return module_finalize(info->hdr, info->sechdrs, mod);
}

static int module_set_memory(const struct module *mod, enum mod_mem_type type) {
	struct module_memory {
		void	    *base;
		unsigned int size;
	};

	unsigned int	      mprotect_perm = 0;
	struct module_memory *mod_mem = (struct module_memory *)&mod->mem[type];

	if (mod_mem->base == NULL) {
		return 0;
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
		mprotect_perm = PROT_READ | PROT_EXEC | PROT_WRITE;
		break;
	default:
#ifdef ENABLE_DBG_PRINT
		printf("[!] unknown memory type: %d\n", type);
#endif
		return 0;
	}

	return mprotect(mod_mem->base, mod_mem->size,
			mprotect_perm);
}

int module_enable_rodata_ro(const struct module *mod, bool after_init) {
	int ret = 0;

	ret = module_set_memory(mod, MOD_RODATA);

	if (ret) {
		return ret;
	}

	ret = module_set_memory(mod, MOD_INIT_RODATA);

	if (ret) {
		return ret;
	}

	if (after_init) {
		return module_set_memory(mod, MOD_RO_AFTER_INIT);
	}
	return 0;
}

int module_enable_data_nx(const struct module *mod) {
	int ret = 0;

	for_class_mod_mem_type(type, data) {
		ret = module_set_memory(mod, type);

		if (ret) {
			return ret;
		}
	}
	return 0;
}

int module_enable_text_rox(const struct module *mod) {
	int ret = 0;

	for_class_mod_mem_type(type, text) {
		ret = module_set_memory(mod, type);

		if (ret) {
			return ret;
		}
	}
	return 0;
}

static int complete_formation(struct module *mod, struct load_info *info) {
	int err = 0;

	err = module_enable_rodata_ro(mod, false);

	if (err) {
		goto out_strict_rwx;
	}

	err = module_enable_data_nx(mod);

	if (err) {
		goto out_strict_rwx;
	}

	err = module_enable_text_rox(mod);

	if (err) {
		goto out_strict_rwx;
	}

	mod->state    = MODULE_STATE_COMING;
	return 0;

out_strict_rwx:
	/** do nothing **/
out:
	return err;
}

static void do_mod_ctors(struct module *mod) {
	unsigned long i = 0;

	//        for (i = 0; i < mod->num_ctors; i++){
	//              mod->ctors[i]();
	//    }
}

static inline unsigned long ex_to_insn(const struct exception_table_entry *x) {
	return (unsigned long)&x->insn + x->insn;
}

static inline bool within_module_init(unsigned long	   addr,
				      const struct module *mod) {
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

	if (ret > 0) {
#ifdef ENABLE_DBG_PRINT
		printf("%s: '%s'->init suspiciously returned %d, it should "
		       "follow 0/-E convention\n"
		       "%s: loading module anyway...\n",
		       __FUNCTION__, mod->name, ret, __FUNCTION__);
#endif
	}

	mod->state = MODULE_STATE_LIVE;
	trim_init_extable(mod);
	return 0;
}

static int call_final_function(struct module *mod, struct load_info *info,
			       unsigned char *function_name) {
	int ret = 0;
	unsigned int n = 0;
	char *strtab = NULL;
	const char *name = NULL;
	unsigned char *ptr = NULL;
	int (*fn)(void) = NULL;
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
		printf("%s returned %d\n", name, ret);
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
			printf("%s returned %d\n", name, ret);

			return ret;
		}
	}

	printf("Function not found in symtab: %s\n", function_name);
	return -ENOENT;
}


/* main entry point of the kernel "module_init" function, permits to load entirely a kernel module, in our case instead, it will load it only in memory and wait for the function to run */
int module_init(unsigned char *file_name, unsigned char *function_name, struct load_info *info) {
	int	       fd		= 0;
	long	       err		= 0;
	unsigned int   file_size	= 0;
	bool	       module_allocated = false;
	struct module *mod		= NULL;
	char	      *after_dashes	= NULL;
	unsigned char *mapped_file	= NULL;
	unsigned char *tmp_mapped_file	= NULL;
	struct stat    file_stats	= {};

	if (file_name == NULL) {
		return -EINVAL;
	}

	fd = open(file_name, O_RDONLY);

	if(fd < 0){
		printf("[!] file %s not exist!\n", file_name);
		return -EINVAL;
	}

	fstat(fd, &file_stats);
	file_size = file_stats.st_size;

	if(file_size == 0){
		printf("[!] file %s is empty!\n", file_name);
		return -EINVAL;
	}

	tmp_mapped_file = (unsigned char *)mmap(
	    NULL, file_size, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0x00);

	mapped_file = (unsigned char *)mmap(
	    NULL, file_size, PROT_READ | PROT_WRITE,
	    MAP_ANON | MAP_PRIVATE, -1, 0x00);

	if(tmp_mapped_file == MAP_FAILED || mapped_file == MAP_FAILED){
		printf("[!] error during mmap! aborting...\n");
		if(tmp_mapped_file != NULL){
			munmap(tmp_mapped_file, file_size);
		}
		if(mapped_file != NULL){
			munmap(mapped_file, file_size);
		}
		close(fd);
		return -EINVAL;
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
		err = dwarf_init(director.fd, DW_DLC_READ, NULL, NULL, &director.dbg, &director.err);
		if(err != DW_DLV_OK){
			printf("[!] failed to initialize libdwarf!\n");
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

	printf("name %s\n", get_modinfo(info, "name"));
	printf("vermagic %s\n", get_modinfo(info, "vermagic"));

	mod = layout_and_allocate(info, 0);

	if (mod == NULL) {
		printf("module struct is null!\n");
		return -1;
	}

	module_allocated = true;
	err		 = percpu_modalloc(mod, info);

	if (err) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = find_module_sections(mod, info);

	if (err) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = simplify_symbols(mod, info);

	if (err < 0 && director.global_conf.strict_missing_syms == true) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = apply_relocations(mod, info);

	if (err < 0) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = post_relocation(mod, info);

	if (err < 0) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = complete_formation(mod, info);

	if (err) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = do_init_module(mod);

	if (err) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	free(director.info);
	director.info = NULL;
	director.loaded_lkm_num++;
	munmap(mapped_file, file_size);
	mapped_file = NULL;
	file_size = 0;
	return err;
}

/* internal function used for initialize a workqueue */
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

int initialize_director(struct module_controller *director, struct load_info **info) {
	int ret = 0;

	director->runtime_generated_structs = &runtime_generated_structs;
        director->info = calloc(1, sizeof(struct load_info));
	if (director->info == NULL){
		ret = -ENOMEM;
		return ret;
	}
	director->rfkill = calloc(1, sizeof(struct rfkill));
	if(director->rfkill == NULL){
		ret = -ENOMEM;
		return ret;
	}

	/* initialize workqueues if necessary */
	if(system_wq == NULL){
		workqueue_init(&director->system_wq, "system_wq");
		system_wq = &director->system_wq;
	}
	if(system_dfl_wq == NULL){
		workqueue_init(&director->system_dfl_wq, "system_dfl_wq");
		system_dfl_wq = &director->system_dfl_wq;
	}

	if(system_freezable_wq == NULL){
		workqueue_init(&director->system_freezable_wq, "system_freezable_wq");
		system_freezable_wq = &director->system_freezable_wq;
	}

	/* initialize usb i/o mutex */
	pthread_mutex_init(&director->usb_io_lock, NULL);
	pthread_cond_init(&director->usb_io_idle, NULL);
	director->usb_io_depth = 0;

	/* initialize skb queue mutex */
	pthread_mutex_init(&director->skb_queue_lock, NULL);

	pthread_mutex_init(&director->timer_lock, NULL);
	pthread_cond_init(&director->timer_cond, NULL);
	director->timer_once  = PTHREAD_ONCE_INIT;

	*info = director->info;
	return ret;
}

/* clean everything allocated in the main controller */
void finalize_director(void){
	if(director.info != NULL){
		free(director.info);
		director.info = NULL;
	}
	if(director.ieee80211_local != NULL){

	}
	/* Stop the libusb event thread BEFORE freeing USB resources it accesses;
	 * otherwise Thread 3 races on the freed usb_device_ptr block. */
	atomic_store_explicit(&director.libusb_thread_stop, true, memory_order_release);
	libusb_interrupt_event_handler(NULL);
	if (director.libusb_event_thread != 0) {
		pthread_join(director.libusb_event_thread, NULL);
		director.libusb_event_thread = 0;
	}

	if(director.usb_device_ptr != NULL){
		free(director.usb_device_ptr);
		director.usb_device_ptr = NULL;
	}
	if(percpu_area != NULL){
		munmap(percpu_area, 1024 * 1024 * 8);
		percpu_area = NULL;
	}
}

int collect_sizes(struct module_controller *director){
        int ret = 0;
        u16 current_struct_size = 0;
        memset(&director->struct_sizes, 0, sizeof(director->struct_sizes));
        current_struct_size = get_struct_size("ieee80211_hw");
        if(current_struct_size != director->struct_sizes.ieee80211_hw && current_struct_size > 0){
                director->struct_sizes.ieee80211_hw = current_struct_size;
        }
        current_struct_size = get_struct_size("ieee80211_vif");
        if(current_struct_size != director->struct_sizes.ieee80211_vif && current_struct_size > 0){
                director->struct_sizes.ieee80211_vif = current_struct_size;
        }
        current_struct_size = get_struct_size("ieee80211_bss_conf");
        if(current_struct_size != director->struct_sizes.ieee80211_bss_conf && current_struct_size > 0){
                director->struct_sizes.ieee80211_bss_conf = current_struct_size;
        }
        current_struct_size = get_struct_size("ieee80211_ops");
        if(current_struct_size != director->struct_sizes.ieee80211_ops && current_struct_size > 0){
                director->struct_sizes.ieee80211_ops = current_struct_size;
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
        current_struct_size = get_struct_size("device");
        if(current_struct_size != director->struct_sizes.device && current_struct_size > 0){
                director->struct_sizes.device = current_struct_size;
        }
        current_struct_size = get_struct_size("hci_dev");
        if(current_struct_size != director->struct_sizes.hci_dev && current_struct_size > 0){
                director->struct_sizes.hci_dev = current_struct_size;
        }

        if(director->struct_sizes.skb_shared_info == 0){
                director->struct_sizes.skb_shared_info = sizeof(struct skb_shared_info);
        }
        return ret;
}

/* fast api: a very minimal and direct way for executing an immediate action (load a kernel driver, executing a function..) */
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

/* get the symbol pointer and it's size */
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

/* do the opposite of the function above: given a function pointer, retrieve it's name */
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

/* similar to 'fast_get_name' but search the symbol inside a range (function_ptr + function_size) */
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

#ifdef __linux__
struct so_context {
	void *sym_addr;
	unsigned char **sym_name;
};

/* callback used for walking through the loaded libraries */
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

/* search the name of a symbol by walking in the shared objects, used mainly for the backtrace subsystem */
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

int fast_execute_function(unsigned char *function_name){
	int ret = 0;
	u8 found = false;
	int (*fn)(void) = NULL;
	unsigned char *ptr = NULL;
	unsigned char *current_function_name = NULL;

	/* call the function */
	for (unsigned int i = 0; ; i++) {
		current_function_name = sym_table[i].function_name;

		if (current_function_name == NULL || current_function_name[0] == '\0'){
			continue;
		}
		if (strcmp(current_function_name, function_name) == 0) {
			found = true;
			fn = (int (*)(void))(uintptr_t)sym_table[i].function_ptr;
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
		if(sym_table[i + 1].function_name[0] == '\0'){
			break;
		}
	}
	if(found == false){
		printf("[!] failed to execute function '%s'\n", function_name);
	}
	return ret;
}

/*
 *      Utilities functions used for the cmd shell.
 */
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

/* given a .ko name, load it into memory, this permits to load muultiple kernel modules into the process memory without executing them */
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

/* the opposite of 'load', given a kernel module name, unload it from the system */
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

/* 'start' is used as an alias for 'run' */
int drv_cmd_start(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;

	return ret;
}

/* stop the driver execution, different from the unload function */
int drv_cmd_stop(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;

	return ret;
}

/* given a kernel module name registered in the system and the init function, run it, this permits to loads different symbols from every loaded driver into the memory */
int drv_cmd_run(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;
	u8 found = false;
	int (*fn)(void) = NULL;
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
	for (unsigned int i = 0; ; i++) {
		current_function_name = sym_table[i].function_name;

		if (current_function_name == NULL || current_function_name[0] == '\0'){
			continue;
		}
		if (strcmp((char *)module_function, current_function_name) == 0) {
			found = true;
			fn = (int (*)(void))(uintptr_t)sym_table[i].function_ptr;
			ptr = (unsigned char *)fn;
			if (ptr[0] == 0xE8) {
				int32_t disp = *(int32_t *)(ptr + 1);
				void   *tgt  = ptr + 5 + disp;
			}

			/* mark the module execution */
			printf("--------------------------\n");
			ret = fn();
			printf("%s returned %d\n", module_function, ret);
		}
		if(sym_table[i + 1].function_name[0] == '\0'){
			break;
		}
	}
	if(found == false){
		printf("[!] failed to execute function '%s'\n", module_function);
	}
	return ret;
}

/* set a context variable to a specific value, used for changing the runtime of the driver executor */
int drv_cmd_set(struct module_controller *director, char *argv[], int num_args) {
	int ret = 0;

	return ret;
}

/* permits to get data from the program */
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
		elf_generator_handler();
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

/* permits to invoke various 'ops' struct registered by running the loaded kernel module, like 'ieee80211_ops' */
int drv_cmd_ops(struct module_controller *__director, char *argv[], int num_args) {
        int ret = 0;
        unsigned char *ops_req = NULL;
	struct ieee80211_ops *ops = NULL;
	struct ieee80211_local *local = NULL;
	struct ieee80211_hw *ieee80211_hw = NULL;
	struct ieee80211_bss_conf *bss_conf = NULL;
	struct ieee80211_vif *ieee80211_vif = NULL;
	struct ieee80211_vif *ieee80211_ap_vif = NULL;
	struct ieee80211_scan_request *scan_req = NULL;
	int (*start)(struct ieee80211_hw *hw) = NULL;
	void (*stop)(struct ieee80211_hw *hw, bool suspend) = NULL;
	int (*add_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	void (*cancel_hw_scan)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	void (*remove_interface)(struct ieee80211_hw *hw, struct ieee80211_vif *vif) = NULL;
	int (*hw_scan)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_scan_request *req) = NULL;
	int (*start_ap)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_bss_conf *link_conf) = NULL;
	void (*stop_ap)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_bss_conf *link_conf) = NULL;
	int (*ampdu_action)(struct ieee80211_hw *hw, struct ieee80211_vif *vif, struct ieee80211_ampdu_params *params) = NULL;

	if(argv[1] == NULL){
		printf("[!] ops <callback name>\n");
		ret = -EINVAL;
		return ret;
	}

	ops_req = argv[1];

	/* for now use only 'ieee80211_ops' as reference */
	ops = __director->ieee80211_ops;
	if(ops == NULL){
		printf("[!] ieee80211_ops is NULL!\n");
		ret = -EINVAL;
		return ret;
	}

	local = __director->ieee80211_local;
	if(local->ieee80211_hw == NULL){
		printf("[!] local->ieee80211_hw is NULL!\n");
		ret = -EINVAL;
		return ret;
	}

	if(strcmp(ops_req, "start") == 0){
		start = ops->start;
		if(start != NULL){
			ret = start(local->ieee80211_hw);
		}
	}
	if(strcmp(ops_req, "stop") == 0){
		stop = ops->stop;
		if(stop != NULL){
			stop(local->ieee80211_hw, false);
		}
	}
	if(strcmp(ops_req, "add_interface") == 0){
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
				rcu_assign_pointer(ieee80211_vif->link_conf[0], bss_conf);
				ieee80211_vif->valid_links  |= BIT(0);
				ieee80211_vif->active_links |= BIT(0);
			}
			ret = add_interface(local->ieee80211_hw, local->ieee80211_vif);
		}
	}
	if(strcmp(ops_req, "remove_interface") == 0){
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

	if(strcmp(ops_req, "scan") == 0){
		hw_scan = ops->hw_scan;
		if(hw_scan != NULL && local->ieee80211_vif != NULL){
			if(scan_req != NULL){
				free(scan_req);
				scan_req = NULL;
			}
			scan_req = calloc(1, sizeof(struct ieee80211_scan_request) + 14 * sizeof(void *));
			/* generate the callback parameters */
			ret = fill_hw_scan_req(scan_req);
			ret = hw_scan(local->ieee80211_hw, local->ieee80211_vif, scan_req);
		}else{
			goto ieee80211_vif_not_initialized;
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
	if(strcmp(ops_req, "stop_ap") == 0){
		stop_ap = ops->stop_ap;
		if(stop_ap != NULL && local->ieee80211_vif != NULL){
			if(bss_conf != NULL){
				free(bss_conf);
				bss_conf = NULL;
			}
			bss_conf = calloc(1, director.struct_sizes.ieee80211_bss_conf);
			fill_start_ap_req(bss_conf);
			stop_ap(local->ieee80211_hw, local->ieee80211_vif, bss_conf);
		}else{
			goto ieee80211_vif_not_initialized;
		}
	}

	return ret;
	ieee80211_vif_not_initialized:
	printf("[!] ieee80211_vif is not initialized!\n");
	ret = -EINVAL;
	return ret;
}

#ifdef _WIN64
int pe_generator_handler(void){
	int ret = 0;
	BYTE *base_addr = NULL;
	IMAGE_DOS_HEADER *dos_header = NULL;
	IMAGE_NT_HEADERS *nt_header = NULL;

	base_addr = GetModuleHandleW(NULL);
	if(base_addr == NULL){
		ret = -EINVAL;
		return ret;
	}
	dos_header = (IMAGE_DOS_HEADER *)base_addr;
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE){
		printf("[!] not a DOS header!\n");
		ret = -EINVAL;
		return ret;
	}
	nt_header = (IMAGE_NT_HEADERS *)(base_addr + dos_header->e_lfanew);
	if (nt_header->Signature != IMAGE_NT_SIGNATURE){
		printf("[!] not an NT header!\n");
		ret = -EINVAL;
		return ret;
	}

	printf("Image base      : %p\n", base_addr);
	printf("SizeOfImage     : 0x%lx\n", (unsigned long)nt_header->OptionalHeader.SizeOfImage);
	printf("SizeOfHeaders   : 0x%lx\n", (unsigned long)nt_header->OptionalHeader.SizeOfHeaders);
	printf("EntryPoint RVA  : 0x%lx\n", (unsigned long)nt_header->OptionalHeader.AddressOfEntryPoint);

	return ret;
}
#endif

void elf_crash_handler(int sig){
	if(sig != SIGABRT){
		if(director.rfkill != NULL){
			free(director.rfkill);
			director.rfkill = NULL;
		}
		if(system_wq != NULL){
			destroy_workqueue(system_wq);
			system_wq = NULL;
		}
		if(system_dfl_wq != NULL){
			destroy_workqueue(system_dfl_wq);
			system_dfl_wq = NULL;
		}
		if(system_freezable_wq != NULL){
			destroy_workqueue(system_freezable_wq);
			system_freezable_wq = NULL;
		}
		atomic_store_explicit(&director.libusb_thread_stop, true, memory_order_release);
		libusb_interrupt_event_handler(NULL);
		if (director.libusb_event_thread != 0) {
			pthread_join(director.libusb_event_thread, NULL);
			director.libusb_event_thread = 0;
		}
	}
	/* quit from libusb */
	libusb_exit(NULL);
}

int elf_entry_stub(int argc, char *argv[]){
	int ret = 0;
	/* insert here any additional code */

	/* this part of code is similar to 'main' and 'initialize_director', we must re-allocate/re-initialize every object */
	signal(SIGSEGV, elf_crash_handler);
	signal(SIGABRT, elf_crash_handler);
	signal(SIGILL,  elf_crash_handler);

	libusb_init(NULL);
	atomic_store(&director.libusb_thread_stop, false);
	pthread_create(&director.libusb_event_thread, NULL, libusb_thread, NULL);
	if(director.rfkill == NULL){
		director.rfkill = calloc(1, sizeof(struct rfkill));
        }

	/* initialize workqueues if necessary */
	if(system_wq == NULL){
		workqueue_init(&director.system_wq, "system_wq");
		system_wq = &director.system_wq;
	}
	if(system_dfl_wq == NULL){
		workqueue_init(&director.system_dfl_wq, "system_dfl_wq");
		system_dfl_wq = &director.system_dfl_wq;
	}

	if(system_freezable_wq == NULL){
		workqueue_init(&director.system_freezable_wq, "system_freezable_wq");
		system_freezable_wq = &director.system_freezable_wq;
	}

        /* initialize usb i/o mutex */
	pthread_mutex_init(&director.usb_io_lock, NULL);
	pthread_cond_init(&director.usb_io_idle, NULL);
	director.usb_io_depth = 0;

	/* initialize skb queue mutex */
	pthread_mutex_init(&director.skb_queue_lock, NULL);

	pthread_mutex_init(&director.timer_lock, NULL);
	pthread_cond_init(&director.timer_cond, NULL);
	director.timer_once  = PTHREAD_ONCE_INIT;
	/* call the driver init */
	if(director.driver_init_function != NULL){
		ret = director.driver_init_function();
		usleep(5000000);
		ret = director.driver_init_function();
	}

	quit:
	exit(ret);
}

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
		printf("[!] no modules loaded!\n");
		ret = -EINVAL;
		return ret;
	}

	if(director.envp == NULL){
		printf("[!] envp not saved!\n");
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

	/* save the symbol for the generated ELF */
	fast_get_symbol("_driver_init", &director.driver_init_function);

	elf_dump_fd = open("dumped_driver.elf", O_RDWR | O_CREAT | O_TRUNC, 0777);
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

/*
 * add also a crash handler which returns a custom file format when it crash,
 * output file format is still wip, it should contains both the callback table and the offset table.
*/
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

        #ifdef ENABLE_UNWIND_DEBUGGING && defined(__linux__)
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

static void trim_inplace(unsigned char *line){
	unsigned char *start = NULL;
	unsigned char *end = NULL;

	if (line == NULL){
		goto quit;
	}

	start = line;

	while (*start && isspace((unsigned char)*start)){
		start++;
	}

	if (start != line){
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

/* helper function used for searching a strut given it's name */
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

int handle_offset_config(unsigned char *offset_config_name){
	int ret = 0;
	int fd = 0;
	int nread = 0;
	int total = 0;
	int file_size = 0;
	char line[48] = {};
	struct stat config_stats = {};
	unsigned char *cursor = NULL;
	unsigned char *config_buffer = NULL;
	struct runtime_struct_layout *current_layout = NULL;

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

int main(int argc, char *argv[], char *envp[]) {
	int	       ret	     = 0;
	int            fd            = 0;
	char           line[64]      = {};
	unsigned char *drv_arg[5]    = {};
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
			.cmd_args = 1,
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
			.cmd_args = 1,
			.cmd_ptr  = drv_cmd_ops,
		},
	};
	/* register now the signal handler */
	signal(SIGSEGV, crash_handler);
	signal(SIGABRT, crash_handler);
	signal(SIGILL,  crash_handler);

	director.internal_cb_table = &sym_table;
	director.envp = envp;
	libusb_init(NULL);

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
			struct runtime_struct_layout *runtime_struct_layout = NULL;
			struct struct_field *current_struct_fields = NULL;

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

	/* add a fast loading option: if the user insert as argv 1 or more '.ko', load them in the order given */
	if(strstr(argv[1], ".ko") != NULL){
		for(u8 argv_idx = 1; argv[argv_idx] != NULL; argv_idx++){
			/* add a new fast option: if there is only 1 name without .ko, assume it is the function that needs to be executed */
			if(argv_idx != 1 && strstr(argv[argv_idx], ".ko") == NULL && strstr(argv[argv_idx], ".conf") == NULL){
				function_name = argv[argv_idx];
				continue;
			}
			/* skip the .conf file */
			if(argv_idx != 1 && strstr(argv[argv_idx], ".ko") == NULL && strstr(argv[argv_idx], ".conf") != NULL){
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

	return ret;
}

#include <cb_table.h>
