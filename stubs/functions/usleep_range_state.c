void usleep_range_state(unsigned long min, unsigned long max, unsigned int state){
	usleep((min + max) / 2);
}
