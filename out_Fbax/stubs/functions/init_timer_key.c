void init_timer_key(struct timer_list *timer, void (*func)(struct timer_list *), unsigned int flags, const char *name, void *key){
	timer->entry.pprev = NULL;
	timer->function = func;
	if ((flags & ~TIMER_INIT_FLAGS)){
		flags &= TIMER_INIT_FLAGS;
	}
}
