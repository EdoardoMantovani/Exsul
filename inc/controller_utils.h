/* header file which contains functions for interacting with the 'mod_controller' struct */

/* used for retrieving at runtime the size of a not defined struct in the
 * main source code (usually this is for 'struct device' or others which changes in every kernel version)
 */

static int get_struct_size(unsigned char *struct_name){
	int ret = 0;
	struct runtime_struct *runtime_structs = NULL;
	struct runtime_struct_layout *runtime_struct_layout = NULL;
	struct struct_field *current_struct_fields = NULL;
	runtime_structs = &runtime_generated_structs;

	if(struct_name == NULL){
		ret = -EINVAL;
		return ret;
	}

	for (int counter = 0; ; counter++) {
		runtime_struct_layout = &runtime_structs->structs[counter];
		if (runtime_struct_layout->struct_name == NULL){
			break;
		}else{
			if(strcmp(struct_name, runtime_struct_layout->struct_name) == 0){
				ret =runtime_struct_layout->struct_size;
				return ret;
			}
		}
	}
	/* if it arrives here, it means that nothing was found */
	ret = -EINVAL;
	return ret;
}
