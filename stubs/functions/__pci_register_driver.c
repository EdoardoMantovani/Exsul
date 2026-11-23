int __pci_register_driver(struct pci_driver *pci_driver, struct module *owner, const char *mod_name){
	int ret = 0;
	u32 class = 0;
	u32 device = 0;
	u32 vendor = 0;
	u32 subvendor = 0;
	u32 subdevice = 0;
	const char *bdf = NULL;
	struct pci_dev *pci_dev = NULL;
	const struct pci_device_id *id = NULL;
	int (LINUX_ABI *probe)(struct pci_dev *, const struct pci_device_id *) = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	if(pci_driver == NULL || pci_driver->probe == NULL || pci_driver->id_table == NULL){
		ret = -EINVAL;
		return ret;
	}
	if(director.struct_sizes.pci_dev <= 0){
		ret = -EINVAL;
		return ret;
	}
	ret = pci_read_identity(bdf, &vendor, &device, &subvendor, &subdevice, &class);
	if(ret){
		return ret;
	}
	id = pci_match_id(pci_driver->id_table, vendor, device, subvendor, subdevice, class);
	if(id == NULL){
		ret = -ENODEV;
		return ret;
	}
	ret = vfio_open(bdf);
	if(ret){
		#ifdef ENABLE_DBG_PRINT
		printf("[!] failed to open vfio device %s\n", bdf);
		#endif
		return ret;
	}
	pci_dev = calloc(1, director.struct_sizes.pci_dev);
	if(pci_dev == NULL){
		vfio_close();
		ret = -ENOMEM;
		return ret;
	}
	ret = pci_prepare_device(pci_dev, bdf);
	if(ret){
		free(pci_dev);
		vfio_close();
		return ret;
	}

	director.bound_pci_driver = pci_driver;
	director.pci_device = pci_dev;
	director.pci_device_id = id;
	probe = pci_driver->probe;
	ret = probe(pci_dev, id);
	if(ret){
		director.pci_device = NULL;
		director.pci_device_id = NULL;
		director.bound_pci_driver = NULL;
		//free(pci_dev->dev.dma_mask);
		free(pci_dev);
		vfio_close();
	}
	#endif
	return ret;
}
