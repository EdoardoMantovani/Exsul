u32 get_random_u32(void){
	struct pcg32_random {
                u64 state;
                u64 inc;
        }prng_seed = {
		.state = 0,
		.inc = 0
	};

	return pcg32_random_r(&prng_seed);
}
