stub bool usb_check_int_endpoints(struct usb_interface *intf, const u8 *ep_addrs){
	const struct usb_host_endpoint *ep = NULL;

	for (; *ep_addrs; ++ep_addrs) {
		ep = usb_find_endpoint(intf, *ep_addrs);
		if (!ep || !usb_endpoint_xfer_int(&ep->desc)){
			return false;
		}
	}
	return true;
}
