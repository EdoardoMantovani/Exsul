int usb_control_msg(struct usb_device *dev, unsigned int pipe, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *data, __u16 size, int timeout){
	int ret = 0;

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
	return ret;
}
