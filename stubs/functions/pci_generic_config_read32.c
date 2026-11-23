int pci_generic_config_read32(struct pci_bus *pci_bus, unsigned int devfn, int where, int size, u32 *val){
	int ret = 0;
	void __iomem *addr = NULL;
	struct pci_ops *pci_ops = NULL;
	void __iomem *(LINUX_ABI *map_bus)(struct pci_bus *bus, unsigned int devfn, int where) = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	pci_ops = pci_bus->ops;
	map_bus = pci_ops->map_bus;
	addr = map_bus(pci_bus, devfn, where & ~0x3);
	if (addr == NULL){
		ret = PCIBIOS_DEVICE_NOT_FOUND;
		return ret;
	}
	*val = readl(addr);

	if (size <= 2){
		*val = (*val >> (8 * (where & 3))) & ((1 << (size * 8)) - 1);
	}
	ret = PCIBIOS_SUCCESSFUL;
	#endif
	return ret;
}
