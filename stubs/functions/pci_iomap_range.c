void *pci_iomap_range(struct pci_dev *pci_dev, int bar, unsigned long offset, unsigned long maxlen){
	u64 length = 0;
	void *base = NULL;
	#ifdef EXSUL_PCI_SUPPORT
	struct vfio_region_info *region = NULL;

	if(bar < 0 || bar >= PCI_MAX_BARS){
		return NULL;
	}
	region = &director.vfio->regions[VFIO_PCI_BAR0_REGION_INDEX + bar];
	if(region->size == 0 || !(region->flags & VFIO_REGION_INFO_FLAG_MMAP) || offset >= region->size){
		return NULL;
	}
	length = region->size - offset;
	if(maxlen && maxlen < length){
		length = maxlen;
	}
	if(length == 0){
		return NULL;
	}
	base = director.vfio->bars[bar];
	if(base == NULL || base == MAP_FAILED){
		base = vfio_map_bar(bar);
		if(base == NULL || base == MAP_FAILED){
			return NULL;
		}
	}

	return (void __iomem *)((u8 *)base + offset);
	#else
	return NULL;
	#endif
}
