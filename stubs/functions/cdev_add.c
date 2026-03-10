int cdev_add(struct cdev *p, u32 dev, unsigned count){
	int ret = 0;

	p->dev = dev;
	p->count = count;

	if (dev == WHITEOUT_DEV) {
		ret = -EBUSY;
		goto err;
	}

	kobject_get(p->kobj.parent);

	return ret;
err:
	kfree_const(p->kobj.name);
	p->kobj.name = NULL;
	return ret;
}
