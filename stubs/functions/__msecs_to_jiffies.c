unsigned long __msecs_to_jiffies(const unsigned int m){
	if ((int)m < 0){
		return MAX_JIFFY_OFFSET;
	}
	return _msecs_to_jiffies(m);
}
