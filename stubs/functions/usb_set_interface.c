int usb_set_interface(struct usb_device *udev, int interface, int alternate){
	int ret = 0;
	struct usb_interface *intf = NULL;
	struct usb_host_interface *alt = NULL;

	if(udev == NULL || director.usb_dev == NULL){
		ret = -ENODEV;
		return ret;
	}
	intf = usb_ifnum_to_if(udev, interface);
	if(intf == NULL){
		ret = -EINVAL;
		return ret;
	}
	for(int i = 0; i < intf->num_altsetting; i++){
		if(intf->altsetting[i].desc.bAlternateSetting == alternate){
			alt = &intf->altsetting[i];
			break;
		}
	}
	if(alt == NULL){
		ret = -EINVAL;
		return ret;
	}
	ret = libusb_set_interface_alt_setting(director.usb_dev, interface, alternate);
	if(ret < 0){
		ret = -EIO;
		return ret;
	}
	usb_uncache_endpoints(udev, intf->cur_altsetting);
	intf->cur_altsetting = alt;
	usb_cache_endpoints(udev, alt);
	return ret;
}
