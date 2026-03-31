bool __list_del_entry_valid_or_report(struct list_head *entry){
	struct list_head *prev, *next;

	prev = entry->prev;
	next = entry->next;

	if (CHECK_DATA_CORRUPTION(next == NULL, NULL,
			"list_del corruption, %px->next is NULL\n", entry) ||
	    CHECK_DATA_CORRUPTION(prev == NULL, NULL,
			"list_del corruption, %px->prev is NULL\n", entry) ||
	    CHECK_DATA_CORRUPTION(next == LIST_POISON1, next,
			"list_del corruption, %px->next is LIST_POISON1 (%px)\n",
			entry, LIST_POISON1) ||
	    CHECK_DATA_CORRUPTION(prev == LIST_POISON2, prev,
			"list_del corruption, %px->prev is LIST_POISON2 (%px)\n",
			entry, LIST_POISON2) ||
	    CHECK_DATA_CORRUPTION(prev->next != entry, prev,
			"list_del corruption. prev->next should be %px, but was %px. (prev=%px)\n",
			entry, prev->next, prev) ||
	    CHECK_DATA_CORRUPTION(next->prev != entry, next,
			"list_del corruption. next->prev should be %px, but was %px. (next=%px)\n",
			entry, next->prev, next)){
		return false;
	}
	return true;
}
