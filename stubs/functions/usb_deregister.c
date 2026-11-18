void usb_deregister(struct usb_driver *driver){
	void (LINUX_ABI *disconnect)(struct usb_interface *intf) = NULL;

	if(driver == NULL || director.bound_usb_driver != driver){
		return;
	}
	/* The driver owns its probe allocations; disconnect before USB shutdown. */
	director.bound_usb_driver = NULL;
	disconnect = driver->disconnect;
	if(disconnect != NULL && director.usb_interface != NULL){
		disconnect(director.usb_interface);
	}
}
