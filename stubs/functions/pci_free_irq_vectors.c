void pci_free_irq_vectors(struct pci_dev *pci_dev){
	int ret = 0;
	#ifdef EXSUL_PCI_SUPPORT
	struct vfio_irq_set set = {};

	for(int i = 0; i < director.vfio->irq_count; i++){
		free_irq(i + 1, NULL);
	}

	set.argsz = sizeof(set);
	set.flags = VFIO_IRQ_SET_DATA_NONE | VFIO_IRQ_SET_ACTION_TRIGGER;
	set.index = director.vfio->irq_index;
	set.count = 0;
	ioctl(director.vfio->device_fd, VFIO_DEVICE_SET_IRQS, &set);
	director.vfio->irq_count = 0;
	#endif
}
