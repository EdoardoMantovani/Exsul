void get_random_bytes(void *buf, size_t len){
	u32 val = 0;

	if(buf == NULL || len == 0){
		goto end;
	}

	for(u16 j = 0; j < len; j += sizeof(u32)){
		val = get_random_u32();
		memcpy(buf + j, &val, sizeof(u32));
	}
	end:
}
