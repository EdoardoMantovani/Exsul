void crypto_destroy_tfm(void *mem, struct crypto_tfm *tfm){
	struct crypto_alg *alg = NULL;
	struct crypto_type *type = NULL;

	if (unlikely(!mem) || IS_ERR_VALUE((unsigned long)mem)){
		return;
	}

	if (!refcount_dec_and_test(&tfm->refcnt)){
		return;
	}

	alg = tfm->__crt_alg;

	if (!tfm->exit && alg->cra_exit){
		alg->cra_exit(tfm);
	}

	/* inline 'crypto_exit_ops' here */
	type = tfm->__crt_alg->cra_type;

	if (type && tfm->exit){
		tfm->exit(tfm);
	}

	/* inline also 'crypto_alg_put' here */
	if (refcount_dec_and_test(&alg->cra_refcnt)){
		crypto_destroy_alg(alg);
	}

	kfree_sensitive(mem);
}
