struct crypto_aead *crypto_alloc_aead(const char *alg_name, u32 type, u32 mask){
	static const struct crypto_type crypto_aead_type = {
		.extsize = crypto_alg_extsize,
		.init_tfm = crypto_aead_init_tfm,
		.free = crypto_aead_free_instance,
		.maskclear = ~CRYPTO_ALG_TYPE_MASK,
		.maskset = CRYPTO_ALG_TYPE_MASK,
		.type = CRYPTO_ALG_TYPE_AEAD,
		.tfmsize = offsetof(struct crypto_aead, base),
		.algsize = offsetof(struct aead_alg, base),
	};

	return crypto_alloc_tfm(alg_name, &crypto_aead_type, type, mask);
}
