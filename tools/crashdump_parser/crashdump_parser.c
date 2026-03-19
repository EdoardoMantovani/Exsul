/*
 *	Made by Edoardo Mantovani, 2026
 *	Simple crashdump parser
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include <asm-generic/errno.h>
#include <asm-generic/errno-base.h>

#define DUMP_MAGIC              0xDCCCCB
struct crashdump_section {
	int start_offset;
	int length;
};

/* don't report also the symbol length */
struct reduced_internal_cb_table {
	unsigned char *function_name;
	void *function_ptr;
};

int main(int argc, char *argv[]){
	int ret = 0;
	int fd = 0;
	int offset_counter = 0;
	struct stat dump_stat = {};
	unsigned char *dump_name = NULL;
	unsigned char *dump_mmap = NULL;
	struct crashdump_section *section = NULL;
	struct reduced_internal_cb_table *current_sym = NULL;

	dump_name = argv[1];

	if(dump_name == NULL){
		ret = -EINVAL;
		return ret;
	}

	fd = open(dump_name, O_RDONLY);
	if(fd < 0){
		printf("[!] '%s' not found!\n", dump_name);
		ret = -EINVAL;
		return ret;
	}
	fstat(fd, &dump_stat);
	if(dump_stat.st_size <= 0){
		printf("[!] '%s' size is invalid!\n", dump_name);
		ret = -EINVAL;
		goto end;
	}

	dump_mmap = (unsigned char *)mmap(
		NULL,
		dump_stat.st_size,
		PROT_READ,
		MAP_FILE | MAP_SHARED,
		fd,
		0
	);

	if(dump_mmap == NULL){
		printf("[!] failed to mmap '%s'\n", dump_name);
		ret = -ENOMEM;
		goto end;
	}

	dump_mmap += sizeof(DUMP_MAGIC);
	offset_counter += sizeof(DUMP_MAGIC);
	printf("sections in the dump file: %d\n", (uint8_t)dump_mmap);
	dump_mmap += sizeof(uint8_t);
	offset_counter += sizeof(uint8_t);
	section = (struct crashdump_section *)dump_mmap;
	printf("section start offset: %d section length: %d\n", section->start_offset, section->length);
	/* return at the base offset + the section start offset */
	offset_counter -= section->start_offset;
	dump_mmap += offset_counter;
	current_sym = (struct reduced_internal_cb_table *)dump_mmap;
	printf("%s\n", current_sym->function_name);

//	for(int k = 0; current_sym[k].function_name != NULL; k++){
//		printf("%s\n", current_sym[k].function_name);
//	}

	end:
	if(fd > 0){
		close(fd);
	}
	if(dump_mmap != NULL){
		munmap(dump_mmap, dump_stat.st_size);
	}
	return ret;
}
