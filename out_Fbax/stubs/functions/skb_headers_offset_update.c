void skb_headers_offset_update(struct sk_buff *skb, int off){
	/* Only adjust this if it actually is csum_start rather than csum */
	if (((char *)skb + director.runtime_generated_structs->structs[2].fields[17].field_offset) == CHECKSUM_PARTIAL){
		((char *)skb + director.runtime_generated_structs->structs[2].fields[18].field_offset) += off;
	}
	/* {transport,network,mac}_header and tail are relative to ((char *)skb + director.runtime_generated_structs->structs[2].fields[2].field_offset) */
	((char *)skb + director.runtime_generated_structs->structs[2].fields[19].field_offset) += off;
	((char *)skb + director.runtime_generated_structs->structs[2].fields[20].field_offset)   += off;
	if (skb_mac_header_was_set(skb)){
		((char *)skb + director.runtime_generated_structs->structs[2].fields[16].field_offset) += off;
	}
	((char *)skb + director.runtime_generated_structs->structs[2].fields[15].field_offset) += off;
	((char *)skb + director.runtime_generated_structs->structs[2].fields[14].field_offset) += off;
	((char *)skb + director.runtime_generated_structs->structs[2].fields[13].field_offset) += off;
}
