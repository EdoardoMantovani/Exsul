#ifdef __x86_64__

#define EM_CURRENT_ARCH				EM_X86_64
#define TRAMP_REGION_SIZE			(64 * 1024)

#define REPEAT_BYTE(x)                          ((~0ul / 0xff) * (x))
#define WORD_AT_A_TIME_CONSTANTS                { REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) }

struct exception_table_entry {
	int insn;
	int fixup;
	int type;
};

/*
 * The patching flags are part of the upper bits of the @ft_flags parameter when
 * specifying them. The split is currently like this:
 *
 * [31... flags ...16][15... CPUID feature bit ...0]
 *
 * but since this is all hidden in the macros argument being split, those fields can be
 * extended in the future to fit in a u64 or however the need arises.
 */
struct alt_instr {
	s32 instr_offset;	/* original instruction */
	s32 repl_offset;	/* offset to replacement instruction */

	union {
		struct {
			u32 cpuid: 16;	/* CPUID bit set for replacement */
			u32 flags: 16;	/* patching control flags */
		};
		u32 ft_flags;
	};

	u8  instrlen;		/* length of original instruction */
	u8  replacementlen;	/* length of new instruction */
} __packed;

struct callthunk_sites {
	s32                       *call_start;
	s32                       *call_end;
	struct alt_instr          *alt_start;
	struct alt_instr          *alt_end;
};

#if 0
struct exception_table_entry {
	int insn;
	int fixup;
	int data;
};
#endif

bool constant_test_bit(int nr, const void *addr){
	const u32 *p = addr;
	return ((1UL << (nr & 31)) & (p[nr >> 5])) != 0;
}

#ifndef ARCH_INDEPENDENT_BITOPS
bool variable_test_bit(int nr, const void *addr){
	bool v = false;
	const u32 *p = addr;

	asm("btl %2,%1" : "=@ccc" (v) : "m" (*p), "Ir" (nr));
	return v;
}

#define test_bit(nr,addr) (__builtin_constant_p(nr) ? constant_test_bit((nr),(addr)) : variable_test_bit((nr),(addr)))

void set_bit(int nr, void *addr){
	asm("btsl %1,%0" : "+m" (*(u32 *)addr) : "Ir" (nr));
}
#endif

int module_frob_arch_sections(Elf(Ehdr) *hdr, Elf(Shdr) *sechdrs, char *secstrings, struct module *mod){
	return 0;
}

void alternatives_smp_module_add(struct module *mod, char *name, void *locks, void *locks_end, void *text, void *text_end) {
	/* do nothing here */
}

unsigned long zero_bytemask(unsigned long bits){
	return bits;
}

void delay_loop(u64 __loops){
	unsigned long loops = (unsigned long)__loops;

	__asm__ __volatile__(
		"	test %0,%0	\n"
		"	jz 3f		\n"
		"	jmp 1f		\n"

		".align 16		\n"
		"1:	jmp 2f		\n"

		".align 16		\n"
		"2:	dec %0		\n"
		"	jnz 2b		\n"
		"3:	dec %0		\n"

		: "+a" (loops)
		:
	);
}

void __delay(unsigned long loops){
	delay_loop(loops);
}

void arch__const_udelay(unsigned long xloops){
	int d0;

	xloops *= 4;
	__asm__("mull %%edx"
		: "=d" (xloops), "=&a" (d0)
		: "1" (xloops), "0"
		(1<<12 * (HZ/4)));

	__delay(++xloops);
}

void *x86_64_make_trampoline(void *target_stub){
	unsigned char *current_stub = NULL;

	if(director.trampoline_area == NULL){
		director.trampoline_area = mmap(NULL,
			TRAMP_REGION_SIZE,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANON | MAP_32BIT,
			-1,
			0);
	}

	if (director.trampoline_area == MAP_FAILED) {
		director.trampoline_area = NULL;
		return NULL;
	}

	if (director.trampoline_used + 16 > TRAMP_REGION_SIZE){
		return NULL;
	}

	current_stub = director.trampoline_area + director.trampoline_used;
	director.trampoline_used += 16;

	/* movabs rax, imm64 ; jmp rax */
	current_stub[0]  = 0x48;
	current_stub[1]  = 0xB8;
	*(uint64_t *)(current_stub + 2) = (uint64_t)(uintptr_t)target_stub;
	current_stub[10] = 0xFF;
	current_stub[11] = 0xE0;
	current_stub[12] = 0x90;
	current_stub[13] = 0x90;
	current_stub[14] = 0x90;
	current_stub[15] = 0x90;

	__builtin___clear_cache((char *)current_stub, (char *)current_stub + 16);
	return current_stub;
}

