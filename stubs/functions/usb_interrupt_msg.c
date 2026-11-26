int usb_interrupt_msg(struct usb_device *usb_dev, unsigned int pipe, void *data, int len, int *actual_length, int timeout){
	int ret = 0;
	u8 ep = 0;

	ep = convert_usb_pipe_to_ep(pipe);
	usb_lock();
        ret = libusb_interrupt_transfer(
                director.usb_dev,
		ep,
                data,
                len,
                actual_length,
                timeout
        );
	usb_unlock();

	return ret;
}
