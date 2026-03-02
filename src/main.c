#include <layer.h>
#include <ieee80211.h>
#include <prototypes.h>
#include <cb_table.h>

#include <arm64/arm64.h>
#include <x86_64/x86_64.h>

struct module_controller director;

int dwarf_search_field(unsigned char *field_input_name, Dwarf_Debug dbg, Dwarf_Die struct_die){
	int ret = 0;
	int sibling = 0;
	Dwarf_Unsigned field_offset = 0;
	Dwarf_Error err = 0;
	Dwarf_Die cur = 0;
	Dwarf_Die sib = 0;
	Dwarf_Die child = 0;
	Dwarf_Half tag = 0;
	Dwarf_Attribute attr = 0;
	Dwarf_Unsigned exprlen = 0;
	char *block = NULL;
	char *field_name = NULL;

	ret = dwarf_child(struct_die, &child, &err);
	if (ret != DW_DLV_OK){
		ret = -ENOENT;
		return ret;
	}

	cur = child;

	for (;;) {
		tag = 0;
		ret = dwarf_tag(cur, &tag, &err);
		if (ret == DW_DLV_OK && tag == DW_TAG_member) {
			ret = dwarf_diename(cur, &field_name, &err);
			if (ret == DW_DLV_OK && field_name && strcmp(field_name, (char *)field_input_name) == 0) {
				field_offset = 0;
				ret = dwarf_attr(cur, DW_AT_data_member_location, &attr, &err);
				if (ret == DW_DLV_OK) {
					if (dwarf_formudata(attr, &field_offset, &err) != DW_DLV_OK) {
						if (dwarf_formexprloc(attr, &exprlen, &block, &err) == DW_DLV_OK && exprlen > 0 && block) {
							if (block[0] == DW_OP_plus_uconst || block[0] == DW_OP_constu)
								field_offset = read_uleb128(block + 1, (size_t)exprlen - 1);
							}
						}
						dwarf_dealloc(dbg, attr, DW_DLA_ATTR);
					}
					return (int)field_offset;
				}
		}

		sibling = dwarf_siblingof(dbg, cur, &sib, &err);
		dwarf_dealloc(dbg, cur, DW_DLA_DIE);
		if (sibling != DW_DLV_OK){
			break;
		}
		cur = sib;
	}

	ret = -ENOENT;
	return ret;
}

int dwarf_detect_struct(Dwarf_Debug dbg, Dwarf_Die die, unsigned char *struct_to_search, Dwarf_Die *struct_found){
	int ret = 0;
	Dwarf_Error err = 0;
	Dwarf_Half tag = 0;
	Dwarf_Die child = 0;
	Dwarf_Die cur = 0;
	Dwarf_Die sib = 0;
	char *die_name = NULL;

	ret = dwarf_tag(die, &tag, &err);
	if (ret != DW_DLV_OK){
		ret = false;
		return ret;
	}

	if (tag == DW_TAG_structure_type) {
		ret = dwarf_diename(die, &die_name, &err);
		if (ret == DW_DLV_OK && die_name && strcmp(die_name, (char *)struct_to_search) == 0) {
			*struct_found = die;
			ret = true;
			return ret;
		}
	}

	ret = dwarf_child(die, &child, &err);
	if (ret != DW_DLV_OK){
		ret = false;
		return ret;
	}

	cur = child;
	for (;;) {
		ret = dwarf_detect_struct(dbg, cur, struct_to_search, struct_found);
		if (ret == true) {
			if (*struct_found != cur){
				dwarf_dealloc(dbg, cur, DW_DLA_DIE);
			}
			ret = true;
			return ret;
		}
		if (ret < 0) {
			dwarf_dealloc(dbg, cur, DW_DLA_DIE);
			return ret;
		}

		ret = dwarf_siblingof(dbg, cur, &sib, &err);
		dwarf_dealloc(dbg, cur, DW_DLA_DIE);
		if (ret != DW_DLV_OK){
			break;
		}
		cur = sib;
	}
	ret = 0;
	return ret;
}

int dwarf_loader_parser(unsigned char *struct_to_search, unsigned char *field_to_search, unsigned short *field_offset){
	int ret = 0;
	int fd = -1;
	Dwarf_Debug dbg = 0;
	Dwarf_Error err = 0;

	Dwarf_Unsigned cu_header_len = 0;
	Dwarf_Unsigned abbrev_off = 0;
	Dwarf_Unsigned next_cu = 0;
	Dwarf_Half version = 0;
	Dwarf_Half addr_size = 0;

	Dwarf_Die found = 0;
	Dwarf_Die cu_die = 0;

	if(director.fd < 0){
		ret = -EINVAL;
		return ret;
	}
	fd = dup(director.fd);
	if (fd < 0){
		ret = -EINVAL;
		return ret;
	}

	ret = dwarf_init(fd, DW_DLC_READ, NULL, NULL, &dbg, &err);
	if (ret != DW_DLV_OK) {
		close(fd);
		ret = -EINVAL;
		return ret;
	}

	while (dwarf_next_cu_header(dbg, &cu_header_len, &version, &abbrev_off, &addr_size, &next_cu, &err) == DW_DLV_OK) {
		cu_die = 0;
		ret = dwarf_siblingof(dbg, NULL, &cu_die, &err);
		if (ret != DW_DLV_OK){
			continue;
		}

		if (dwarf_detect_struct(dbg, cu_die, struct_to_search, &found)) {
			dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
			break;
		}
		dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
	}

	if (found == 0) {
		goto out;
	}

	ret = dwarf_search_field(field_to_search, dbg, found);
	if (ret >= 0){
		*field_offset = ret;
	}

	out:
	if (found == true){
		dwarf_dealloc(dbg, found, DW_DLA_DIE);
	}

	dwarf_finish(dbg, &err);
	close(fd);
	return ret;
}

int dwarf_load_struct(Dwarf_Debug dbg, Dwarf_Error err, unsigned char *struct_to_search){
	int ret = 0;
	Dwarf_Unsigned cu_header_len = 0;
	Dwarf_Unsigned abbrev_off = 0;
	Dwarf_Unsigned next_cu = 0;
	Dwarf_Half version = 0;
	Dwarf_Half addr_size = 0;
	Dwarf_Die found = 0;
	Dwarf_Die cu_die = 0;

	/* iterate for every 'compilation unit' inside the dwarf section */
	while (dwarf_next_cu_header(dbg, &cu_header_len, &version, &abbrev_off, &addr_size, &next_cu, &err) == DW_DLV_OK) {
		cu_die = 0;
		ret = dwarf_siblingof(dbg, NULL, &cu_die, &err);
		if(ret != DW_DLV_OK){
			continue;
		}
		ret = dwarf_detect_struct(dbg, cu_die, struct_to_search, &found);
		if(ret){
			dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
			break;
		}
		dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
	}

	if(found == 0){
		printf("[!] struct %s not found, switching to the backup plan!\n", struct_to_search);
		goto backup_plan;
	}

	backup_plan:
	/* if for any reason the .ko hasn't any dwarf/btf debug information, use a precomputed kernel offset table generated during the build time. */
	#ifdef ENABLE_OFFSET_BACKUP

	#endif
	end:
	return ret;
}

