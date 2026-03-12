struct urb *usb_alloc_urb(int iso_packets, int mem_flags){
	struct urb *urb = NULL;

	urb = malloc(sizeof(struct urb) + (sizeof(struct usb_iso_packet_descriptor) * iso_packets));
	if (urb == NULL){
		return NULL;
	}
	usb_init_urb(urb);
	return urb;
}
