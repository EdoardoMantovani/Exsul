const void *free_irq(unsigned int irq, void *dev_id){
	int ret = 0;
	u64 wake_up = true;
	struct exsul_pci_irq *entry = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	if(irq == 0 || irq > PCI_MAX_IRQS){
		return NULL;
	}
	entry = &director.vfio->irqs[irq - 1];
	if(entry->started){
		atomic_store(&entry->stop, true);
		write(entry->event_fd, &wake_up, sizeof(wake_up));
		pthread_join(entry->thread, NULL);
		close(entry->event_fd);
		memset(entry, 0, sizeof(*entry));
		entry->event_fd = -1;
	}
	#endif
	return NULL;
}