int module_finalize(const Elf(Ehdr) *hdr, const Elf(Shdr) *sechdrs, struct module *mod){
	unsigned int rsize      = 0;
	unsigned int csize      = 0;
        const Elf(Shdr) *s              = NULL;
        const Elf(Shdr) *alt            = NULL;
        const Elf(Shdr) *locks          = NULL;
        const Elf(Shdr) *orc            = NULL;
        const Elf(Shdr) *orc_ip         = NULL;
        const Elf(Shdr) *retpolines     = NULL;
        const Elf(Shdr) *returns        = NULL;
        const Elf(Shdr) *ibt_endbr      = NULL;
        const Elf(Shdr) *calls          = NULL;
        const Elf(Shdr) *cfi            = NULL;
	void *rseg              = NULL;
	void *cseg              = NULL;
	char *secstrings  = (void *)hdr + sechdrs[hdr->e_shstrndx].sh_offset;
	struct callthunk_sites cs = {};

        for (s = sechdrs; s < sechdrs + hdr->e_shnum; s++) {
                if (!strcmp(".altinstructions", secstrings + s->sh_name)){
                        printf("%s\n", ".altinstructions");
                        alt = s;
                }

                if (!strcmp(".smp_locks", secstrings + s->sh_name)){
                        printf("%s\n", ".smp_locks");
                        locks = s;
                }

                if (!strcmp(".orc_unwind", secstrings + s->sh_name)){
                        printf("%s\n", ".orc_unwind");
                        orc = s;
                }

                if (!strcmp(".orc_unwind_ip", secstrings + s->sh_name)){
                        printf("%s\n", ".orc_unwind_ip");
                        orc_ip = s;
                }

                if (!strcmp(".retpoline_sites", secstrings + s->sh_name)){
                        printf("%s\n", ".retpoline_sites");
                        retpolines = s;
                }

                if (!strcmp(".return_sites", secstrings + s->sh_name)){
                        printf("%s\n", ".return_sites");
                        returns = s;
                }

                if (!strcmp(".call_sites", secstrings + s->sh_name)){
                        printf("%s\n", ".call_sites");
                        calls = s;
                }
		if (!strcmp(".cfi_sites", secstrings + s->sh_name)){
                        printf("%s\n", ".cfi_sites");
                        cfi = s;
                }

                if (!strcmp(".ibt_endbr_seal", secstrings + s->sh_name)){
                        printf("%s\n", ".ibt_endbr_seal");
                        ibt_endbr = s;
                }
        }

        if (retpolines || cfi) {
                if (retpolines) {
                        rseg            = (void *)retpolines->sh_addr;
                        rsize           = retpolines->sh_size;
                }

                if (cfi) {
                        cseg            = (void *)cfi->sh_addr;
                        csize           = cfi->sh_size;
                }
                /**

                  Build the kernel with support for Indirect Branch Tracking, a
                  hardware support course-grain forward-edge Control Flow Integrity
                  protection. It enforces that all indirect calls must land on
                  an ENDBR instruction, as such, the compiler will instrument the
                  code with them to make this happen.

                  In addition to building the kernel with IBT, seal all functions that
                  are not indirect call targets, avoiding them ever becoming one.

                  This requires LTO like objtool runs and will slow down the build. It
                  does significantly reduce the number of ENDBR instructions in the
                  kernel image.

                **/

                //apply_fineibt(rseg, rseg + rsize, cseg, cseg + csize);
        }
        if (retpolines) {
		rseg              = (void *)retpolines->sh_addr;

                /**

                Compile kernel with the retpoline compiler options to guard against
                kernel-to-user data leaks by avoiding speculative indirect
                branches. Requires a compiler with -mindirect-branch=thunk-extern
                support for full protection. The kernel may run slower.

                **/

                //apply_retpolines(rseg, rseg + retpolines->sh_size);
        }
        if (returns) {
                void *rseg              = (void *)returns->sh_addr;

                /**

                Compile kernel with the retpoline compiler options to guard against
                kernel-to-user data leaks by avoiding speculative indirect
                branches. Requires a compiler with -mindirect-branch=thunk-extern
                support for full protection. The kernel may run slower.

                **/

                //apply_returns(rseg, rseg + returns->sh_size);
        }
        if (alt) {
                void *aseg              = (void *)alt->sh_addr;

                /**

                Replace instructions with better alternatives for this CPU type. This runs
                before SMP is initialized to avoid SMP problems with self modifying code.
                This implies that asymmetric systems where APs have less capabilities than
                the boot processor are not handled. Tough. Make sure you disable such
                features by hand.

                Marked "noinline" to cause control flow change and thus insn cache
                to refetch changed I$ lines.

                **/

                //apply_alternatives(aseg, aseg + alt->sh_size);
        }
        if (calls || alt) {
                if (calls) {
                        cs.call_start   = (void *)calls->sh_addr;
                        cs.call_end     = (void *)calls->sh_addr + calls->sh_size;
                }

                if (alt) {
                        cs.alt_start    = (void *)alt->sh_addr;
                        cs.alt_end      = (void *)alt->sh_addr + alt->sh_size;
                }

                /**

                Defined for CONFIG_MODULES, so this is necessary to implement.

                **/

                //callthunks_patch_module_calls(&cs, me);
        }
        if (ibt_endbr) {
                void *iseg              = (void *)ibt_endbr->sh_addr;

                /**

                Build the kernel with support for Indirect Branch Tracking, a
                hardware support course-grain forward-edge Control Flow Integrity
                protection. It enforces that all indirect calls must land on
                an ENDBR instruction, as such, the compiler will instrument the
                code with them to make this happen.

                In addition to building the kernel with IBT, seal all functions that
                are not indirect call targets, avoiding them ever becoming one.

                This requires LTO like objtool runs and will slow down the build. It
                does significantly reduce the number of ENDBR instructions in the
                kernel image.

                **/

                //apply_seal_endbr(iseg, iseg + ibt_endbr->sh_size);
        }
        if (locks) {
                void *lseg              = (void *)locks->sh_addr;
                void *text              = mod->mem[MOD_TEXT].base;
                void *text_end          = text + mod->mem[MOD_TEXT].size;

                /**

                This enables support for systems with more than one CPU. If you have
                a system with only one CPU, say N. If you have a system with more
                than one CPU, say Y.

                If you say N here, the kernel will run on uni- and multiprocessor
                machines, but will use only one CPU of a multiprocessor machine. If
                you say Y here, the kernel will run on many, but not all,
                uniprocessor machines. On a uniprocessor machine, the kernel
                will run faster if you say N here.

                Note that if you say Y here and choose architecture "586" or
                "Pentium" under "Processor family", the kernel will not work on 486
                architectures. Similarly, multiprocessor kernels for the "PPro"
                architecture may not work on all Pentium based boards.

                People using multiprocessor machines who say Y here should also say
                Y to "Enhanced Real Time Clock Support", below. The "Advanced Power
                Management" code will be disabled if you say Y here.

                **/

                alternatives_smp_module_add(mod, mod->name, lseg, lseg + locks->sh_size, text, text_end);
        }

        if (orc && orc_ip){
                /**

                This option enables the ORC (Oops Rewind Capability) unwinder for
                unwinding kernel stack traces.  It uses a custom data format which is
                a simplified version of the DWARF Call Frame Information standard.

                This unwinder is more accurate across interrupt entry frames than the
                frame pointer unwinder.  It also enables a 5-10% performance
                improvement across the entire kernel compared to frame pointers.

                Enabling this option will increase the kernel's runtime memory usage
                by roughly 2-4MB, depending on your kernel config.

                **/

                //unwind_module_init(me, (void *)orc_ip->sh_addr, orc_ip->sh_size, (void *)orc->sh_addr, orc->sh_size);
        }

        return 0;
}

