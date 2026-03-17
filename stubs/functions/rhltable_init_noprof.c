int rhltable_init_noprof(struct rhltable *hlt, const struct rhashtable_params *params){
	int ret = 0;

	ret = rhashtable_init_noprof(&hlt->ht, params);
	hlt->ht.rhlist = true;
	return ret;
}
