int bt_to_errno(__u16 code){
	int ret = 0;

	switch (code) {
	case 0:
		return ret;
	case 0x01:
		ret = EBADRQC;
		return ret;
	case 0x02:
		ret = ENOTCONN;
		return ret;
	case 0x03:
		ret = EIO;
		return ret;
	case 0x04:
	case 0x3c:
		ret = EHOSTDOWN;
		return ret;
	case 0x05:
		ret = EACCES;
		return ret;
	case 0x06:
		ret = EBADE;
		return ret;
	case 0x07:
		ret = ENOMEM;
		return ret;
	case 0x08:
		ret = ETIMEDOUT;
		return ret;
	case 0x09:
		ret = EMLINK;
		return ret;
	case 0x0a:
		ret = EMLINK;
		return ret;
	case 0x0b:
		ret = EALREADY;
		return ret;
	case 0x0c:
		ret = EBUSY;
		return ret;
	case 0x0d:
	case 0x0e:
	case 0x0f:
		ret = ECONNREFUSED;
		return ret;
	case 0x10:
		ret = ETIMEDOUT;
		return ret;
	case 0x11:
	case 0x27:
	case 0x29:
	case 0x20:
		ret = EOPNOTSUPP;
		return ret;
	case 0x12:
		ret = EINVAL;
		return ret;
	case 0x13:
	case 0x14:
	case 0x15:
		ret = ECONNRESET;
		return ret;
	case 0x16:
		ret = ECONNABORTED;
		return ret;
	case 0x17:
		ret = ELOOP;
		return ret;
	case 0x18:
		ret = EACCES;
		return ret;
	case 0x1a:
		ret = EPROTONOSUPPORT;
		return ret;
	case 0x1b:
		ret = ECONNREFUSED;
		return ret;
	case 0x19:
	case 0x1e:
	case 0x23:
	case 0x24:
	case 0x25:
		ret = EPROTO;
		return ret;
	default:
		ret = ENOSYS;
		return ret;
	}
}
