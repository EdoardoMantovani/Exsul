int pci_set_cacheline_size(struct pci_dev *pci_dev){
	int ret = 0;
	u8 cacheline_size = 0;
	u8 pci_cache_line_size = 16;

	#ifdef EXSUL_PCI_SUPPORT
	/* Validate current setting: the PCI_CACHE_LINE_SIZE must be
	   equal to or multiple of the right value. */
	pci_read_config_byte(pci_dev, PCI_CACHE_LINE_SIZE, &cacheline_size);
	if (cacheline_size >= pci_cache_line_size && (cacheline_size % pci_cache_line_size) == 0){
		return ret;
	}
	/* Write the correct value. */
	pci_write_config_byte(pci_dev, PCI_CACHE_LINE_SIZE, pci_cache_line_size);
	/* Read it back. */
	pci_read_config_byte(pci_dev, PCI_CACHE_LINE_SIZE, &cacheline_size);
	if (cacheline_size == pci_cache_line_size){
		return ret;
	}
	#endif
	ret = -EINVAL;
	return ret;
}
