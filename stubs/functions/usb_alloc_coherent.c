void *usb_alloc_coherent(void *dev, size_t size, int mem_flags, unsigned long *dma){
	void *buffer = NULL;

	buffer = calloc(1, size);
	if(dma != NULL){
		*dma = (unsigned long)(uintptr_t)buffer;
	}
	return buffer;
}
