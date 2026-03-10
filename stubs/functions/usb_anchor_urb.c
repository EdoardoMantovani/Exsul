void usb_anchor_urb(struct urb *urb, struct usb_anchor *anchor){
	usb_get_urb(urb);
	list_add_tail(&urb->anchor_list, &anchor->urb_list);
	*(struct usb_anchor **)urb->anchor = anchor;
}
