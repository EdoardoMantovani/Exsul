void kvfree_call_rcu(struct rcu_head *head, void *ptr){
	if(ptr != NULL){
		free(ptr);
	}
}

