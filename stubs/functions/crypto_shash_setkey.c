int crypto_shash_setkey(struct crypto_shash *tfm, const u8 *key, unsigned int keylen){
	int err = 0;
	struct shash_alg *shash = NULL;

	shash = crypto_shash_alg(tfm);

	err = shash->setkey(tfm, key, keylen);
	if (unlikely(err)) {
		shash_set_needkey(tfm, shash);
		return err;
	}

	crypto_shash_clear_flags(tfm, CRYPTO_TFM_NEED_KEY);
	return 0;
}
