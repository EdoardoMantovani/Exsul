int usb_control_msg(struct usb_device *dev, unsigned int pipe, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *data, __u16 size, int timeout){
	int ret = 0;

	usb_lock();
	printf("usb_control_msg1\n");
	ret = libusb_control_transfer(
		director.usb_dev,
		requesttype,
		request,
		value,
		index,
		data,
		size,
		timeout
	);
	printf("usb_control_msg2\n");
	usb_unlock();
	printf("usb_control_msg3\n");
	return ret;
}
