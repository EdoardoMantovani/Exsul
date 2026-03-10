struct kobject *kobject_get(struct kobject *kobj){
	if (kobj) {
		kref_get(&kobj->kref);
	}
	return kobj;
}