int __write_relocate_add(Elf(Shdr) *sechdrs, const char *strtab, unsigned int symindex, unsigned int relsec,  struct module *mod, void *(*write)(void *dest, const void *src, size_t len), bool apply){
        unsigned int i                                  = 0;
        Elf(Rela) *rel                                  = (void *)sechdrs[relsec].sh_addr;
        Elf(Sym) *sym                                   = NULL;
        void *loc                                       = NULL;
        u64 val                                         = 0ULL;
        u64 zero                                        = 0ULL;
	size_t size = 0;
        for (i = 0; i < sechdrs[relsec].sh_size / sizeof(*rel); i++) {
                size_t size                             = 0;

                loc                                     = (void *)sechdrs[sechdrs[relsec].sh_info].sh_addr + rel[i].r_offset;
                sym                                     = (Elf(Sym) *)sechdrs[symindex].sh_addr + ELF64_R_SYM(rel[i].r_info);
                val                                     = sym->st_value + rel[i].r_addend;

                switch (ELF64_R_TYPE(rel[i].r_info)) {
                        case R_X86_64_NONE:
                        continue;
                        case R_X86_64_64:
                                size = 8;
                        break;
                        case R_X86_64_32:
                                if (val != *(uint32_t *)&val){
                                        goto overflow;
                                }
                                size = 4;
                        break;
                        case R_X86_64_32S:
                                if ((int64_t)val != *(int32_t *)&val){
                                        //goto overflow;
                                }
                                size = 4;
                        break;
                        case R_X86_64_PC32:
                        case R_X86_64_PLT32:
                                val -= (uint64_t)loc;
                                size = 4;
                        break;
                        case R_X86_64_PC64:
                                val -= (uint64_t)loc;
                                size = 8;
                        break;
                        default:
                                printf("%s: Unknown rela relocation: %lu\n", mod->name, ELF64_R_TYPE(rel[i].r_info));
                        return -ENOEXEC;
                }

                if (apply) {
                        if (memcmp(loc, &zero, size)) {
                                printf("x86/modules: Invalid relocation target, existing value is nonzero for type %d, loc %p, val %lx \n", (int)ELF64_R_TYPE(rel[i].r_info), loc, val);
                                return -ENOEXEC;
                        }
                        if( strcmp((unsigned char *)(strtab + sym->st_name), "cleanup_module") != 0 ){
                                write(loc, &val, size);
                        }
                } else {
                        if (memcmp(loc, &val, size)) {
                                printf("x86/modules: Invalid relocation target, existing value does not match expected value for type %d, loc %p, val %lx\n", (int)ELF64_R_TYPE(rel[i].r_info), loc, val);
                                return -ENOEXEC;
                        }
                        write(loc, &zero, size);
                }
        }

        return 0;

overflow:
        printf("overflow in relocation type %d val %lx name %s\n", (int)ELF64_R_TYPE(rel[i].r_info), val, (unsigned char *)(strtab + sym->st_name));
        printf("`%s' likely not compiled with -mcmodel=kernel\n", mod->name);
        return -ENOEXEC;
}