void *search_similar_function(unsigned char *function_name){
	int n = 0;
	void *fn = NULL;
	void *strtab = NULL;
	Elf(Sym) *sym = NULL;
	Elf(Shdr) *symsect = NULL;
	struct load_info *info = NULL;
	unsigned char *cur_name = NULL;

	info = &director.info;

	symsect = &info->sechdrs[info->index.sym];
	sym = (void *)symsect->sh_addr;
        strtab = (void *)info->sechdrs[info->index.str].sh_addr;
        n = symsect->sh_size / sizeof(*sym);

        for (unsigned int i = 0; i < n; i++) {
		cur_name = &strtab[sym[i].st_name];

		if (cur_name == NULL || cur_name[0] == '\0'){
			continue;
		}

		if (strstr((char *)function_name, cur_name) != NULL) {
			fn = (void *)sym[i].st_value;
			return fn;
		}
	}
	return fn;
}

void *get_symbol_data(unsigned char *symbol_name, int *symbol_size){
	int n = 0;
	void *ptr = NULL;
	void *strtab = NULL;
	Elf(Sym) *sym = NULL;
	Elf(Shdr) *symsect = NULL;
	struct load_info *info = NULL;
	unsigned char *cur_name = NULL;

	info = &director.info;
	symsect = &info->sechdrs[info->index.sym];
	sym = (void *)symsect->sh_addr;
	strtab = (void *)info->sechdrs[info->index.str].sh_addr;
	n = symsect->sh_size / sizeof(*sym);

	for (unsigned int i = 0; i < n; i++) {
		cur_name = &strtab[sym[i].st_name];

		if (cur_name == NULL || cur_name[0] == '\0'){
			continue;
		}

		if (strstr((char *)symbol_name, cur_name) != NULL) {
			*symbol_size = sym[i].st_size;
			ptr = malloc(sym[i].st_size);
			memset(ptr, 0, sym[i].st_size);
			memcpy(ptr, sym[i].st_value, sym[i].st_size);
		}
	}
	return ptr;
}

int parse_module_offset_table(void){
	int ret = 0;
	u8 counter = 0;
	struct runtime_struct *runtime_generated_structs = NULL;
	struct runtime_struct_layout *runtime_struct_layout = NULL;
	struct struct_field *current_struct_fields = NULL;
	runtime_generated_structs = director.runtime_generated_structs;

	for (counter = 0; ; counter++) {
		runtime_struct_layout = &runtime_generated_structs->structs[counter];
		if (runtime_struct_layout->struct_name == NULL){
			break;
		}
		if(runtime_struct_layout->struct_field_num == 0){
			continue;
		}
		current_struct_fields = runtime_struct_layout->fields;
		for (u8 k = 0; k < runtime_struct_layout->struct_field_num; k++) {
			if(current_struct_fields[k].field_name == NULL){
				break;
			}
			ret = dwarf_loader_parser(runtime_struct_layout->struct_name, current_struct_fields[k].field_name, &current_struct_fields[k].field_offset);
			if(ret > 0){
				printf("  - %s->%s offset: %d\n",
					runtime_struct_layout->struct_name,
					current_struct_fields[k].field_name,
					current_struct_fields[k].field_offset);
			}
		}
	}

	dwarf_finish(director.dbg, &director.err);
	return ret;
}

int parse_module_usb_table(void){
	int ret = 0;
	int usb_table_size = 0;
	void *p = NULL;
	struct usb_device_id *usb_device_table_ptr = NULL;

	p = get_symbol_data(USB_TABLE_SYM_NAME, &usb_table_size);
	if(p == NULL){
		ret = -ENOMEM;
		return ret;
	}

	ret = (usb_table_size % sizeof(struct usb_device_id));
	if(ret != 0){
		printf("[!] usb device table symbol isn't aligned with the size of \'usb_device_id\', aborting...\n");
		free(p);
		ret = -EINVAL;
		return ret;
	}

	printf("[*] usb devices detected for the current kernel module:\n");
	for(u8 j = 0; j < ret; j++){
		usb_device_table_ptr = (struct usb_device_id *)p;
		printf("[*] usb pid: 0x%08x vid: 0x%08x\n", usb_device_table_ptr->idProduct, usb_device_table_ptr->idVendor);
		/* now add the pid/vid tuple into our main struct, this will be used later for the libusb device handling */
		for(u8 k = 0; k < ARRAY_SIZE(director.usb_dev_list); k++){
			if(director.usb_dev_list[k].is_used == false){
				director.usb_dev_list[k].usb_vid = usb_device_table_ptr->idVendor;
				director.usb_dev_list[k].usb_pid = usb_device_table_ptr->idProduct;
				director.usb_dev_list[k].is_used = true;
				break;
			}
		}
		p += sizeof(struct usb_device_id);
	}

	free(p);
	return ret;
}

/* start of the core kernel functions related to the loading of lkm */
unsigned char *module_next_tag_pair(char *string, unsigned long *secsize) {
	while (string[0]) {
		string++;
		if ((*secsize)-- <= 1) {
			return NULL;
		}
	}

	while (!string[0]) {
		string++;
		if ((*secsize)-- <= 1) {
			return NULL;
		}
	}
	return string;
}

static char *get_next_modinfo(const struct load_info *info, const char *tag,
			      char *prev) {
	unsigned char *p      = NULL;
	unsigned int   taglen = strlen(tag);
	Elf(Shdr) *infosec    = &info->sechdrs[info->index.info];
	unsigned long size    = infosec->sh_size;

	char *modinfo = (char *)info->hdr + infosec->sh_offset;

	if (prev) {
		size -= prev - modinfo;
		modinfo = module_next_tag_pair(prev, &size);
	}

	for (p = modinfo; p; p = module_next_tag_pair(p, &size)) {
		if (strncmp(p, tag, taglen) == 0 && p[taglen] == '=') {
			return p + taglen + 1;
		}
	}
	return NULL;
}

static char *get_modinfo(const struct load_info *info, const char *tag) {
	return get_next_modinfo(info, tag, NULL);
}

