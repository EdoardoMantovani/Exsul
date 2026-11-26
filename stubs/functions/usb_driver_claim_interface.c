int usb_driver_claim_interface(struct usb_driver *driver, struct usb_interface *iface, void *data){
	int ret = 0;
	u8 number = 0;
	struct device *device = NULL;

	if(driver == NULL || iface == NULL || iface->cur_altsetting == NULL || director.usb_dev == NULL){
		ret = -ENODEV;
		return ret;
	}
	number = iface->cur_altsetting->desc.bInterfaceNumber;
	/* add this unnecessary interface number check */
	if(number >= USB_MAXINTERFACES){
		ret = -EINVAL;
		return ret;
	}
	if(!(director.usb_claimed_interfaces & BIT(number))){
		ret = libusb_claim_interface(director.usb_dev, number);
		if(ret < 0){
			return ret == LIBUSB_ERROR_BUSY ? -EBUSY : -ENODEV;
		}
		director.usb_claimed_interfaces |= BIT(number);
	}
	device = &iface->dev;
	device->driver = &driver->driver;
	usb_set_intfdata(iface, data);
	iface->condition = USB_INTERFACE_BOUND;
	return ret;
}
