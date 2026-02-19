struct usb_interface *usb_ifnum_to_if(const struct usb_device *dev, unsigned ifnum){
	struct usb_host_config *config = NULL; //dev->actconfig;
	int i = 0;

	if (!config){
		return NULL;
	}

	for (i = 0; i < config->desc.bNumInterfaces; i++){
		if (config->interface[i]->altsetting[0].desc.bInterfaceNumber == ifnum){
			return config->interface[i];
		}
	}
	return NULL;
}