static int elf_validity_cache_copy(struct load_info *info, int flags) {
	unsigned int i		   = 0;
	Elf(Shdr) *shdr		   = NULL;
	Elf(Shdr) *strhdr	   = NULL;
	int	     err	   = 0;
	unsigned int num_mod_secs  = 0;
	unsigned int mod_idx	   = 0;
	unsigned int num_info_secs = 0;
	unsigned int info_idx	   = 0;
	unsigned int num_sym_secs  = 0;
	unsigned int sym_idx	   = 0;

	if (info->hdr->e_shentsize != sizeof(Elf(Shdr))) {
		printf("Invalid ELF section header size\n");
		goto no_exec;
	}

	info->sechdrs = (void *)info->hdr + info->hdr->e_shoff;

	if (info->hdr->e_shstrndx == SHN_UNDEF ||
	    info->hdr->e_shstrndx >= info->hdr->e_shnum) {
		printf("Invalid ELF section name index: %d || e_shstrndx (%d) "
		       ">= e_shnum (%d)\n",
		       info->hdr->e_shstrndx, info->hdr->e_shstrndx,
		       info->hdr->e_shnum);
		goto no_exec;
	}

	strhdr = &info->sechdrs[info->hdr->e_shstrndx];

	if (strhdr->sh_offset == 0x00) {
		printf("sh offset of string header points to 0!\n");
		goto no_exec;
	}

	info->secstrings = (void *)info->hdr + strhdr->sh_offset;

	if (strhdr->sh_size == 0) {
		printf("empty section name table\n");
		goto no_exec;
	}
	if (info->secstrings[strhdr->sh_size - 1] != '\0') {
		printf("ELF Spec violation: section name table isn't null "
		       "terminated\n");
		goto no_exec;
	}

	if (info->sechdrs[0].sh_type != SHT_NULL ||
	    info->sechdrs[0].sh_size != 0 || info->sechdrs[0].sh_addr != 0) {
		printf("ELF Spec violation: section 0 type(%d)!=SH_NULL or "
		       "non-zero len or addr\n",
		       info->sechdrs[0].sh_type);
		goto no_exec;
	}

	for (i = 1; i < info->hdr->e_shnum; i++) {
		shdr = &info->sechdrs[i];
		switch (shdr->sh_type) {
		case SHT_NULL:
		case SHT_NOBITS:
			/* do nothing */
			continue;
		case SHT_SYMTAB:

			if (shdr->sh_link == SHN_UNDEF ||
			    shdr->sh_link >= info->hdr->e_shnum) {
				printf("Invalid ELF sh_link!=SHN_UNDEF(%d) or "
				       "(sh_link(%d) >= hdr->e_shnum(%d)\n",
				       shdr->sh_link, shdr->sh_link,
				       info->hdr->e_shnum);
				goto no_exec;
			}

			num_sym_secs++;
			sym_idx = i;
			fallthrough;
		default:
			if (strcmp(info->secstrings + shdr->sh_name,
				   ".gnu.linkonce.this_module") == 0) {
				num_mod_secs++;
				mod_idx = i;
				printf("detected .gnu.linkonce.this_module\n");
			} else if (strcmp(info->secstrings + shdr->sh_name,
					  ".modinfo") == 0) {
				num_info_secs++;
				info_idx = i;
				printf("detected .modinfo\n");
			}
			if (shdr->sh_flags & SHF_ALLOC) {
				if (shdr->sh_name >= strhdr->sh_size) {
					printf("Invalid ELF section name in "
					       "module (section %u type %u)\n",
					       i, shdr->sh_type);
					return -ENOEXEC;
				}
			}
			break;
		}
	}

	if (num_info_secs > 1) {
		printf("Only one .modinfo section must exist.\n");
		goto no_exec;
	} else if (num_info_secs == 1) {
		/* Try to find a name early so we can log errors with a module
		 * name */
		info->index.info = info_idx;
		info->name	 = get_modinfo(info, "name");
	}

	info->index.sym = sym_idx;
	shdr		= &info->sechdrs[sym_idx];
	info->index.str = shdr->sh_link;
	info->strtab	= (unsigned char *)info->hdr +
		       info->sechdrs[info->index.str].sh_offset;

	if (num_mod_secs != 1) {
		printf("module %s: Only one .gnu.linkonce.this_module section "
		       "must exist.\n",
		       info->name
			   ?: "(missing .modinfo section or name field)");
		goto no_exec;
	}

	shdr = &info->sechdrs[mod_idx];

	if (shdr->sh_type == SHT_NOBITS) {
		printf("module %s: .gnu.linkonce.this_module section must have "
		       "a size set\n",
		       info->name
			   ?: "(missing .modinfo section or name field)");
		goto no_exec;
	}

	if (!(shdr->sh_flags & SHF_ALLOC)) {
		printf("module %s: .gnu.linkonce.this_module must occupy "
		       "memory during process execution\n",
		       info->name
			   ?: "(missing .modinfo section or name field)");
		goto no_exec;
	}

	if (shdr->sh_size != sizeof(struct module)) {
		printf(
		    "module %s: .gnu.linkonce.this_module section size must "
		    "match the kernel's built struct module size at run time\n",
		    info->name ?: "(missing .modinfo section or name field)");
	}

	info->index.mod	 = mod_idx;
	info->mod	 = (void *)info->hdr + shdr->sh_offset;
	info->index.vers = 0;

	return 0;

no_exec:
	return -ENOEXEC;
}

static int rewrite_section_headers(struct load_info *info, int flags) {
	Elf(Shdr) *shdr		 = NULL;
	info->sechdrs[0].sh_addr = 0;

	for (unsigned int i = 1; i < info->hdr->e_shnum; i++) {
		shdr	      = &info->sechdrs[i];
		shdr->sh_addr = (size_t)info->hdr + shdr->sh_offset;
	}

	info->sechdrs[info->index.vers].sh_flags &= ~(unsigned long)SHF_ALLOC;
	info->sechdrs[info->index.info].sh_flags &= ~(unsigned long)SHF_ALLOC;

	return 0;
}

static int early_mod_check(struct load_info *info, int flags) {
	int err = 0;
	err	= rewrite_section_headers(info, flags);

	return err;
}

static inline bool within_module_mem_type(unsigned long	       addr,
					  const struct module *mod,
					  enum mod_mem_type    type) {
	unsigned long base = 0;
	unsigned long size;

	base = (unsigned long)mod->mem[type].base;
	size = mod->mem[type].size;
	return addr - base < size;
}

int module_enforce_rwx_sections(Elf(Ehdr) * hdr, Elf(Shdr) * sechdrs,
				char *secstrings, struct module *mod) {
	const unsigned long shf_wx = SHF_WRITE | SHF_EXECINSTR;
	int		    i	   = 0;

	for (i = 0; i < hdr->e_shnum; i++) {
		if ((sechdrs[i].sh_flags & shf_wx) == shf_wx) {
			printf("%s: section %s (index %d) has invalid "
			       "WRITE|EXEC flags\n",
			       mod->name, secstrings + sechdrs[i].sh_name, i);
			return -ENOEXEC;
		}
	}
	return 0;
}

