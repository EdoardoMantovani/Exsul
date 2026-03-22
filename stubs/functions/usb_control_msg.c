int usb_control_msg(struct usb_device *dev, unsigned int pipe, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *data, __u16 size, int timeout){
	int ret = 0;

	printf("usb_control_msg handle=%p data=%p size=%u timeout=%d req=%02x type=%02x value=%04x index=%04x\n", director.usb_dev, data, size, timeout, request, requesttype, value, index);
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
	printf("rret : %d\n", ret);
	return ret;
}
