struct pci_ops *pci_bus_set_ops(struct pci_bus *bus, struct pci_ops *ops){
	struct pci_ops *old_ops = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	old_ops = bus->ops;
	bus->ops = ops;
	return old_ops;
	#else
	return NULL;
	#endif
}
