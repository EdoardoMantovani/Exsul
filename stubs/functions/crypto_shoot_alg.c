void crypto_shoot_alg(struct crypto_alg *alg){
	alg->cra_flags |= CRYPTO_ALG_DYING;
}
