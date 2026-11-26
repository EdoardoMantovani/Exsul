const struct usb_device_id *usb_match_id(struct usb_interface *interface, const struct usb_device_id *id){
	int ret = 0;
	struct usb_device *udev = NULL;
	struct usb_interface_descriptor *desc = NULL;

	if(interface == NULL || id == NULL || interface->cur_altsetting == NULL){
		return NULL;
	}
	udev = interface_to_usbdev(interface);
	if(udev == NULL){
		return NULL;
	}
	desc = &interface->cur_altsetting->desc;
	while(!usb_id_terminator(id)){
		ret = __usb_match_id(&udev->descriptor, desc, id);
		if(ret){
			return id;
		}
		id++;
	}
	return NULL;
}
