void pci_unregister_driver(struct pci_driver *pci_driver){
	int ret = 0;
	struct device *device = NULL;
	struct pci_dev *pci_device = NULL;
	void (LINUX_ABI *remove)(struct pci_dev *) = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	if(pci_driver == NULL || pci_driver != director.bound_pci_driver){
		goto end;
	}
	remove = pci_driver->remove;
	if(remove != NULL && director.pci_device != NULL){
		remove(director.pci_device);
	}

	if(director.pci_device != NULL){
		pci_device = director.pci_device;
		if(pci_device != NULL){
			device = &pci_device->dev;
			free(device->dma_mask);
		}
		free(director.pci_device);
	}
	director.pci_device = NULL;
	director.pci_device_id = NULL;
	director.bound_pci_driver = NULL;
	vfio_close();
	#endif
	end:
}
