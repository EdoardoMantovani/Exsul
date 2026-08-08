char *iwe_stream_add_value(struct iw_request_info *info, char *event, char *value, char *ends, void *iwe, int event_len){
	int lcp_len = 0;
	struct iw_event *iw_event = NULL;

	/* use this trick for 'struct iw_event' */
	struct iw_event {
		__u16            len;            /* Real length of this stuff */
		__u16            cmd;            /* Wireless IOCTL */
		union iwreq_data u;              /* IOCTL fixed payload */
	};

	iw_event = (struct iw_event *)iwe;
	lcp_len = IW_EV_LCP_LEN;
	/* Don't duplicate LCP */
	event_len -= IW_EV_LCP_LEN;

	/* Check if it's possible */
	if (likely((value + event_len) < ends)) {
		/* Add new value */
		memcpy(value, &iw_event->u, event_len);
		value += event_len;
		/* Patch LCP */
		iw_event->len = value - event;
		memcpy(event, (char *) iw_event, lcp_len);
	}
	return value;
}
