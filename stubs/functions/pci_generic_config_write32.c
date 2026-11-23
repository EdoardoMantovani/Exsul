int pci_generic_config_write32(struct pci_bus *pci_bus, unsigned int devfn, int where, int size, u32 val){
	int ret = 0;
	u32 tmp = 0;
	u32 mask = 0;
	void __iomem *addr = NULL;
	struct pci_ops *pci_ops = NULL;
	void __iomem *(LINUX_ABI *map_bus)(struct pci_bus *bus, unsigned int devfn, int where) = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	if(pci_bus == NULL){
		ret = -EINVAL;
		return ret;
	}
	pci_ops = pci_bus->ops;
	map_bus = pci_ops->map_bus;
	addr = map_bus(pci_bus, devfn, where & ~0x3);
	if (addr == NULL){
		ret = PCIBIOS_DEVICE_NOT_FOUND;
		return ret;
	}
	if (size == 4) {
		writel(val, addr);
		ret = PCIBIOS_SUCCESSFUL;
		return ret;
	}

	mask = ~(((1 << (size * 8)) - 1) << ((where & 0x3) * 8));
	tmp = readl(addr) & mask;
	tmp |= val << ((where & 0x3) * 8);
	writel(tmp, addr);

	ret = PCIBIOS_SUCCESSFUL;
	#endif
	return ret;
}
