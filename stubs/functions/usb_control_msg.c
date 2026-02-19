int usb_control_msg(struct usb_device *dev, unsigned int pipe, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *data, __u16 size, int timeout){
	int ret = 0;
        ret = libusb_control_transfer(
                &director.usb_dev,
                requesttype,
                request,
                value,
                index,
                data,
                size,
                timeout
        );
	return ret;
}
