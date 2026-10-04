int usb_reset_device(struct usb_device *udev){
	int ret = 0;

	/* instead of using the normal kernel function, use libusb */
	if(director.usb_dev != NULL){
		ret = libusb_reset_device(director.usb_dev);
	}
	return ret;
}