static unsigned int find_sec(const struct load_info *info, const char *name) {
	Elf(Shdr) *shdr = NULL;
	for (int i = 1; i < info->hdr->e_shnum; i++) {
		shdr = &info->sechdrs[i];
		/* Alloc bit cleared means "ignore it." */
		if ((shdr->sh_flags & SHF_ALLOC) &&
		    strcmp(info->secstrings + shdr->sh_name, name) == 0) {
			return i;
		}
	}
	return 0;
}

bool module_init_section(const char *name) { return strstarts(name, ".init"); }

bool module_exit_section(const char *name) { return strstarts(name, ".exit"); }

bool module_init_layout_section(const char *sname) {
	return module_init_section(sname);
}

long module_get_offset_and_type(struct module *mod, enum mod_mem_type type,
				Elf(Shdr) * sechdr, unsigned int      section) {
	long offset = 0;
	long mask   = 0;

	mask = ((unsigned long)(type)&SH_ENTSIZE_TYPE_MASK)
	       << SH_ENTSIZE_TYPE_SHIFT;

	mod->mem[type].size += 0;
	offset = ALIGN(mod->mem[type].size, sechdr->sh_addralign ?: 1);
	mod->mem[type].size = offset + sechdr->sh_size;

	return offset | mask;
}

static void __layout_sections(struct module *mod, struct load_info *info,
			      bool is_init) {
	unsigned int	  m = 0;
	unsigned int	  i = 0;
	enum mod_mem_type type;
	Elf(Shdr) *s	  = NULL;
	const char *sname = NULL;

	static const unsigned long masks[][2] = {
	    {SHF_EXECINSTR | SHF_ALLOC, 0x00},
	    {SHF_ALLOC, SHF_WRITE | 0x00},
	    {SHF_RO_AFTER_INIT | SHF_ALLOC, 0x00},
	    {SHF_WRITE | SHF_ALLOC, 0x00},
	    {0x00 | SHF_ALLOC, 0}};

	static const int core_m_to_mem_type[] = {
	    MOD_TEXT, MOD_RODATA, MOD_RO_AFTER_INIT, MOD_DATA, MOD_DATA,
	};

	static const int init_m_to_mem_type[] = {
	    MOD_INIT_TEXT, MOD_INIT_RODATA, MOD_INVALID,
	    MOD_INIT_DATA, MOD_INIT_DATA,
	};

	for (m = 0; m < sizeof(masks); ++m) {
		type = is_init ? init_m_to_mem_type[m] : core_m_to_mem_type[m];

		for (i = 0; i < info->hdr->e_shnum; ++i) {
			s     = &info->sechdrs[i];
			sname = info->secstrings + s->sh_name;

			if ((s->sh_flags & masks[m][0]) != masks[m][0] ||
			    (s->sh_flags & masks[m][1]) ||
			    s->sh_entsize != ~0UL ||
			    is_init != module_init_layout_section(sname)) {
				continue;
			}

			if ((type == MOD_INVALID)) {
				continue;
			}

			s->sh_entsize =
			    module_get_offset_and_type(mod, type, s, i);
			printf("\t%s\n", sname);
		}
	}
}

static void layout_sections(struct module *mod, struct load_info *info) {
	for (unsigned int i = 0; i < info->hdr->e_shnum; i++) {
		info->sechdrs[i].sh_entsize = ~0UL;
	}

	printf("Core section allocation order for %s:\n", mod->name);
	__layout_sections(mod, info, false);

	printf("Init section allocation order for %s:\n", mod->name);
	__layout_sections(mod, info, true);
	return;
}

static int module_memory_alloc(struct module *mod, enum mod_mem_type type) {
	unsigned int size = PAGE_ALIGN(mod->mem[type].size);
	void	    *ptr;

	mod->mem[type].size = size;

	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC,
		   MAP_PRIVATE | MAP_ANON | MAP_32BIT, -1, 0x00);

	if (ptr == NULL) {
		return -ENOMEM;
	}

	memset(ptr, 0, size);
	mod->mem[type].base = ptr;

	return 0;
}

static void module_memory_free(struct module *mod, enum mod_mem_type type,
			       bool unload_codetags) {
	void *ptr = mod->mem[type].base;

	if (mod_mem_type_is_core_data(type)) {
		return;
	}

	// TOOD
	//      munmap(ptr);
}

static int move_module(struct module *mod, struct load_info *info) {
	int		  ret  = ENOMEM;
	int		  i    = 0;
	enum mod_mem_type t    = 0;
	enum mod_mem_type type = 0;
	void		 *dest = NULL;
	Elf(Shdr) *shdr	       = NULL;

	for_each_mod_mem_type(type) {
		if (!mod->mem[type].size) {
			mod->mem[type].base = NULL;
			continue;
		}

		ret = module_memory_alloc(mod, type);
		if (ret) {
			t = type;
			goto out_enomem;
		}
	}

	printf("Final section addresses for %s:\n", mod->name);
	for (i = 0; i < info->hdr->e_shnum; i++) {
		dest = NULL;
		shdr = &info->sechdrs[i];
		type = shdr->sh_entsize >> SH_ENTSIZE_TYPE_SHIFT;

		if (!(shdr->sh_flags & SHF_ALLOC)) {
			continue;
		}

		dest = mod->mem[type].base +
		       (shdr->sh_entsize & SH_ENTSIZE_OFFSET_MASK);

		if (shdr->sh_type != SHT_NOBITS) {
			memcpy(dest, (void *)shdr->sh_addr, shdr->sh_size);
		}
		shdr->sh_addr = (unsigned long)dest;
		printf("\t0x%lx 0x%.8lx %s\n", (long)shdr->sh_addr,
		       (long)shdr->sh_size, info->secstrings + shdr->sh_name);
	}

	return 0;
out_enomem:
	for (t--; t >= 0; t--) {
		module_memory_free(mod, t, true);
	}
	return ret;
}

static bool is_core_symbol(const Elf(Sym) * src, const Elf(Shdr) * sechdrs,
			   unsigned int shnum, unsigned int pcpundx) {
	const Elf(Shdr) * sec;
	enum mod_mem_type type;

	if (src->st_shndx == SHN_UNDEF || src->st_shndx >= shnum ||
	    !src->st_name) {
		return false;
	}

	sec  = sechdrs + src->st_shndx;
	type = sec->sh_entsize >> SH_ENTSIZE_TYPE_SHIFT;
	if (!(sec->sh_flags & SHF_ALLOC) || mod_mem_type_is_init(type)) {
		return false;
	}

	return true;
}

