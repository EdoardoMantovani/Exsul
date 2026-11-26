const struct usb_device_id *usb_device_match_id(struct usb_device *udev, const struct usb_device_id *id){
	if (id == NULL){
		return NULL;
	}
	for(; !usb_id_terminator(id); id++){
		if (usb_match_device(udev, id)){
			return id;
		}
	}
	return NULL;
}
