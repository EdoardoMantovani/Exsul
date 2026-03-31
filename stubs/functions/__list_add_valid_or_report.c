bool __list_add_valid_or_report(struct list_head *new, struct list_head *prev, struct list_head *next){
	if (CHECK_DATA_CORRUPTION(prev == NULL, NULL, "list_add corruption. prev is NULL.\n") ||
	    CHECK_DATA_CORRUPTION(next == NULL, NULL, "list_add corruption. next is NULL.\n") ||
	    CHECK_DATA_CORRUPTION(next->prev != prev, next, "list_add corruption. next->prev should be prev (%px), but was %px. (next=%px).\n", prev, next->prev, next) ||
	    CHECK_DATA_CORRUPTION(prev->next != next, prev, "list_add corruption. prev->next should be next (%px), but was %px. (prev=%px).\n", next, prev->next, prev) ||
	    CHECK_DATA_CORRUPTION(new == prev || new == next, NULL, "list_add double add: new=%px, prev=%px, next=%px.\n", new, prev, next)){
		return false;
	}
	return true;
}
