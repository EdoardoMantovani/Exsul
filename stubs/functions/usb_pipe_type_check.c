int usb_pipe_type_check(struct usb_device *dev, unsigned int pipe){
	int ret = 0;
	const struct usb_host_endpoint *ep = NULL;
	const int pipetypes[4] = {
		PIPE_CONTROL, PIPE_ISOCHRONOUS, PIPE_BULK, PIPE_INTERRUPT
	};

	ep = usb_pipe_endpoint(dev, pipe);
	if (ep == NULL){
		ret = -EINVAL;
		return ret;
	}
	if (usb_pipetype(pipe) != pipetypes[usb_endpoint_type(&ep->desc)]){
		ret = -EINVAL;
		return ret;
	}
	return ret;
}
