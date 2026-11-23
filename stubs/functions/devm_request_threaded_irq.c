int devm_request_threaded_irq(struct device *dev, unsigned int irq, irq_handler_t handler, irq_handler_t thread_fn, unsigned long irqflags, const char *devname, void *dev_id){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	if(irq == 0){
		ret = -EINVAL;
		return ret;
	}
	ret = vfio_irq_bind(irq - 1, handler, thread_fn, dev_id);
	#endif
	return ret;
}
