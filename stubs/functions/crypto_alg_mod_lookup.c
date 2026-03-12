struct crypto_alg *crypto_alg_mod_lookup(const char *name, u32 type, u32 mask){
	int ok = 0;
	struct crypto_alg *alg = NULL;
	struct crypto_alg *larval = NULL;

	/*
	 * If the internal flag is set for a cipher, require a caller to
	 * invoke the cipher with the internal flag to use that cipher.
	 * Also, if a caller wants to allocate a cipher that may or may
	 * not be an internal cipher, use type | CRYPTO_ALG_INTERNAL and
	 * !(mask & CRYPTO_ALG_INTERNAL).
	 */
	if (!((type | mask) & CRYPTO_ALG_INTERNAL)){
		mask |= CRYPTO_ALG_INTERNAL;
	}

	//larval = crypto_larval_lookup(name, type, mask);

	if (IS_ERR(larval) || !crypto_is_larval(larval)){
		return larval;
	}

	ok = crypto_probing_notify(CRYPTO_MSG_ALG_REQUEST, larval);

	if (ok == NOTIFY_STOP){
	//	alg = crypto_larval_wait(larval, type, mask);
	}else{
		crypto_mod_put(larval);
		alg = ERR_PTR(-ENOENT);
	}
//	crypto_larval_kill(container_of(larval, struct crypto_larval, alg));
	return alg;
}
