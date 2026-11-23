int pci_alloc_irq_vectors(struct pci_dev *pci_dev, unsigned int min_vecs, unsigned int max_vecs, unsigned int flags){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	ret = vfio_irq_select(min_vecs, max_vecs, flags);
	#endif
	return ret;
}