void layout_symtab(struct module *mod, struct load_info *info) {
	Elf(Shdr) *symsect = (Elf(Shdr) *)(info->sechdrs + info->index.sym);
	Elf(Shdr) *strsect = (Elf(Shdr) *)(info->sechdrs + info->index.str);

	Elf(Sym) *src				= NULL;
	unsigned int	      i			= 0;
	unsigned int	      nsrc		= 0;
	unsigned int	      ndst		= 0;
	unsigned int	      strtab_size	= 0;
	struct module_memory *mod_mem_data	= &mod->mem[MOD_DATA];
	struct module_memory *mod_mem_init_data = &mod->mem[MOD_INIT_DATA];

	symsect->sh_flags |= SHF_ALLOC;
	symsect->sh_entsize = module_get_offset_and_type(
	    mod, MOD_INIT_DATA, symsect, info->index.sym);
	printf("\t%s\n", info->secstrings + symsect->sh_name);

	src  = (void *)info->hdr + symsect->sh_offset;
	nsrc = symsect->sh_size / sizeof(*src);

	for (ndst = i = 0; i < nsrc; i++) {
		if (i == 0 ||
		    is_core_symbol(src + i, info->sechdrs, info->hdr->e_shnum,
				   info->index.pcpu)) {
			strtab_size +=
			    strlen(&info->strtab[src[i].st_name]) + 1;
			ndst++;
		}
	}

	info->symoffs = ALIGN(mod_mem_data->size, symsect->sh_addralign ?: 1);
	info->stroffs = mod_mem_data->size =
	    info->symoffs + ndst * sizeof(Elf(Sym));
	mod_mem_data->size += strtab_size;
	info->core_typeoffs = mod_mem_data->size;
	mod_mem_data->size += ndst * sizeof(char);

	strsect->sh_flags |= SHF_ALLOC;
	strsect->sh_entsize = module_get_offset_and_type(
	    mod, MOD_INIT_DATA, strsect, info->index.str);
	printf("\t%s\n", info->secstrings + strsect->sh_name);

	mod_mem_init_data->size =
	    ALIGN(mod_mem_init_data->size, __alignof__(struct mod_kallsyms));
	info->mod_kallsyms_init_off = mod_mem_init_data->size;

	mod_mem_init_data->size += sizeof(struct mod_kallsyms);
	info->init_typeoffs = mod_mem_init_data->size;
	mod_mem_init_data->size += nsrc * sizeof(char);
}

static struct module *layout_and_allocate(struct load_info *info, int flags) {
	struct module *mod = NULL;
	unsigned int   ndx = 0;
	int	       err = 0;

	err = module_frob_arch_sections(info->hdr, info->sechdrs,
					info->secstrings, info->mod);

	if (err < 0) {
		return NULL;
	}

	err = module_enforce_rwx_sections(info->hdr, info->sechdrs,
					  info->secstrings, info->mod);

	if (err < 0) {
		return NULL;
	}

	info->sechdrs[info->index.pcpu].sh_flags &= ~(unsigned long)SHF_ALLOC;

	ndx = find_sec(info, ".data..ro_after_init");

	if (ndx) {
		printf("section %s found!\n", ".data..ro_after_init");
		info->sechdrs[ndx].sh_flags |= SHF_RO_AFTER_INIT;
	}

	ndx = find_sec(info, "__jump_table");

	if (ndx) {
		printf("section %s found!\n", "__jump_table");
		info->sechdrs[ndx].sh_flags |= SHF_RO_AFTER_INIT;
	}

	layout_sections(info->mod, info);

	layout_symtab(info->mod, info);

	err = move_module(info->mod, info);
	if (err) {
		printf("failed to move module!\n");
		return NULL;
	}

	mod = (void *)info->sechdrs[info->index.mod].sh_addr;

	return mod;
}

static int percpu_modalloc(struct module *mod, struct load_info *info) {
	int ret = 0;
	/* UP modules shouldn't have this section: ENOMEM isn't quite right */
	if (info->sechdrs[info->index.pcpu].sh_size != 0) {
		ret = -ENOMEM;
		return ret;
	}
	return ret;
}

static void *section_objs(const struct load_info *info, const char *name,
			  size_t object_size, unsigned int *num) {
	unsigned int sec = find_sec(info, name);

	/* Section 0 has sh_addr 0 and sh_size 0. */
	*num = info->sechdrs[sec].sh_size / object_size;
	return (void *)info->sechdrs[sec].sh_addr;
}

static void *section_addr(const struct load_info *info, const char *name) {
	/* Section 0 has sh_addr 0. */
	return (void *)info->sechdrs[find_sec(info, name)].sh_addr;
}

void *find_runtime_section(struct module *mod, struct load_info *info,
			   unsigned char *section_name) {
	int		  ret  = 0;
	unsigned int	  i    = 0;
	enum mod_mem_type type = 0;
	void		 *dest = NULL;
	Elf(Shdr) *shdr	       = NULL;

	for (i = 0; i < info->hdr->e_shnum; i++) {
		dest = NULL;
		shdr = &info->sechdrs[i];
		type = shdr->sh_entsize >> SH_ENTSIZE_TYPE_SHIFT;

		if (!(shdr->sh_flags & SHF_ALLOC)) {
			continue;
		}

		dest = mod->mem[type].base +
		       (shdr->sh_entsize & SH_ENTSIZE_OFFSET_MASK);

		if (strcmp((unsigned char *)(info->secstrings + shdr->sh_name),
			   section_name) == 0) {
			return (void *)shdr->sh_addr;
		}
	}
out_enomem:
	return NULL;
}

static int find_module_sections(struct module *mod, struct load_info *info) {
	int ret = 0;
	mod->kp = section_objs(info, "__param", sizeof(*mod->kp), &mod->num_kp);
	mod->syms =
	    section_objs(info, "__ksymtab", sizeof(*mod->syms), &mod->num_syms);
	mod->crcs     = section_addr(info, "__kcrctab");
	mod->gpl_syms = section_objs(
	    info, "__ksymtab_gpl", sizeof(*mod->gpl_syms), &mod->num_gpl_syms);
	mod->gpl_crcs = section_addr(info, "__kcrctab_gpl");

#ifdef CONFIG_CONSTRUCTORS
	mod->ctors =
	    section_objs(info, ".ctors", sizeof(*mod->ctors), &mod->num_ctors);
	if (!mod->ctors) {
		mod->ctors = section_objs(info, ".init_array",
					  sizeof(*mod->ctors), &mod->num_ctors);
	} else if (find_sec(info, ".init_array")) {
		printf("%s: has both .ctors and .init_array.\n", mod->name);
		ret = -EINVAL;
		return ret;
	}
#endif

	mod->noinstr_text_start =
	    section_objs(info, ".noinstr.text", 1, &mod->noinstr_text_size);

	mod->extable = section_objs(info, "__ex_table", sizeof(*mod->extable),
				    &mod->num_exentries);

	if (section_addr(info, "__obsparm")) {
		printf("%s: Ignoring obsolete parameters\n", mod->name);
	}

	#ifdef ENABLE_DYNDBG
	mod->dyndbg_info.descs =
	    section_objs(info, "__dyndbg", sizeof(*mod->dyndbg_info.descs),
			 &mod->dyndbg_info.num_descs);
	mod->dyndbg_info.classes = section_objs(
	    info, "__dyndbg_classes", sizeof(*mod->dyndbg_info.classes),
	    &mod->dyndbg_info.num_classes);
	#endif
	return ret;
}

