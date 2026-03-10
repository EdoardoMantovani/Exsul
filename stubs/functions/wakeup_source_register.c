struct wakeup_source *wakeup_source_register(struct device *dev, const char *name){
	int ret = 0;
	struct wakeup_source *ws = NULL;

	/* inline here the function 'wakeup_source_create' */
	ws = (struct wakeup_source *)malloc(sizeof(struct wakeup_source));
	memset(ws, 0, sizeof(struct wakeup_source));
	ws->name = strdup(name);

	if (ws != NULL) {
		/* inline also 'wakeup_source_add' */
		ws->active = false;
		list_add_rcu(&ws->entry, &director.wakeup_sources);
	}
	return ws;
}
