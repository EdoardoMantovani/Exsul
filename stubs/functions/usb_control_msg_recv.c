int usb_control_msg_recv(struct usb_device *dev, __u8 endpoint, __u8 request, __u8 requesttype, __u16 value, __u16 index, void *driver_data, __u16 size, int timeout, int memflags){
	int ret = 0;
	unsigned int pipe = 0;
	u8 *data = NULL;

	pipe = usb_rcvctrlpipe(dev, endpoint);
	if (size == 0|| driver_data == NULL){
		ret = -EINVAL;
		return ret;
	}
	data = calloc(1, size);
	if (data == NULL){
		ret = -ENOMEM;
		return ret;
	}
	ret = usb_control_msg(dev, pipe, request, requesttype, value, index, data, size, timeout);
	if (ret < 0){
		goto exit;
	}
	if (ret == size) {
		memcpy(driver_data, data, size);
		ret = 0;
	} else {
		ret = -EREMOTEIO;
	}
	exit:
	kfree(data);
	return ret;
}
