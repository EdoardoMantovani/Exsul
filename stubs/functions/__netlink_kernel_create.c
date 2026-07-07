void *__netlink_kernel_create(void *net, int unit, void *module, void *cfg){
	static unsigned char netlink_socket;

	return &netlink_socket;
}
