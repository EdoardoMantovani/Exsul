void usb_driver_release_interface(struct usb_driver *driver, struct usb_interface *iface){
	int ret = 0;
	u8 number = 0;
	struct device *device = NULL;

	if(driver == NULL || iface == NULL || iface->cur_altsetting == NULL){
		return;
	}
	number = iface->cur_altsetting->desc.bInterfaceNumber;
	if(director.usb_dev != NULL && number < USB_MAXINTERFACES && (director.usb_claimed_interfaces & BIT(number))){
		ret = libusb_release_interface(director.usb_dev, number);
		director.usb_claimed_interfaces &= ~BIT(number);
	}
	usb_set_intfdata(iface, NULL);
	device = &iface->dev;
	device->driver = NULL;
	iface->condition = USB_INTERFACE_UNBOUND;
}
