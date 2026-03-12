int crypto_aead_setauthsize(struct crypto_aead *tfm, unsigned int authsize){
	int err;

	if ((!authsize && crypto_aead_maxauthsize(tfm)) || authsize > crypto_aead_maxauthsize(tfm)){
		return -EINVAL;
	}

	if (crypto_aead_alg(tfm)->setauthsize) {
		err = crypto_aead_alg(tfm)->setauthsize(tfm, authsize);
		if (err){
			return err;
		}
	}

	tfm->authsize = authsize;
	return 0;
}
