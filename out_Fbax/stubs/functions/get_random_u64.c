u64 get_random_u64(void){
	struct pcg32_random {
                u64 state;
                u64 inc;
        }prng_seed = {
                .state = 0,
                .inc = 0
        };

        return (u64)pcg32_random_r(&prng_seed);
}
