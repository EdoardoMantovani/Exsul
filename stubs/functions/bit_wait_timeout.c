int bit_wait_timeout(struct wait_bit_key *key, int mode){
	int ret = 0;

	if(key == NULL){
		return -EINVAL;
	}
	if(!test_bit(key->bit_nr, key->flags)){
		return ret;
	}
	return key->timeout == 0 ? -EAGAIN : 1;
}
