char *iwe_stream_add_event(struct iw_request_info *info, char *stream, char *ends, void *iwe, int event_len){
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
	/* Check if it's possible */
	if (likely((stream + event_len) < ends)) {
		iw_event->len = event_len;
		/* Beware of alignment issues on 64 bits */
		memcpy(stream, (char *) iw_event, IW_EV_LCP_PK_LEN);
		memcpy(stream + lcp_len, &iw_event->u, event_len - lcp_len);
		stream += event_len;
	}

	return stream;
}