uint64_t find_runtime_relocation_by_name(struct module	  *mod,
					 struct load_info *info,
					 unsigned char	  *name) {
	Elf(Shdr) *symsec = (Elf(Shdr) *)&info->sechdrs[info->index.sym];
	Elf(Sym) *sym	  = (void *)symsec->sh_addr;

	Elf(Addr) st_value = 0;

	unsigned char *text_runtime = find_runtime_section(mod, info, ".text");

	for (unsigned int i = 1; i < info->hdr->e_shnum; i++) {
		unsigned int infosec = info->sechdrs[i].sh_info;

		if (info->sechdrs[i].sh_type == SHT_RELA) {
			Elf(Rela) *tmp_rela =
			    (Elf(Rela) *)(info->sechdrs[i].sh_addr);
			for (unsigned int j = 0;
			     j < info->sechdrs[i].sh_size /
				     info->sechdrs[i].sh_entsize;
			     j++) {
			}
		}
	}
}

static int simplify_symbols(struct module *mod, struct load_info *info) {
	Elf(Shdr) *symsec = (Elf(Shdr) *)&info->sechdrs[info->index.sym];
	Elf(Sym) *sym	  = (void *)symsec->sh_addr;
	unsigned long	      secbase	 = 0;
	unsigned int	      i		 = 0;
	int		      ret	 = 0;
	struct kernel_symbol *ksym	 = NULL;
	void		     *dlopen_ctx = NULL;
	void                 *trampoline = NULL;
	unsigned char	     *name	 = NULL;

	for (i = 1; i < symsec->sh_size / sizeof(Elf(Sym)); i++) {
		name = (unsigned char *)(info->strtab + sym[i].st_name);

		switch (sym[i].st_shndx) {
		case SHN_COMMON:
			if (!strncmp(name, "__gnu_lto", 9)) {
				break;
			}
			printf("Common symbol: %s\n", name);
			ret = -ENOEXEC;
			break;
		case SHN_ABS:
			printf("Absolute symbol:  0x%08lx %s\n",
			       (long)sym[i].st_value, name);
			break;
		case SHN_UNDEF:
			void *target = NULL;
			for (int j = 0; sym_table[j].function_ptr != NULL;
			     j++) {
				if (strcmp(
					(char *)name,
					(char *)sym_table[j].function_name) ==
				    0) {
					target = sym_table[j].function_ptr;
					break;
				}
			}
			if (target == NULL) {
				dlopen_ctx = dlopen(NULL, 0);
				target	   = dlsym(dlopen_ctx, (char *)name);
			}
			if (target == NULL) {
				printf("missing function: %s\n", name);
				ret = -ENOEXEC;
				break;
			}
#if defined(__x86_64__)
			/* only trampoline functions; objects are another story
			 */
			if (ELF64_ST_TYPE(sym[i].st_info) == STT_FUNC) {
				trampoline = x86_64_make_trampoline(target);
				if (trampoline == NULL) {
					printf(
					    "trampoline alloc failed for %s\n",
					    name);
					ret = -ENOMEM;
					break;
				}
				sym[i].st_value = (Elf64_Addr)(uintptr_t)trampoline;
#ifdef ENABLE_DBG_PRINT
				printf("resolved %-24s target=%p tramp=%p\n",
				       name, target, trampoline);
#endif
			} else {
				sym[i].st_value = (Elf64_Addr)(uintptr_t)target;
#ifdef ENABLE_DBG_PRINT
				printf("resolved %-24s target=%p (no tramp)\n",
				       name, target);
#endif
			}
#else
			sym[i].st_value = (Elf64_Addr)(uintptr_t)target;
#endif
			break;
		default:
			if (sym[i].st_shndx == info->index.pcpu) {
				secbase = (unsigned long)(mod->percpu);
			} else {
				secbase =
				    info->sechdrs[sym[i].st_shndx].sh_addr;
			}
			sym[i].st_value += secbase;
			break;
		}
	}

	return ret;
}

static int apply_relocations(struct module *mod, const struct load_info *info) {
	int err = 0;

	for (unsigned int i = 1; i < info->hdr->e_shnum; i++) {
		unsigned int infosec = info->sechdrs[i].sh_info;

		if (infosec >= info->hdr->e_shnum) {
			continue;
		}

		if (!(info->sechdrs[infosec].sh_flags & SHF_ALLOC)) {
			continue;
		}

		if (info->sechdrs[i].sh_flags & SHF_RELA_LIVEPATCH) {
			printf("LIVEPATCH not supported!\n");
			return err;
		}

		if (info->sechdrs[i].sh_type == SHT_REL) {
			err = apply_relocate(info->sechdrs, info->strtab,
					     info->index.sym, i, mod);
		} else if (info->sechdrs[i].sh_type == SHT_RELA) {
			err = apply_relocate_add(info->sechdrs, info->strtab,
						 info->index.sym, i, mod);
		}

		if (err < 0) {
			break;
		}
	}
	return err;
}

static char elf_type(const Elf(Sym) * sym, const struct load_info *info) {
	const Elf(Shdr) *sechdrs = NULL;

	sechdrs = info->sechdrs;

	if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
		if (ELF_ST_TYPE(sym->st_info) == STT_OBJECT) {
			return 'v';
		} else {
			return 'w';
		}
	}
	if (sym->st_shndx == SHN_UNDEF) {
		return 'U';
	}
	if (sym->st_shndx == SHN_ABS || sym->st_shndx == info->index.pcpu) {
		return 'a';
	}
	if (sym->st_shndx >= SHN_LORESERVE) {
		return '?';
	}
	if (sechdrs[sym->st_shndx].sh_flags & SHF_EXECINSTR) {
		return 't';
	}
	if (sechdrs[sym->st_shndx].sh_flags & SHF_ALLOC &&
	    sechdrs[sym->st_shndx].sh_type != SHT_NOBITS) {
		if (!(sechdrs[sym->st_shndx].sh_flags & SHF_WRITE)) {
			return 'r';
		} else if (sechdrs[sym->st_shndx].sh_flags & 0x00) {
			return 'g';
		} else {
			return 'd';
		}
	}
	if (sechdrs[sym->st_shndx].sh_type == SHT_NOBITS) {
		if (sechdrs[sym->st_shndx].sh_flags & 0x00) {
			return 's';
		} else {
			return 'b';
		}
	}
	if (strstarts(info->secstrings + sechdrs[sym->st_shndx].sh_name,
		      ".debug")) {
		return 'n';
	}
	return '?';
}

