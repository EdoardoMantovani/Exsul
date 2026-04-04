void sk_skb_reason_drop(struct sock *sk, struct sk_buff *skb, int reason){
	/* Must check the refcount before freeing. The inline __sk_skb_reason_drop
	 * in skb.h does this, but this exported stub was missing it, causing
	 * use-after-free when skb->users > 1. */
	if (!skb_unref(skb)){
		return;
	}
	__kfree_skb(skb);
}
