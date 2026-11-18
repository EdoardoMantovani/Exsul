int usb_anchor_empty(struct usb_anchor *anchor){
	int ret = 0;

	ret = list_empty(&anchor->urb_list);
	return ret;
}
