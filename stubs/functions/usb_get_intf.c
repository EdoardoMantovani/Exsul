struct usb_interface *usb_get_intf(struct usb_interface *intf){
	if (intf != NULL){
		get_device(&intf->dev);
	}
	return intf;
}
