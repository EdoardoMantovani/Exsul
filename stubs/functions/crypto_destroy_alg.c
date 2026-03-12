void crypto_destroy_alg(struct crypto_alg *alg){
	if (alg->cra_type && alg->cra_type->destroy){
		alg->cra_type->destroy(alg);
	}

	if (alg->cra_destroy != NULL){
		alg->cra_destroy(alg);
	}
}
