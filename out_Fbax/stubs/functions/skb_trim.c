void skb_trim(struct sk_buff *skb, unsigned int len){
	if (((char *)skb + director.runtime_generated_structs->structs[2].fields[0].field_offset) > len){
		__skb_trim(skb, len);
	}
}
