struct usb_interface *usb_ifnum_to_if(const struct usb_device *udev, unsigned ifnum){
	struct usb_interface *intf = NULL;
	struct usb_host_config *config = NULL;

	if(udev == NULL){
		return NULL;
	}
	config = udev->actconfig;
	if (config == NULL){
		return NULL;
	}

	for (int i = 0; i < config->desc.bNumInterfaces; i++){
		intf = config->interface[i];
		if(intf != NULL && intf->altsetting != NULL && intf->altsetting[0].desc.bInterfaceNumber == ifnum){
			return intf;
		}
	}
	return NULL;
}
