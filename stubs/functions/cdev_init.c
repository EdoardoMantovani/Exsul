void cdev_init(struct cdev *cdev, const struct file_operations *fops){
	memset(cdev, 0, sizeof(struct cdev));
	INIT_LIST_HEAD(&cdev->list);
	cdev->ops = fops;
}
