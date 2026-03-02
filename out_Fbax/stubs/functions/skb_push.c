void *skb_push(struct sk_buff *skb, unsigned int len){
	*(unsigned char *)((char *)skb + director.runtime_generated_structs->structs[2].fields[1].field_offset) -= len;
	*(int *)((char *)skb + director.runtime_generated_structs->structs[2].fields[0].field_offset)  += len;
	if (unlikely(((char *)skb + director.runtime_generated_structs->structs[2].fields[1].field_offset) < ((char *)skb + director.runtime_generated_structs->structs[2].fields[2].field_offset))){
		skb_under_panic(skb, len, __builtin_return_address(0));
	}
	return *(unsigned char *)((char *)skb + director.runtime_generated_structs->structs[2].fields[1].field_offset);
}
