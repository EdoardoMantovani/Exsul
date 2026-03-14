void usb_deregister(struct usb_driver *driver){
	if(driver == NULL){
		goto end;
	}

	pr_info("[usbcore] deregistering interface driver %s\n", driver->name);

	end:
}
