struct urb *usb_alloc_urb(int iso_packets, int mem_flags){
	int urb_size = 0;
	struct urb *urb = NULL;

	urb_size = director.struct_sizes.urb;
	urb = calloc(1, urb_size + (sizeof(struct usb_iso_packet_descriptor) * iso_packets));
	if (urb == NULL){
		return NULL;
	}
	usb_init_urb(urb);
	return urb;
}
