int crypto_aead_setkey(struct crypto_aead *tfm, const u8 *key, unsigned int keylen){
	unsigned long alignmask = crypto_aead_alignmask(tfm);
	int err;

	if ((unsigned long)key & alignmask)
		err = setkey_unaligned(tfm, key, keylen);
	else
		err = crypto_aead_alg(tfm)->setkey(tfm, key, keylen);

	if (unlikely(err)) {
		crypto_aead_set_flags(tfm, CRYPTO_TFM_NEED_KEY);
		return err;
	}

	crypto_aead_clear_flags(tfm, CRYPTO_TFM_NEED_KEY);
	return 0;
}
