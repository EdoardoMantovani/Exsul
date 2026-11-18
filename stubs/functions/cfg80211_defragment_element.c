ssize_t cfg80211_defragment_element(const struct element *elem, const u8 *ies, size_t ieslen, u8 *data, size_t data_len, u8 frag_id) {
	int ret = 0;
	int copied = 0;
	u8 elem_datalen = 0;
	const struct element *next = NULL;

	if (!elem){
		ret = -EINVAL;
		return ret;
	}
	/* elem might be invalid after the memmove */
	next = (void *)(elem->data + elem->datalen);
	elem_datalen = elem->datalen;
	if (elem->id == WLAN_EID_EXTENSION) {
		copied = elem->datalen - 1;

		if (data) {
			if (copied > data_len){
				ret = -ENOSPC;
				return ret;
			}
			memmove(data, elem->data + 1, copied);
		}
	}else{
		copied = elem->datalen;

		if (data) {
			if (copied > data_len){
				ret = -ENOSPC;
				return ret;
			}
			memmove(data, elem->data, copied);
		}
	}

	/* Fragmented elements must have 255 bytes */
	if (elem_datalen < 255){
		return copied;
	}
	for (elem = next;
	     elem->data < ies + ieslen &&
		elem->data + elem->datalen <= ies + ieslen;
	     elem = next) {
		/* elem might be invalid after the memmove */
		next = (void *)(elem->data + elem->datalen);

		if (elem->id != frag_id){
			break;
		}
		elem_datalen = elem->datalen;

		if (data) {
			if (copied + elem_datalen > data_len){
				ret = -ENOSPC;
				return ret;
			}
			memmove(data + copied, elem->data, elem_datalen);
		}
		copied += elem_datalen;
		/* Only the last fragment may be short */
		if (elem_datalen != 255){
			break;
		}
	}
	return copied;
}
