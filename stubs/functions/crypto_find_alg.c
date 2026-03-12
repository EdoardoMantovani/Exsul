struct crypto_alg *crypto_find_alg(const char *alg_name, const struct crypto_type *frontend, u32 type, u32 mask){
	if (frontend) {
		type &= frontend->maskclear;
		mask &= frontend->maskclear;
		type |= frontend->type;
		mask |= frontend->maskset;
	}

	return crypto_alg_mod_lookup(alg_name, type, mask);
}
