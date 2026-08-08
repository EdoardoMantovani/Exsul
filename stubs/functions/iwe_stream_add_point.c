char *iwe_stream_add_point(struct iw_request_info *info, char *stream, char *ends, void *iwe, char *extra){
	int lcp_len = 0;
	int point_len = 0;
	int event_len = 0;
	struct iw_event *iw_event = NULL;

	/* use this trick for 'struct iw_event' */
	struct iw_event {
		__u16            len;            /* Real length of this stuff */
		__u16            cmd;            /* Wireless IOCTL */
		union iwreq_data u;              /* IOCTL fixed payload */
	};

	iw_event = (struct iw_event *)iwe;
	lcp_len = IW_EV_LCP_LEN;
	point_len = IW_EV_POINT_LEN;
	event_len = IW_EV_POINT_LEN + iw_event->u.data.length;

	/* Check if it's possible */
	if (likely((stream + event_len) < ends)) {
		iw_event->len = event_len;
		memcpy(stream, (char *) iw_event, IW_EV_LCP_PK_LEN);
		memcpy(stream + lcp_len, ((char *) &iw_event->u) + IW_EV_POINT_OFF, IW_EV_POINT_PK_LEN - IW_EV_LCP_PK_LEN);
		if (iw_event->u.data.length && extra){
			memcpy(stream + point_len, extra, iw_event->u.data.length);
		}
		stream += event_len;
	}
	return stream;
}

