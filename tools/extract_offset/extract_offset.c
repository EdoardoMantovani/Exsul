/*
 *	Made by Edoardo Mantovani, 2026,
 *	given the translator program, extract and display the content of it's '.offset' struct
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <asm-generic/errno.h>
#include <asm-generic/errno-base.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#include <linux/byteorder/little_endian.h>
#else
#include <linux/byteorder/big_endian.h>
#endif

#define TARGET_SECTION ".offset"

#if UINTPTR_MAX == 0xffffffff
	#define Elf(field)      Elf32_##field
	#define BITS_PER_LONG   32
	#define RAND_BASE_ADDR  unsigned int
#elif UINTPTR_MAX == 0xffffffffffffffff
	#define Elf(field)      Elf64_##field
	#define BITS_PER_LONG   64
	#define RAND_BASE_ADDR  unsigned long
#else
	#error NO UINTPTR???
#endif

struct struct_field {
        unsigned char  field_name[24];
        unsigned short field_offset;
};

struct runtime_struct_layout {
        short struct_size;
        unsigned char *struct_name;
        unsigned short struct_field_num;
	struct struct_field fields[64];
};

struct runtime_struct {
        int compilation_time_generated_structs;
	struct runtime_struct_layout structs[];
};

int main(int argc, char *argv[]){
	int ret = 0;
	int fd = 0;
	int section_num = 0;
	int offset_reg_size = 0;
	struct stat file_stats = {};
	void *offset_region = NULL;
	Elf(Ehdr) *elf_header = NULL;
	Elf(Shdr) *elf_section = NULL;
	unsigned char *file_name = NULL;
	unsigned char *file_mmap = NULL;
	unsigned char *string_table = NULL;
	unsigned char *current_section_name = NULL;
	struct runtime_struct *runtime_structs = NULL;
	struct runtime_struct_layout *runtime_current_layout = NULL;

	file_name = argv[1];
	if(file_name == NULL || strlen(file_name) <= 0){
		ret = -EINVAL;
		return ret;
	}

	fd = open(file_name, O_RDONLY);
	if(fd < 0){
		ret = -EIO;
		return ret;
	}

	fstat(fd, &file_stats);
	if(file_stats.st_size <= 0){
		ret = -EIO;
		close(fd);
		return ret;
	}

	file_mmap = (unsigned char *)mmap(
			NULL,
			file_stats.st_size,
			PROT_READ,
			MAP_SHARED | MAP_FILE,
			fd,
			0);

	if(file_mmap == NULL){
		ret = -ENOMEM;
		close(fd);
		return ret;
	}
	close(fd);

	elf_header = (Elf(Ehdr) *)file_mmap;
	elf_section = (Elf(Shdr) *)(file_mmap + elf_header->e_shoff);
	section_num = elf_header->e_shnum;
	for(int k = 0; k < section_num; k++){
		current_section_name = (unsigned char *)(file_mmap + elf_section[elf_header->e_shstrndx].sh_offset + elf_section[k].sh_name);
		if(strcmp(current_section_name, TARGET_SECTION) == 0){
			offset_region = file_mmap + elf_section[k].sh_offset;
			offset_reg_size = elf_section[k].sh_size;
			printf("[*] target section found... size: %d address: 0x%08x\n", offset_reg_size, offset_region);
			break;
		}
	}

	if(offset_region == NULL || offset_reg_size == 0){
		printf("[!] " TARGET_SECTION " not found! aborting now...\n");
		goto end;
	}
	/* skip the first element from the parsing */
	offset_region   += sizeof(int);
	offset_reg_size -= 4;
	runtime_current_layout = offset_region;

	end:
	memset(&file_stats, 0, sizeof(struct stat));
	munmap(file_mmap, file_stats.st_size);
	return ret;
}

