int crypto_aead_setkey(struct crypto_aead *tfm, const u8 *key, unsigned int keylen){
	int ret = 0;
	unsigned long alignmask = 0;

	alignmask = crypto_aead_alignmask(tfm);
	if ((unsigned long)key & alignmask){
		ret = setkey_unaligned(tfm, key, keylen);
	}else{
		ret = crypto_aead_alg(tfm)->setkey(tfm, key, keylen);
	}

	if (unlikely(ret)) {
		crypto_aead_set_flags(tfm, CRYPTO_TFM_NEED_KEY);
		return ret;
	}

	crypto_aead_clear_flags(tfm, CRYPTO_TFM_NEED_KEY);
	return ret;
}
