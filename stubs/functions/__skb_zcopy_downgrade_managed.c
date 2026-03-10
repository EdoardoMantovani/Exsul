void __skb_zcopy_downgrade_managed(struct sk_buff *skb){
	skb_shinfo(skb)->flags &= ~SKBFL_MANAGED_FRAG_REFS;
	for (int i = 0; i < skb_shinfo(skb)->nr_frags; i++){
		skb_frag_ref(skb, i);
	}
}
