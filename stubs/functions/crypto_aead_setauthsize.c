int crypto_aead_setauthsize(struct crypto_aead *tfm, unsigned int authsize){
	int ret = 0;

	if ((!authsize && crypto_aead_maxauthsize(tfm)) || authsize > crypto_aead_maxauthsize(tfm)){
		ret = -EINVAL;
		return ret;
	}
	if (crypto_aead_alg(tfm)->setauthsize) {
		ret = crypto_aead_alg(tfm)->setauthsize(tfm, authsize);
		if (ret){
			return ret;
		}
	}
	tfm->authsize = authsize;
	return ret;
}