void add_kallsyms(struct module *mod, const struct load_info *info) {
	unsigned int i	    = 0;
	unsigned int ndst   = 0;
	ssize_t	     ret    = 0;
	const Elf(Sym) *src = NULL;
	Elf(Sym) *dst	    = NULL;
	char *s		    = NULL;

	Elf(Shdr) *symsec	     = &info->sechdrs[info->index.sym];
	unsigned long strtab_size    = 0;
	void	     *data_base	     = mod->mem[MOD_DATA].base;
	void	     *init_data_base = mod->mem[MOD_INIT_DATA].base;

	mod->kallsyms = (void *)init_data_base + info->mod_kallsyms_init_off;

	((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->symtab = (void *)symsec->sh_addr;
	((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->num_symtab =
	    symsec->sh_size / sizeof(Elf(Sym));
	((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->strtab =
	    (void *)info->sechdrs[info->index.str].sh_addr;
	((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->typetab =
	    init_data_base + info->init_typeoffs;

	mod->core_kallsyms.symtab = dst = data_base + info->symoffs;
	mod->core_kallsyms.strtab = s = data_base + info->stroffs;
	mod->core_kallsyms.typetab    = data_base + info->core_typeoffs;
	strtab_size		      = info->core_typeoffs - info->stroffs;
	src			      = rcu_dereference(mod->kallsyms)->symtab;

	for (ndst = i = 0; i < ((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->num_symtab;
	     i++) {
		((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->typetab[i] =
		    elf_type(src + i, info);
		if (i == 0 ||
		    is_core_symbol(src + i, info->sechdrs, info->hdr->e_shnum,
				   info->index.pcpu)) {
			ret = 0;

			mod->core_kallsyms.typetab[ndst] =
			    ((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))->typetab[i];
			dst[ndst]	    = src[i];
			dst[ndst++].st_name = s - mod->core_kallsyms.strtab;

			ret = strscpy(s,
				      &((struct mod_kallsyms *)rcu_dereference(mod->kallsyms))
					   ->strtab[src[i].st_name],
				      strtab_size);

			if (ret < 0) {
				break;
			}

			s += ret + 1;
			strtab_size -= ret + 1;
		}
	}

	mod->core_kallsyms.num_symtab = ndst;
}

static inline bool within_coretext(const struct core_text *ct, void *addr) {
	unsigned long p = (unsigned long)addr;

	return ct->base <= p && p < ct->end;
}

static inline bool within_module_coretext(void *addr) {
	bool	       ret = false;
	struct module *mod = NULL;

	/**
	mod = __module_address((unsigned long)addr);
	if (mod && within_module_core((unsigned long)addr, mod)){
		ret = true;
	}

	**/

	return ret;
}

static bool is_coretext(const struct core_text *ct, void *addr) {
	if (ct && within_coretext(ct, addr)) {
		return true;
	}

	return true;
}

void sort_extable(struct exception_table_entry *start,
		  struct exception_table_entry *finish) {
	// sort(start, finish - start, sizeof(struct exception_table_entry),
	// cmp_ex_sort, swap_ex);
}

static int post_relocation(struct module *mod, const struct load_info *info) {
	sort_extable(mod->extable, mod->extable + mod->num_exentries);

	add_kallsyms(mod, info);
	return module_finalize(info->hdr, info->sechdrs, mod);
}

static int module_set_memory(const struct module *mod, enum mod_mem_type type) {
	struct module_memory {
		void	    *base;
		unsigned int size;
	};

	unsigned int	      mprotect_perm = 0;
	struct module_memory *mod_mem = (struct module_memory *)&mod->mem[type];

	if (mod_mem->base == NULL) {
		return 0;
	}

	switch (type) {
	case MOD_RODATA:
	case MOD_INIT_RODATA:
	case MOD_RO_AFTER_INIT:
		mprotect_perm = PROT_READ | PROT_WRITE;
		break;
	case MOD_DATA:
	case MOD_INIT_DATA:
		mprotect_perm = PROT_READ | PROT_WRITE;
		break;
	case MOD_TEXT:
	case MOD_INIT_TEXT:
		mprotect_perm = PROT_READ | PROT_EXEC | PROT_WRITE;
		break;
	default:
		printf("[!] unknown memory type: %d\n", type);
		return 0;
	}

	return mprotect(mod_mem->base, mod_mem->size,
			mprotect_perm);
}

int module_enable_rodata_ro(const struct module *mod, bool after_init) {
	int ret = 0;

	ret = module_set_memory(mod, MOD_RODATA);

	if (ret) {
		return ret;
	}

	ret = module_set_memory(mod, MOD_INIT_RODATA);

	if (ret) {
		return ret;
	}

	if (after_init) {
		return module_set_memory(mod, MOD_RO_AFTER_INIT);
	}
	return 0;
}

int module_enable_data_nx(const struct module *mod) {
	int ret = 0;

	for_class_mod_mem_type(type, data) {
		ret = module_set_memory(mod, type);

		if (ret) {
			return ret;
		}
	}
	return 0;
}

int module_enable_text_rox(const struct module *mod) {
	int ret = 0;

	for_class_mod_mem_type(type, text) {
		ret = module_set_memory(mod, type);

		if (ret) {
			return ret;
		}
	}
	return 0;
}

static int complete_formation(struct module *mod, struct load_info *info) {
	int err = 0;

	// module_bug_finalize(info->hdr, info->sechdrs, mod);
	// module_cfi_finalize(info->hdr, info->sechdrs, mod);

	err = module_enable_rodata_ro(mod, false);

	if (err) {
		goto out_strict_rwx;
	}

	err = module_enable_data_nx(mod);

	if (err) {
		goto out_strict_rwx;
	}

	err = module_enable_text_rox(mod);

	if (err) {
		goto out_strict_rwx;
	}

	// mod->state    = MODULE_STATE_COMING;
	return 0;

out_strict_rwx:
	/** do nothing **/
out:
	return err;
}

static void do_mod_ctors(struct module *mod) {
	unsigned long i = 0;

	//        for (i = 0; i < mod->num_ctors; i++){
	//              mod->ctors[i]();
	//    }
}

static inline unsigned long ex_to_insn(const struct exception_table_entry *x) {
	return (unsigned long)&x->insn + x->insn;
}

static inline bool within_module_init(unsigned long	   addr,
				      const struct module *mod) {
	for_class_mod_mem_type(type, init) {
		if (within_module_mem_type(addr, mod, type)) {
			return true;
		}
	}
	return false;
}

void trim_init_extable(struct module *mod) {
	while (mod->num_exentries &&
	       within_module_init(ex_to_insn(&mod->extable[0]), mod)) {
		mod->extable++;
		mod->num_exentries--;
	}

	while (mod->num_exentries &&
	       within_module_init(ex_to_insn(&mod->extable[mod->num_exentries - 1]),
				  mod)) {
		mod->num_exentries--;
	}
}

static int do_init_module(struct module *mod) {
	int ret = 0;

	do_mod_ctors(mod);
	if (mod->init != NULL) {
		// ret = do_one_initcall(mod->init);
	}

	if (ret > 0) {
		printf("%s: '%s'->init suspiciously returned %d, it should "
		       "follow 0/-E convention\n"
		       "%s: loading module anyway...\n",
		       __FUNCTION__, mod->name, ret, __FUNCTION__);
	}

	// mod->state = MODULE_STATE_LIVE;
	trim_init_extable(mod);
	return 0;
}

static int call_final_function(struct module *mod, struct load_info *info,
			       unsigned char *function_name) {
	int ret = 0;
	unsigned int n = 0;
	char *strtab = NULL;
	const char *name = NULL;
	unsigned char *ptr = NULL;
	int (*fn)(void) = NULL;
	Elf(Shdr) *symsect = &info->sechdrs[info->index.sym];

	Elf(Sym) *sym = (void *)symsect->sh_addr;

	strtab = (void *)info->sechdrs[info->index.str].sh_addr;
	n = symsect->sh_size / sizeof(*sym);

	for (unsigned int i = 0; i < n; i++) {
		name = &strtab[sym[i].st_name];

		if (name == NULL || name[0] == '\0'){
			continue;
		}

		if (strcmp((char *)function_name, name) == 0) {
			fn = (int (*)(void))(uintptr_t)sym[i].st_value;
			ptr = (unsigned char *)fn;
			if (ptr[0] == 0xE8) {
				int32_t disp = *(int32_t *)(ptr + 1);
				void   *tgt  = ptr + 5 + disp;
			}

			/* mark the module execution */
			printf("--------------------------\n");
			ret = fn();
			printf("%s returned %d\n", name, ret);

			return ret;
		}
	}

	printf("Function not found in symtab: %s\n", function_name);
	return -ENOENT;
}

int module_init(unsigned char *file_name, unsigned char *function_name, struct load_info *info) {
	int	       fd		= 0;
	long	       err		= 0;
	unsigned int   file_size	= 0;
	bool	       module_allocated = false;
	struct module *mod		= NULL;
	char	      *after_dashes	= NULL;
	unsigned char *mapped_file	= NULL;
	unsigned char *tmp_mapped_file	= NULL;
	struct stat    file_stats	= {};

	if (file_name == NULL || function_name == NULL) {
		return -EINVAL;
	}

	fd = open(file_name, O_RDONLY);

	if(fd < 0){
		printf("[!] file %s not exist!\n", file_name);
		return -EINVAL;
	}

	fstat(fd, &file_stats);
	file_size = file_stats.st_size;

	if(file_size == 0){
		printf("[!] file %s is empty!\n", file_name);
		return -EINVAL;
	}

	tmp_mapped_file = (unsigned char *)mmap(
	    NULL, file_size, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0x00);

	RAND_BASE_ADDR mapped_file_base_random =
	    (RAND_BASE_ADDR)(arc4random() + arc4random());
	RAND_BASE_ADDR mapped_file_base_address =
	    (RAND_BASE_ADDR)(0xffffffff + mapped_file_base_random);

	printf("random base address for the module loading: 0x%lx\n",
	       mapped_file_base_address);

	mapped_file = (unsigned char *)mmap(
	    NULL, file_size, PROT_READ | PROT_WRITE,
	    MAP_ANON | MAP_PRIVATE, -1, 0x00);

	memset(mapped_file, 0x00, file_size);
	memcpy(mapped_file, tmp_mapped_file, file_size);
	munmap(tmp_mapped_file, file_size);

	director.fd = fd;
	err = dwarf_init(director.fd, DW_DLC_READ, NULL, NULL, &director.dbg, &director.err);
	if(err != DW_DLV_OK){
		printf("[!] failed to initialize libdwarf!\n");
		close(director.fd);
		return err;
	}
	parse_module_offset_table();
	close(fd);

	info = (struct load_info *)malloc(sizeof(struct load_info));
	memset(info, 0x00, sizeof(struct load_info));

	info->hdr = (Elf(Ehdr) *)mapped_file;
	info->len = file_size;

	elf_validity_cache_copy(info, 0);

	early_mod_check(info, 0);

	printf("name %s\n", get_modinfo(info, "name"));
	printf("vermagic %s\n", get_modinfo(info, "vermagic"));

	mod = layout_and_allocate(info, 0);

	if (mod == NULL) {
		printf("module struct is null!\n");
		return -1;
	}

	module_allocated = true;
	err		 = percpu_modalloc(mod, info);

	if (err) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = find_module_sections(mod, info);

	if (err) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = simplify_symbols(mod, info);

	if (err < 0) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = apply_relocations(mod, info);

	if (err < 0) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = post_relocation(mod, info);

	if (err < 0) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = complete_formation(mod, info);

	if (err) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = do_init_module(mod);

	if (err) {
		printf("end at line: %d\n", __LINE__);
		return -1;
	}

	err = call_final_function(mod, info, function_name);

	/* at the end, call the 'exit' function for free the memory allocated by the 'init' function */
	err = call_final_function(mod, info, "cleanup_module");

	return err;
}

void display_usage(unsigned char *argv[]) {
	printf("./%s <lkm_name> <function_name> !\n", argv[0]);
}

int initialize_director(struct module_controller *director, struct load_info *info) {
	int ret		     = 0;
	director->libusb_ret = libusb_init(NULL);
	if (director->libusb_ret < 0) {
		ret = -EINVAL;
		goto out;
	}

	director->internal_cb_table = &sym_table;
	director->runtime_generated_structs = &runtime_generated_structs;
	director->info = info;
out:
	return ret;
}

int main(int argc, char *argv[], char *envp[]) {
	int	       ret	     = 0;
	unsigned char *lkm_name	     = NULL;
	unsigned char *function_name = NULL;
	struct load_info load_info = {};

	if (argv[1] == NULL && argv[2] == NULL) {
		display_usage(argv);
		ret = -EINVAL;
		return ret;
	}

	/* assume that if we give only the .ko path, we will call the 'init_module' function */
	if(argv[2] == NULL){
		argv[2] = "init_module";
	}

	if (strlen(argv[1]) < MODULE_INPUT_MIN_LEN ||
	    strlen(argv[2]) < MODULE_INPUT_MIN_LEN) {
		printf("[!] invalid input length!\n");
		ret = -EINVAL;
		return ret;
	}

	/* control if the input finish with '.ko' */
	if (strstr(argv[1], ".ko") == NULL) {
		if (strstr(argv[2], ".ko") != NULL) {
			lkm_name      = argv[2];
			function_name = argv[1];
		} else {
			display_usage(argv);
			ret = -EINVAL;
			return ret;
		}
	} else {
		lkm_name      = argv[1];
		function_name = argv[2];
	}

	ret = initialize_director(&director, &load_info);
	if (ret) {
		return ret;
	}

	ret = module_init(lkm_name, function_name, &load_info);
	return ret;
}
