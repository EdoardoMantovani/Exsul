void msleep(unsigned int msecs){
	struct timespec req = {};
	struct timespec rem = {};

	req.tv_sec  = (time_t)(msecs / 1000U);
	req.tv_nsec = (long)((msecs % 1000U) * 1000000UL);

	while (nanosleep(&req, &rem) == -1 && errno == EINTR) {
		req = rem;
	}
}
