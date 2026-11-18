int usb_bulk_msg(struct usb_device *usb_dev, unsigned int pipe, void *data, int len, int *actual_length, int timeout){
	int ret = 0;
	u8 ep = 0;

	ep = convert_usb_pipe_to_ep(pipe);

	usb_lock();
	ret = libusb_bulk_transfer(
		director.usb_dev,
		ep,
		data,
		len,
		actual_length,
		timeout
	);
	#ifdef ENABLE_FUZZING
	if(ret == LIBUSB_SUCCESS && actual_length != NULL && *actual_length > 0 && usb_pipein(pipe)){
		fuzz_report_usb_rx(data, *actual_length);
	}
	#endif
	usb_unlock();

	return ret;
}
