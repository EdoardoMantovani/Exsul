char *strim(char *s){
	size_t size = 0;
	char *end = NULL;

	#ifdef ENABLE_UNWIND_DEBUGGING
        #define _U                              0x01    /* upper */
        #define _L                              0x02    /* lower */
        #define _D                              0x04    /* digit */
        #define _C                              0x08    /* cntrl */
        #define _P                              0x10    /* punct */
        #define _S                              0x20    /* white space (space/lf/tab) */
        #define _X                              0x40    /* hex digit */
        #define _SP                             0x80    /* hard space (0x20) */
        #endif

	size = strlen(s);
	if (!size){
		return s;
	}

	end = s + size - 1;
	while (end >= s && isspace(*end)){
		end--;
	}
	*(end + 1) = '\0';

	return skip_spaces(s);
}
