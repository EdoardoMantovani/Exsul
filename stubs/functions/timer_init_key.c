void timer_init_key(struct timer_list *timer, void (*func)(struct timer_list *), unsigned int flags, const char *name, struct lock_class_key *key){
	/* inline 'do_init_timer' here */
	timer->entry.pprev = NULL;
	timer->function = func;
	if (flags & ~TIMER_INIT_FLAGS){
		flags &= TIMER_INIT_FLAGS;
	}
	timer->flags = flags;
}
