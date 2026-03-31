struct urb *usb_alloc_urb(int iso_packets, int mem_flags){
	int urb_size = 0;
	struct urb *urb = NULL;
	printf("usb_alloc_urb1\n");
	urb_size = director.struct_sizes.urb;
	printf("usb_alloc_urb2 size: %d\n", urb_size + (sizeof(struct usb_iso_packet_descriptor) * iso_packets));
	urb = __calloc(1, urb_size + (sizeof(struct usb_iso_packet_descriptor) * iso_packets));
	printf("usb_alloc_urb3\n");
	if (urb == NULL){
		return NULL;
	}
	usb_init_urb(urb);
	printf("usb_alloc_urb4\n");
	return urb;
}
