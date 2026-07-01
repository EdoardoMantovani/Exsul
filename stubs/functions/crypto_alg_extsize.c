unsigned int crypto_alg_extsize(struct crypto_alg *alg){
	int ret = 0;

	ret = alg->cra_ctxsize + (alg->cra_alignmask & ~(crypto_tfm_ctx_alignment() - 1));
	return ret;
}
