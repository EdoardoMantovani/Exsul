void *dmam_alloc_attrs(void *dev, size_t size, void *dma_handle, int gfp, unsigned long attrs){
	int ret = 0;
	int map_size = 0;
	u64 iova = 0;
	void *address = NULL;
	struct exsul_pci_dma *dma = NULL;

	#ifdef EXSUL_PCI_SUPPORT
	if(size == 0 || dma_handle == NULL){
		return NULL;
	}

	map_size = page_align(size);
	address = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if(address == MAP_FAILED){
		return NULL;
	}

	iova = iova_alloc(map_size);
	ret = vfio_dma_map(address, size, iova, true, &dma);
	if(ret){
		munmap(address, map_size);
		return NULL;
	}

	*(dma_addr_t *)dma_handle = dma->iova;
	#endif
	return address;
}
