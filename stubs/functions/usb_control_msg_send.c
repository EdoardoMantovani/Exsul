int usb_control_msg_send(struct usb_device *dev, __u8 endpoint, __u8 request, __u8 requesttype, __u16 value, __u16 index, const void *driver_data, __u16 size, int timeout, int memflags){
	int ret = 0;
	unsigned int pipe = 0;
	u8 *data = NULL;

	data = kmemdup_noprof(driver_data, size, memflags);

	usb_lock();
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
	#ifdef ENABLE_FUZZING
	if(ret > 0 && usb_pipein(pipe)){
		(void)fuzz_report_usb_rx(data, ret);
	}
	#endif
	usb_unlock();

	kfree(data);
	return ret;
}
