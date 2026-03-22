/* contains the main code used for calculating/emulating jiffies */

/**
 * time_is_after_jiffies - return true if a is after jiffies
 * @a: time (unsigned long) to compare to jiffies
 *
 * Return: %true is time a is after jiffies, otherwise %false.
 */
#define time_is_after_jiffies(a) time_before(jiffies, a)

/*
 * HZ is equal to or smaller than 1000, and 1000 is a nice round
 * multiple of HZ, divide with the factor between them, but round
 * upwards:
 */
static inline unsigned long _msecs_to_jiffies(const unsigned int m){
	return (m + (MSEC_PER_SEC / HZ) - 1) / (MSEC_PER_SEC / HZ);
}

#if !(USEC_PER_SEC % HZ)
static inline unsigned long _usecs_to_jiffies(const unsigned int u){
	return (u + (USEC_PER_SEC / HZ) - 1) / (USEC_PER_SEC / HZ);
}
#else
static inline unsigned long _usecs_to_jiffies(const unsigned int u){
	return (USEC_TO_HZ_MUL32 * u + USEC_TO_HZ_ADJ32) >> USEC_TO_HZ_SHR32;
}
#endif

static unsigned long round_jiffies_common(unsigned long j, int cpu, bool force_up){
	int rem = 0;
	unsigned long original = j;

	/*
	 * We don't want all cpus firing their timers at once hitting the
	 * same lock or cachelines, so we skew each extra cpu with an extra
	 * 3 jiffies. This 3 jiffies came originally from the mm/ code which
	 * already did this.
	 * The skew is done by adding 3*cpunr, then round, then subtract this
	 * extra offset again.
	 */
	j += cpu * 3;

	rem = j % HZ;

	/*
	 * If the target jiffy is just after a whole second (which can happen
	 * due to delays of the timer irq, long irq off times etc etc) then
	 * we should round down to the whole second, not up. Use 1/4th second
	 * as cutoff for this rounding as an extreme upper bound for this.
	 * But never round down if @force_up is set.
	 */
	if (rem < HZ/4 && !force_up){ /* round down */
		j = j - rem;
	}else{ /* round up */
		j = j - rem + HZ;
	}
	/* now that we have rounded, subtract the extra skew again */
	j -= cpu * 3;

	/*
	 * Make sure j is still in the future. Otherwise return the
	 * unmodified value.
	 */
	if(((long)j - director.jiffies) < 0){
		return j;
	}else{
		return original;
	}
}

static inline unsigned long msecs_to_jiffies(const unsigned int m){
	if (__builtin_constant_p(m)) {
		if ((int)m < 0){
			return MAX_JIFFY_OFFSET;
		}
		return _msecs_to_jiffies(m);
	}else{
		return __msecs_to_jiffies(m);
	}
}
