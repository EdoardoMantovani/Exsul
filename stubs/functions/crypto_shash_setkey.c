int crypto_shash_setkey(struct crypto_shash *tfm, const u8 *key, unsigned int keylen){
	int ret = 0;
	struct shash_alg *shash = NULL;

	shash = crypto_shash_alg(tfm);
	ret = shash->setkey(tfm, key, keylen);
	if (unlikely(ret)) {
		shash_set_needkey(tfm, shash);
		return ret;
	}
	crypto_shash_clear_flags(tfm, CRYPTO_TFM_NEED_KEY);
	return ret;
}
