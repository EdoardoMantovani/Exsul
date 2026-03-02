unsigned char *skb_tail_pointer(const struct sk_buff *skb){
	#if BITS_PER_LONG > 32
	return *(unsigned char *)((char *)skb + director.runtime_generated_structs->structs[2].fields[2].field_offset) + *(sk_buff_data_t *)((char *)skb + director.runtime_generated_structs->structs[2].fields[4].field_offset);
	#else
	return *(sk_buff_data_t *)((char *)skb + director.runtime_generated_structs->structs[2].fields[4].field_offset);
	#endif
}

