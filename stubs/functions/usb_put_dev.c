void usb_put_dev(struct usb_device *udev){
	if (udev != NULL){
		put_device(&udev->dev);
	}
}
