int crypto_shash_init(struct shash_desc *desc){
	int ret = 0;

	if (crypto_shash_get_flags(desc->tfm) & CRYPTO_TFM_NEED_KEY){
		ret = -ENOKEY;
		return ret;
	}
	ret = __crypto_shash_init(desc);
	return ret;
}
