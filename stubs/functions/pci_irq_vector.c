int pci_irq_vector(struct pci_dev *pci_dev, unsigned int vector){
	int ret = 0;

	#ifdef EXSUL_PCI_SUPPORT
	if(vector >= director.vfio->irq_count){
		ret = -EINVAL;
		return ret;
	}
	ret = vector + 1;
	#endif
	return ret;
}
