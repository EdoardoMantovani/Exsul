int dmi_check_system(const struct dmi_system_id *list){
	int ret = 0;
	int count = 0;
	const struct dmi_system_id *d = NULL;

	for (d = list; !dmi_is_end_of_table(d); d++){
		if (dmi_matches(d)) {
			count++;
			if (d->callback && d->callback(d)){
				break;
			}
		}
	}
	ret = count;
	return ret;
}

