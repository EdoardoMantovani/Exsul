int usb_unlink_urb(struct urb *urb){
	int ret = 0;
	struct usb_device *udev = NULL;

	if (urb == NULL){
		return -EINVAL;
	}
	if (urb->dev == NULL){
		return -ENODEV;
	}
	if (urb->ep == NULL){
		return -EIDRM;
	}

	udev = urb->dev;
	/* inline here 'usb_hcd_unlink_urb' */
	if (atomic_load_explicit(&urb->use_count, memory_order_acquire) > 0) {
		ret = 0;
		usb_get_dev(udev);
	}
	if (ret == 0) {
		/* do not use 'unlink1', skip it
		 * retval = unlink1(hcd, urb, status);
		 */
		if (ret == 0){
			ret = -EINPROGRESS;
		}else if (ret != -EIDRM && ret != -EBUSY){
			printf("[!] hcd_unlink_urb %p fail %d\n", urb, ret);
		}
		usb_put_dev(udev);
	}
	return ret;
}
