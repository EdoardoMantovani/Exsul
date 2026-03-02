void *skb_pull_data(struct sk_buff *skb, size_t len){
	void *data = NULL;
	data = ((char *)skb + director.runtime_generated_structs->structs[2].fields[1].field_offset);

	if (((char *)skb + director.runtime_generated_structs->structs[2].fields[0].field_offset) < len){
		return NULL;
	}

	skb_pull(skb, len);
	return data;
}
