int kstrtobool(const char *s, bool *res){
	int ret = 0;

	if (!s){
		ret = -EINVAL;
		return ret;
	}
	switch (s[0]) {
	case 'e':
	case 'E':
	case 'y':
	case 'Y':
	case 't':
	case 'T':
	case '1':
		*res = true;
		return 0;
	case 'd':
	case 'D':
	case 'n':
	case 'N':
	case 'f':
	case 'F':
	case '0':
		*res = false;
		return 0;
	case 'o':
	case 'O':
		switch (s[1]) {
		case 'n':
		case 'N':
			*res = true;
			return 0;
		case 'f':
		case 'F':
			*res = false;
			return 0;
		default:
			break;
		}
		break;
	default:
		break;
	}

	ret = -EINVAL;
	return ret;
}
