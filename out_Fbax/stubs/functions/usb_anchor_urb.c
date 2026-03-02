void usb_anchor_urb(struct urb *urb, struct usb_anchor *anchor){
	usb_get_urb(urb);
	list_add_tail(((char *)urb + director.runtime_generated_structs->structs[5].fields[1].field_offset), &anchor->urb_list);
	*(struct usb_anchor **)((char *)urb + director.runtime_generated_structs->structs[5].fields[2].field_offset) = anchor;
}
