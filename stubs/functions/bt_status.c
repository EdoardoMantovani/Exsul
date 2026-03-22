__u8 bt_status(int err){
	if (err >= 0)
		return err;

	switch (err) {
	case -EBADRQC:
		return 0x01;

	case -ENOTCONN:
		return 0x02;

	case -EIO:
		return 0x03;

	case -EHOSTDOWN:
		return 0x04;

	case -EACCES:
		return 0x05;

	case -EBADE:
		return 0x06;

	case -ENOMEM:
		return 0x07;

	case -ETIMEDOUT:
		return 0x08;

	case -EMLINK:
		return 0x09;

	case -EALREADY:
		return 0x0b;

	case -EBUSY:
		return 0x0c;

	case -ECONNREFUSED:
		return 0x0d;

	case -EOPNOTSUPP:
		return 0x11;

	case -EINVAL:
		return 0x12;

	case -ECONNRESET:
		return 0x13;

	case -ECONNABORTED:
		return 0x16;

	case -ELOOP:
		return 0x17;

	case -EPROTONOSUPPORT:
		return 0x1a;

	case -EPROTO:
		return 0x19;

	default:
		return 0x1f;
	}
}
