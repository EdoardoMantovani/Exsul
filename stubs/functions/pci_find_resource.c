struct resource *pci_find_resource(struct pci_dev *pci_dev, struct resource *res){
	struct resource *r = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	for (int i = 0; i < PCI_STD_NUM_BARS; i++) {
		r = &pci_dev->resource[i];

		if (r->start != NULL && resource_contains(r, res)){
			return r;
		}
	}
	#endif
	return NULL;
}
