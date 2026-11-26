int usb_set_wireless_status(struct usb_interface *iface, int status){
	int ret = 0;

	if (iface->wireless_status == status){
		ret = -EALREADY;
		return ret;
	}
	usb_get_intf(iface);
	iface->wireless_status = status;
	schedule_work(&iface->wireless_status_work);
	return ret;
}
