struct usb_interface *usb_ifnum_to_if(const struct usb_device *udev, unsigned ifnum){
	struct usb_host_config *config = NULL; //((char *)udev + director.runtime_generated_structs->structs[1].fields[0].field_offset);
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
