int usb_urb_ep_type_check(const struct urb *urb){
	int ret = 0;

	ret = usb_pipe_type_check(urb->dev, urb->pipe);
	return ret;
}
