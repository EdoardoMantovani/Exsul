void *find_vpid(int nr){
	static unsigned long pid = 0;

	return &pid;
}