int write_relocate_add(Elf(Shdr) *sechdrs, const char *strtab, unsigned int symindex, unsigned int relsec, struct module *mod, bool apply){
        signed int ret                                  = 0;
        bool early                                      = mod->state == MODULE_STATE_UNFORMED;
        void *(*write)(void *, const void *, size_t)    = memcpy;

        if (!early) {
                //write                                         = text_poke;
        }

        ret                                             = __write_relocate_add(sechdrs, strtab, symindex, relsec, mod,  write, apply);

        return ret;
}

int apply_relocate_add(Elf(Shdr) *sechdrs, const char *strtab, unsigned int symindex, unsigned int relsec, struct module *mod){
	return write_relocate_add(sechdrs, strtab, symindex, relsec, mod, true);
}

/** for 32 bit only **/
int apply_relocate(Elf(Shdr) *sechdrs, const char *strtab, unsigned int symindex, unsigned int relsec, struct module *mod){
        Elf(Rel) *rel           = (void *)sechdrs[relsec].sh_addr;
        Elf(Sym) *sym           = NULL;
        uint32_t *location      = NULL;

        printf("Applying relocate section %u to %u\n", relsec, sechdrs[relsec].sh_info);

        for (unsigned int i = 0; i < sechdrs[relsec].sh_size / sizeof(*rel); i++) {
                location = (void *)sechdrs[sechdrs[relsec].sh_info].sh_addr + rel[i].r_offset;
                sym = (Elf(Sym) *)sechdrs[symindex].sh_addr + ELF32_R_SYM(rel[i].r_info);

                switch (ELF32_R_TYPE(rel[i].r_info)) {
                case R_386_32:
                        /* We add the value into the location given */
                        *location += sym->st_value;
                        break;
                case R_386_PC32:
                case R_386_PLT32:
                        /* Add the value, subtract its position */
                        *location += sym->st_value - (uint32_t)location;
                        break;
                default:
                        printf("%s: Unknown relocation: %lu\n", mod->name, ELF32_R_TYPE(rel[i].r_info));
                        return -ENOEXEC;
                }
        }
        return 0;
}

#endif
