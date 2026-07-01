void *crypto_alloc_tfm_node(const char *alg_name, const struct crypto_type *frontend, u32 type, u32 mask, int node){
	int ret = 0;
	void *tfm = NULL;
	struct crypto_alg *alg = NULL;

	for (;;) {
		alg = crypto_find_alg(alg_name, frontend, type, mask);
		if (IS_ERR(alg)) {
			ret = PTR_ERR(alg);
			goto err;
		}

		tfm = crypto_create_tfm_node(alg, frontend, node);
		if (!IS_ERR(tfm)){
			return tfm;
		}

		crypto_mod_put(alg);
		ret = PTR_ERR(tfm);

		err:
		if (ret != -EAGAIN){
			break;
		}
		#if 0
		if (fatal_signal_pending(current)) {
			ret = -EINTR;
			break;
		}
		#endif
	}

	return ERR_PTR(ret);
}
