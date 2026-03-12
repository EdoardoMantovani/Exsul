void usb_put_intf(struct usb_interface *intf){
	if (intf != NULL){
		put_device(&intf->dev);
	}
}
