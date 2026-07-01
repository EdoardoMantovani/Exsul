int hex_to_bin(char ch){
	int ret = 0;

	if ((ch >= '0') && (ch <= '9')){
		return ch - '0';
	}
	ch = tolower(ch);
	if ((ch >= 'a') && (ch <= 'f')){
		return ch - 'a' + 10;
	}

	ret = -EINVAL;
	return ret;
}
