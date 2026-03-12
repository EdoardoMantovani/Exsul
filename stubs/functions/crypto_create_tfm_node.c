void *crypto_create_tfm_node(struct crypto_alg *alg, struct crypto_type *frontend, int node){
	int ret = 0;
	char *mem = NULL;
	struct crypto_tfm *tfm = NULL;
	struct crypto_type *type = NULL;

	mem = crypto_alloc_tfmmem(alg, frontend, node, GFP_KERNEL);
	if (IS_ERR(mem)){
		goto out;
	}

	tfm = (struct crypto_tfm *)(mem + frontend->tfmsize);
	tfm->fb = tfm;

	ret = frontend->init_tfm(tfm);
	if (ret){
		goto out_free_tfm;
	}

	if (!tfm->exit && alg->cra_init && (ret = alg->cra_init(tfm))){
		goto cra_init_failed;
	}

	goto out;

cra_init_failed:
	/* inline 'crypto_exit_ops' here */
	type = tfm->__crt_alg->cra_type;

	if (type && tfm->exit){
		tfm->exit(tfm);
	}

out_free_tfm:
	if (ret == -EAGAIN){
		crypto_shoot_alg(alg);
	}
	kfree(mem);
	mem = ERR_PTR(ret);
out:
	return mem;
}
