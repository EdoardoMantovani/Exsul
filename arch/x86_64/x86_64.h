#ifdef __x86_64__

#include "inat.h"

#define MAX_UNWIND_ITERATIONS			16
#define EM_CURRENT_ARCH				EM_X86_64
#define TRAMP_REGION_SIZE			(64 * 1024)

#define REPEAT_BYTE(x)                          ((~0ul / 0xff) * (x))
#define WORD_AT_A_TIME_CONSTANTS                { REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) }

#define CALL_RIP_REL_OPCODE			0xff
#define CALL_RIP_REL_MODRM			0x15

#define ASM_NOP_MAX				11

#define INT3_INSN_SIZE				1
#define INT3_INSN_OPCODE			0xCC

#define RET_INSN_SIZE				1
#define RET_INSN_OPCODE				0xC3

#define CALL_INSN_SIZE				5
#define CALL_INSN_OPCODE			0xE8

#define JMP32_INSN_SIZE				5
#define JMP32_INSN_OPCODE			0xE9

#define JMP8_INSN_SIZE				2
#define JMP8_INSN_OPCODE			0xEB

#define TEXT_POKE_MAX_OPCODE_SIZE		5
#define MAX_PATCH_LEN				(255-1)

#define __ASM_FORM(x, ...)			" " __stringify(x,##__VA_ARGS__) " "
#define __ASM_FORM_RAW(x, ...)			    __stringify(x,##__VA_ARGS__)
#define __ASM_FORM_COMMA(x, ...)		" " __stringify(x,##__VA_ARGS__) ","
#define __ASM_REGPFX				%%

#define _ASM_BYTES(x, ...)			__ASM_FORM(.byte x,##__VA_ARGS__ ;)

/*
 * Generic 64bit nops from GAS:
 *
 * 1: nop
 * 2: osp nop
 * 3: nopl (%eax)
 * 4: nopl 0x00(%eax)
 * 5: nopl 0x00(%eax,%eax,1)
 * 6: osp nopl 0x00(%eax,%eax,1)
 * 7: nopl 0x00000000(%eax)
 * 8: nopl 0x00000000(%eax,%eax,1)
 * 9: cs nopl 0x00000000(%eax,%eax,1)
 * 10: osp cs nopl 0x00000000(%eax,%eax,1)
 * 11: osp osp cs nopl 0x00000000(%eax,%eax,1)
 */
#define BYTES_NOP1				0x90
#define BYTES_NOP2				0x66,BYTES_NOP1
#define BYTES_NOP3				0x0f,0x1f,0x00
#define BYTES_NOP4				0x0f,0x1f,0x40,0x00
#define BYTES_NOP5				0x0f,0x1f,0x44,0x00,0x00
#define BYTES_NOP6				0x66,BYTES_NOP5
#define BYTES_NOP7				0x0f,0x1f,0x80,0x00,0x00,0x00,0x00
#define BYTES_NOP8				0x0f,0x1f,0x84,0x00,0x00,0x00,0x00,0x00
#define BYTES_NOP9				0x2e,BYTES_NOP8
#define BYTES_NOP10				0x66,BYTES_NOP9
#define BYTES_NOP11				0x66,BYTES_NOP10

#define ASM_NOP1				_ASM_BYTES(BYTES_NOP1)
#define ASM_NOP2				_ASM_BYTES(BYTES_NOP2)
#define ASM_NOP3				_ASM_BYTES(BYTES_NOP3)
#define ASM_NOP4				_ASM_BYTES(BYTES_NOP4)
#define ASM_NOP5				_ASM_BYTES(BYTES_NOP5)
#define ASM_NOP6				_ASM_BYTES(BYTES_NOP6)
#define ASM_NOP7				_ASM_BYTES(BYTES_NOP7)
#define ASM_NOP8				_ASM_BYTES(BYTES_NOP8)

#define __XEN_EMULATE_PREFIX			0x0f,0x0b,0x78,0x65,0x6e  /* ud2 ; .ascii "xen" */
#define __KVM_EMULATE_PREFIX			0x0f,0x0b,0x6b,0x76,0x6d	/* ud2 ; .ascii "kvm" */

#ifndef OPTIMIZER_HIDE_VAR
/* Make the optimizer believe the variable can be manipulated arbitrarily. */
#define OPTIMIZER_HIDE_VAR(var)			__asm__ ("" : "=r" (var) : "0" (var))
#endif

#define __stringify_1(x...)			#x
#define __stringify(x...)			__stringify_1(x)

#define INCREMENT_CALL_DEPTH

#ifdef __linux__
__asm__(
	".pushsection .rodata                           \n"
	".global skl_call_thunk_template                \n"
	"skl_call_thunk_template:                       \n"
	INCREMENT_CALL_DEPTH
	".global skl_call_thunk_tail                    \n"
	"skl_call_thunk_tail:                           \n"
	".popsection                                    \n"
);
#else
__asm__(".section .rdata,\"dr\"				\n"
        ".global skl_call_thunk_template                \n"
        "skl_call_thunk_template:                       \n"
        INCREMENT_CALL_DEPTH
        ".global skl_call_thunk_tail                    \n"
        "skl_call_thunk_tail:                           \n"
        ".section .text					\n"
);
#endif

extern u8 skl_call_thunk_template[];
extern u8 skl_call_thunk_tail[];

#ifdef __linux__
/*
 * <fineibt_caller_start>:
 *  0:   b8 78 56 34 12          mov    $0x12345678, %eax
 *  5:   4d 8d 5b f0             lea    -0x10(%r11), %r11
 *  9:   0f 1f 44 00 00          nopl   0x0(%rax,%rax,1)
 */
__asm__(".pushsection .rodata			\n"
	"fineibt_caller_start:			\n"
	"	movl	$0x12345678, %eax	\n"
	"	lea	-0x10(%r11), %r11	\n"
	ASM_NOP5
	"fineibt_caller_end:			\n"
	".popsection				\n"
);
#else
__asm__(".section .rdata,\"dr\"              \n"
	"fineibt_caller_start:               \n"
	"    movl    $0x12345678, %eax       \n"
	"    lea     -0x10(%r11), %r11       \n"
	ASM_NOP5
	"fineibt_caller_end:                 \n"
	".section .text                      \n"
);
#endif
extern u8 fineibt_caller_start[];
extern u8 fineibt_caller_end[];

#define fineibt_caller_size				(fineibt_caller_end - fineibt_caller_start)
#define fineibt_caller_hash				1

#define fineibt_caller_jmp				(fineibt_caller_size - 2)

#define MAX_INSN_SIZE					15
#define SKL_TMPL_SIZE					((unsigned int)(skl_call_thunk_tail - skl_call_thunk_template))
#define insn_decode_kernel(_insn, _ptr)			insn_decode((_insn), (_ptr), MAX_INSN_SIZE, INSN_MODE_KERN)

/**
 * for_each_insn_prefix() -- Iterate prefixes in the instruction
 * @insn: Pointer to struct insn.
 * @prefix: Prefix byte.
 *
 * Iterate prefix bytes of given @insn. Each prefix byte is stored in @prefix
 * and the index is stored in @idx (note that this @idx is just for a cursor,
 * do not change it.)
 * Since prefixes.nbytes can be bigger than 4 if some prefixes
 * are repeated, it cannot be used for looping over the prefixes.
 */
#define for_each_insn_prefix(insn, prefix)	\
	for (int idx = 0; idx < ARRAY_SIZE(insn->prefixes.bytes) && (prefix = insn->prefixes.bytes[idx]) != 0; idx++)

#define leXX_to_cpu(t, r)						\
({									\
	__typeof__(t) v;						\
	switch (sizeof(t)) {						\
	case 4: v = le32_to_cpu(r); break;				\
	case 2: v = le16_to_cpu(r); break;				\
	case 1:	v = r; break;						\
	default:							\
		break;							\
	}								\
	v;								\
})

/* Verify next sizeof(t) bytes can be on the same instruction */
#define validate_next(t, insn, n)	\
	((insn)->next_byte + sizeof(t) + n <= (insn)->end_kaddr)

#define __get_next(t, insn)	\
	({ t r = get_unaligned((t *)(insn)->next_byte); (insn)->next_byte += sizeof(t); leXX_to_cpu(t, r); })

#define __peek_nbyte_next(t, insn, n)	\
	({ t r = get_unaligned((t *)(insn)->next_byte + n); leXX_to_cpu(t, r); })

#define get_next(t, insn)	\
	({ if (unlikely(!validate_next(t, insn, 0))) goto err_out; __get_next(t, insn); })

#define peek_nbyte_next(t, insn, n)	\
	({ if (unlikely(!validate_next(t, insn, n))) goto err_out; __peek_nbyte_next(t, insn, n); })

#define peek_next(t, insn)	peek_nbyte_next(t, insn, 0)

#define X86_MODRM_MOD(modrm) (((modrm) & 0xc0) >> 6)
#define X86_MODRM_REG(modrm) (((modrm) & 0x38) >> 3)
#define X86_MODRM_RM(modrm) ((modrm) & 0x07)

#define X86_SIB_SCALE(sib) (((sib) & 0xc0) >> 6)
#define X86_SIB_INDEX(sib) (((sib) & 0x38) >> 3)
#define X86_SIB_BASE(sib) ((sib) & 0x07)

#define X86_REX2_M(rex) ((rex) & 0x80)	/* REX2 M0 */
#define X86_REX2_R(rex) ((rex) & 0x40)	/* REX2 R4 */
#define X86_REX2_X(rex) ((rex) & 0x20)	/* REX2 X4 */
#define X86_REX2_B(rex) ((rex) & 0x10)	/* REX2 B4 */

#define X86_REX_W(rex) ((rex) & 8)	/* REX or REX2 W */
#define X86_REX_R(rex) ((rex) & 4)	/* REX or REX2 R3 */
#define X86_REX_X(rex) ((rex) & 2)	/* REX or REX2 X3 */
#define X86_REX_B(rex) ((rex) & 1)	/* REX or REX2 B3 */

/* VEX bit flags  */
#define X86_VEX_W(vex)	((vex) & 0x80)	/* VEX3 Byte2 */
#define X86_VEX_R(vex)	((vex) & 0x80)	/* VEX2/3 Byte1 */
#define X86_VEX_X(vex)	((vex) & 0x40)	/* VEX3 Byte1 */
#define X86_VEX_B(vex)	((vex) & 0x20)	/* VEX3 Byte1 */
#define X86_VEX_L(vex)	((vex) & 0x04)	/* VEX3 Byte2, VEX2 Byte1 */
/* VEX bit fields */
#define X86_EVEX_M(vex)	((vex) & 0x07)		/* EVEX Byte1 */
#define X86_VEX3_M(vex)	((vex) & 0x1f)		/* VEX3 Byte1 */
#define X86_VEX2_M	1			/* VEX2.M always 1 */
#define X86_VEX_V(vex)	(((vex) & 0x78) >> 3)	/* VEX3 Byte2, VEX2 Byte1 */
#define X86_VEX_P(vex)	((vex) & 0x03)		/* VEX3 Byte2, VEX2 Byte1 */
#define X86_VEX_M_MAX	0x1f			/* VEX3.M Maximum value */
/* XOP bit fields */
#define X86_XOP_R(xop)	((xop) & 0x80)	/* XOP Byte2 */
#define X86_XOP_X(xop)	((xop) & 0x40)	/* XOP Byte2 */
#define X86_XOP_B(xop)	((xop) & 0x20)	/* XOP Byte2 */
#define X86_XOP_M(xop)	((xop) & 0x1f)	/* XOP Byte2 */
#define X86_XOP_W(xop)	((xop) & 0x80)	/* XOP Byte3 */
#define X86_XOP_V(xop)	((xop) & 0x78)	/* XOP Byte3 */
#define X86_XOP_L(xop)	((xop) & 0x04)	/* XOP Byte3 */
#define X86_XOP_P(xop)	((xop) & 0x03)	/* XOP Byte3 */
#define X86_XOP_M_MIN	0x08	/* Min of XOP.M */
#define X86_XOP_M_MAX	0x1f	/* Max of XOP.M */

/*
 * In this context, "source" is where the instructions are placed in the
 * section .altinstr_replacement, for example during kernel build by the
 * toolchain.
 * "Destination" is where the instructions are being patched in by this
 * machinery.
 *
 * The source offset is:
 *
 *   src_imm = target - src_next_ip                  (1)
 *
 * and the target offset is:
 *
 *   dst_imm = target - dst_next_ip                  (2)
 *
 * so rework (1) as an expression for target like:
 *
 *   target = src_imm + src_next_ip                  (1a)
 *
 * and substitute in (2) to get:
 *
 *   dst_imm = (src_imm + src_next_ip) - dst_next_ip (3)
 *
 * Now, since the instruction stream is 'identical' at src and dst (it
 * is being copied after all) it can be stated that:
 *
 *   src_next_ip = src + ip_offset
 *   dst_next_ip = dst + ip_offset                   (4)
 *
 * Substitute (4) in (3) and observe ip_offset being cancelled out to
 * obtain:
 *
 *   dst_imm = src_imm + (src + ip_offset) - (dst + ip_offset)
 *           = src_imm + src - dst + ip_offset - ip_offset
 *           = src_imm + src - dst                   (5)
 *
 * IOW, only the relative displacement of the code block matters.
 */

#define apply_reloc_n(n_, p_, d_)				\
	do {							\
		s32 v = *(s##n_ *)(p_);				\
		v += (d_);					\
		*(s##n_ *)(p_) = (s##n_)v;			\
	} while (0)

#define DUMP_BYTES(type, buf, len, fmt, args...)			\
do {									\
	if (true) {							\
		int j;							\
									\
		if (len == 0){						\
			break;						\
		}							\
		printf(pr_fmt(fmt), ##args);				\
		for (j = 0; j < (len) - 1; j++)				\
			printf("%02hhx ", buf[j]);			\
		printf("%02hhx\n", buf[j]);				\
	}								\
} while (0)

struct exception_table_entry {
	int insn;
	int fixup;
	int type;
};

struct core_text {
	unsigned long	base;
	unsigned long	end;
	const char	*name;
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

static inline u8 * instr_va(struct alt_instr *i){
	return (u8 *)&i->instr_offset + i->instr_offset;
}

#if __BYTE_ORDER == __LITTLE_ENDIAN
struct insn_field {
	union {
		insn_value_t value;
		insn_byte_t bytes[4];
	};
	/* !0 if we've run insn_get_xxx() for this field */
	unsigned char got;
	unsigned char nbytes;
};

static inline void insn_field_set(struct insn_field *p, insn_value_t v, unsigned char n){
	p->value = v;
	p->nbytes = n;
}

static inline void insn_set_byte(struct insn_field *p, unsigned char n, insn_byte_t v){
	p->bytes[n] = v;
}

#else
struct insn_field {
	insn_value_t value;
	union {
		insn_value_t little;
		insn_byte_t bytes[4];
	};
	/* !0 if we've run insn_get_xxx() for this field */
	unsigned char got;
	unsigned char nbytes;
};

static inline void insn_field_set(struct insn_field *p, insn_value_t v, unsigned char n){
	p->value = v;
	p->little = __cpu_to_le32(v);
	p->nbytes = n;
}

static inline void insn_set_byte(struct insn_field *p, unsigned char n, insn_byte_t v){
	p->bytes[n] = v;
	p->value = __le32_to_cpu(p->little);
}
#endif

struct insn {
	struct insn_field prefixes;	/*
					 * Prefixes
					 * prefixes.bytes[3]: last prefix
					 */
	struct insn_field rex_prefix;	/* REX prefix */
	union {
		struct insn_field vex_prefix;	/* VEX prefix */
		struct insn_field xop_prefix;	/* XOP prefix */
	};
	struct insn_field opcode;	/*
					 * opcode.bytes[0]: opcode1
					 * opcode.bytes[1]: opcode2
					 * opcode.bytes[2]: opcode3
					 */
	struct insn_field modrm;
	struct insn_field sib;
	struct insn_field displacement;
	union {
		struct insn_field immediate;
		struct insn_field moffset1;	/* for 64bit MOV */
		struct insn_field immediate1;	/* for 64bit imm or off16/32 */
	};
	union {
		struct insn_field moffset2;	/* for 64bit MOV */
		struct insn_field immediate2;	/* for 64bit imm or seg16 */
	};

	int	emulate_prefix_size;
	insn_attr_t attr;
	unsigned char opnd_bytes;
	unsigned char addr_bytes;
	unsigned char length;
	unsigned char x86_64;

	const insn_byte_t *kaddr;	/* kernel address of insn to analyze */
	const insn_byte_t *end_kaddr;	/* kernel address of last insn in buffer */
	const insn_byte_t *next_byte;
};

union text_poke_insn {
	u8 text[TEXT_POKE_MAX_OPCODE_SIZE];
	struct {
		u8 opcode;
		s32 disp;
	} __packed;
};

static const u8 nops[] = {
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
};

static const insn_byte_t xen_prefix[] = { __XEN_EMULATE_PREFIX };
static const insn_byte_t kvm_prefix[] = { __KVM_EMULATE_PREFIX };

static const unsigned char x86nops[] = {
	BYTES_NOP1,
	BYTES_NOP2,
	BYTES_NOP3,
	BYTES_NOP4,
	BYTES_NOP5,
	BYTES_NOP6,
	BYTES_NOP7,
	BYTES_NOP8,
	BYTES_NOP9,
	BYTES_NOP10,
	BYTES_NOP11,
};

const unsigned char * const x86_nops[ASM_NOP_MAX+1] ={
	NULL,
	x86nops,
	x86nops + 1,
	x86nops + 1 + 2,
	x86nops + 1 + 2 + 3,
	x86nops + 1 + 2 + 3 + 4,
	x86nops + 1 + 2 + 3 + 4 + 5,
	x86nops + 1 + 2 + 3 + 4 + 5 + 6,
	x86nops + 1 + 2 + 3 + 4 + 5 + 6 + 7,
	x86nops + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8,
	x86nops + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9,
	x86nops + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10,
};

#include "inat-tables.c"

const void (*x86_return_thunk)(void) = __x86_return_thunk;

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

void *x86_64_make_trampoline(void *target_stub, bool abi_bridge, bool return_thunk){
	unsigned char *current_stub = NULL;
	unsigned int offset = 0;
	#ifdef _WIN64
	const unsigned int trampoline_size = abi_bridge && !return_thunk ? 160 : 16;
	#else
	const unsigned int trampoline_size = 16;
	#endif

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

	if (director.trampoline_used + trampoline_size > TRAMP_REGION_SIZE){
		return NULL;
	}

	current_stub = director.trampoline_area + director.trampoline_used;
	director.trampoline_used += trampoline_size;
	if(return_thunk){
		current_stub[0] = 0xC3;
		memset(current_stub + 1, 0x90, trampoline_size - 1);
		__builtin___clear_cache((char *)current_stub,
					(char *)current_stub + trampoline_size);
		return current_stub;
	}
	#ifdef _WIN64
	if(abi_bridge){
	/* Linux modules use SysV while native Windows stubs use Microsoft x64. */
	current_stub[offset++] = 0x49; /* mov r10, rdx */
	current_stub[offset++] = 0x89;
	current_stub[offset++] = 0xD2;
	current_stub[offset++] = 0x49; /* mov r11, rcx */
	current_stub[offset++] = 0x89;
	current_stub[offset++] = 0xCB;
	current_stub[offset++] = 0x48; /* sub rsp, 0x78 */
	current_stub[offset++] = 0x83;
	current_stub[offset++] = 0xEC;
	current_stub[offset++] = 0x78;
	current_stub[offset++] = 0x4C; /* mov [rsp + 0x20], r8 */
	current_stub[offset++] = 0x89;
	current_stub[offset++] = 0x44;
	current_stub[offset++] = 0x24;
	current_stub[offset++] = 0x20;
	current_stub[offset++] = 0x4C; /* mov [rsp + 0x28], r9 */
	current_stub[offset++] = 0x89;
	current_stub[offset++] = 0x4C;
	current_stub[offset++] = 0x24;
	current_stub[offset++] = 0x28;

	/* Copy SysV stack arguments 7..12 into Microsoft argument slots 7..12. */
	for (unsigned int arg = 0; arg < 6; arg++) {
		unsigned int source = 0x80 + arg * 8;
		unsigned int destination = 0x30 + arg * 8;

		current_stub[offset++] = 0x48; /* mov rax, [rsp + source] */
		current_stub[offset++] = 0x8B;
		current_stub[offset++] = 0x84;
		current_stub[offset++] = 0x24;
		*(u32 *)(current_stub + offset) = source;
		offset += sizeof(u32);
		current_stub[offset++] = 0x48; /* mov [rsp + destination], rax */
		current_stub[offset++] = 0x89;
		current_stub[offset++] = 0x44;
		current_stub[offset++] = 0x24;
		current_stub[offset++] = destination;
	}

	current_stub[offset++] = 0x48; /* mov rcx, rdi */
	current_stub[offset++] = 0x89;
	current_stub[offset++] = 0xF9;
	current_stub[offset++] = 0x48; /* mov rdx, rsi */
	current_stub[offset++] = 0x89;
	current_stub[offset++] = 0xF2;
	current_stub[offset++] = 0x4D; /* mov r8, r10 */
	current_stub[offset++] = 0x89;
	current_stub[offset++] = 0xD0;
	current_stub[offset++] = 0x4D; /* mov r9, r11 */
	current_stub[offset++] = 0x89;
	current_stub[offset++] = 0xD9;
	current_stub[offset++] = 0x48; /* movabs rax, target_stub */
	current_stub[offset++] = 0xB8;
	*(u64 *)(current_stub + offset) = (u64)(uintptr_t)target_stub;
	offset += sizeof(u64);
	current_stub[offset++] = 0xFF; /* call rax */
	current_stub[offset++] = 0xD0;
	current_stub[offset++] = 0x48; /* add rsp, 0x78 */
	current_stub[offset++] = 0x83;
	current_stub[offset++] = 0xC4;
	current_stub[offset++] = 0x78;
	current_stub[offset++] = 0xC3; /* ret */
		__builtin___clear_cache((char *)current_stub,
					(char *)current_stub + trampoline_size);
		return current_stub;
	}
	#endif
	/* movabs r11, imm64 ; jmp r11 */
	current_stub[0]  = 0x49;
	current_stub[1]  = 0xBB;
	*(uint64_t *)(current_stub + 2) = (uint64_t)(uintptr_t)target_stub;
	current_stub[10] = 0x41;
	current_stub[11] = 0xFF;
	current_stub[12] = 0xE3;
	current_stub[13] = 0x90;
	current_stub[14] = 0x90;
	current_stub[15] = 0x90;
	__builtin___clear_cache((char *)current_stub,
				(char *)current_stub + trampoline_size);
	return current_stub;
}

insn_attr_t inat_get_opcode_attribute(insn_byte_t opcode){
	return inat_primary_table[opcode];
}

insn_attr_t inat_get_escape_attribute(insn_byte_t opcode, int lpfx_id, insn_attr_t esc_attr){
	int ret = 0;
	int n = 0;
	const insn_attr_t *table = NULL;

	n = inat_escape_id(esc_attr);

	table = inat_escape_tables[n][0];
	if (!table){
		return ret;
	}
	if (inat_has_variant(table[opcode]) && lpfx_id) {
		table = inat_escape_tables[n][lpfx_id];
		if (!table){
			return ret;
		}
	}
	return table[opcode];
}

insn_attr_t inat_get_group_attribute(insn_byte_t modrm, int lpfx_id, insn_attr_t grp_attr){
	int n = 0;
	const insn_attr_t *table = NULL;

	n = inat_group_id(grp_attr);

	table = inat_group_tables[n][0];
	if (table == NULL){
		return inat_group_common_attribute(grp_attr);
	}
	if (inat_has_variant(table[X86_MODRM_REG(modrm)]) && lpfx_id) {
		table = inat_group_tables[n][lpfx_id];
		if (table == NULL){
			return inat_group_common_attribute(grp_attr);
		}
	}
	return table[X86_MODRM_REG(modrm)] | inat_group_common_attribute(grp_attr);
}

insn_attr_t inat_get_avx_attribute(insn_byte_t opcode, insn_byte_t vex_m, insn_byte_t vex_p){
	int ret = 0;
	const insn_attr_t *table = NULL;

	if (vex_m > X86_VEX_M_MAX || vex_p > INAT_LSTPFX_MAX){
		return ret;
	}
	/* At first, this checks the master table */
	table = inat_avx_tables[vex_m][0];
	if (table == NULL){
		return ret;
	}
	if (!inat_is_group(table[opcode]) && vex_p) {
		/* If this is not a group, get attribute directly */
		table = inat_avx_tables[vex_m][vex_p];
		if (table == NULL){
			return ret;
		}
	}
	return table[opcode];
}

insn_attr_t inat_get_xop_attribute(insn_byte_t opcode, insn_byte_t map_select){
	int ret = 0;
	const insn_attr_t *table = NULL;

	if (map_select < X86_XOP_M_MIN || map_select > X86_XOP_M_MAX){
		return ret;
	}
	map_select -= X86_XOP_M_MIN;
	/* At first, this checks the master table */
	table = inat_xop_tables[map_select];
	if (table == NULL){
		return ret;
	}
	return table[opcode];
}

static int __insn_get_emulate_prefix(struct insn *insn, const insn_byte_t *prefix, size_t len){
	for (int i = 0; i < len; i++) {
		if (peek_nbyte_next(insn_byte_t, insn, i) != prefix[i])
			goto err_out;
	}

	insn->emulate_prefix_size = len;
	insn->next_byte += len;

	return 1;
	err_out:
	return 0;
}

static void insn_get_emulate_prefix(struct insn *insn){
	if (__insn_get_emulate_prefix(insn, xen_prefix, sizeof(xen_prefix))){
		return;
	}

	__insn_get_emulate_prefix(insn, kvm_prefix, sizeof(kvm_prefix));
}

static inline insn_byte_t insn_vex_m_bits(struct insn *insn){
	if (insn->vex_prefix.nbytes == 2){		/* 2 bytes VEX */
		return X86_VEX2_M;
	}else if (insn->vex_prefix.nbytes == 3){	/* 3 bytes VEX */
		return X86_VEX3_M(insn->vex_prefix.bytes[1]);
	}else{						/* EVEX */
		return X86_EVEX_M(insn->vex_prefix.bytes[1]);
	}
}

static inline insn_byte_t insn_vex_p_bits(struct insn *insn){
	if (insn->vex_prefix.nbytes == 2){	/* 2 bytes VEX */
		return X86_VEX_P(insn->vex_prefix.bytes[1]);
	}else{
		return X86_VEX_P(insn->vex_prefix.bytes[2]);
	}
}

static inline insn_byte_t insn_vex_w_bit(struct insn *insn){
	int ret = 0;

	if (insn->vex_prefix.nbytes < 3){
		return ret;
	}
	return X86_VEX_W(insn->vex_prefix.bytes[2]);
}

static inline insn_byte_t insn_xop_map_bits(struct insn *insn){
	int ret = 0;

	if (insn->xop_prefix.nbytes < 3){	/* XOP is 3 bytes */
		return ret;
	}
	return X86_XOP_M(insn->xop_prefix.bytes[1]);
}

static inline insn_byte_t insn_xop_p_bits(struct insn *insn){
	return X86_XOP_P(insn->vex_prefix.bytes[2]);
}

int insn_get_prefixes(struct insn *insn){
	int ret = 0;
	int i = 0;
	int nb = 0;
	struct insn_field *prefixes = &insn->prefixes;
	insn_attr_t attr;
	insn_byte_t b, lb;

	if (prefixes->got){
		return ret;
	}
	insn_get_emulate_prefix(insn);

	nb = 0;
	lb = 0;
	b = peek_next(insn_byte_t, insn);
	attr = inat_get_opcode_attribute(b);
	while (inat_is_legacy_prefix(attr)) {
		/* Skip if same prefix */
		for (i = 0; i < nb; i++){
			if (prefixes->bytes[i] == b){
				goto found;
			}
		}
		if (nb == 4){
			/* Invalid instruction */
			break;
		}
		prefixes->bytes[nb++] = b;
		if (inat_is_address_size_prefix(attr)) {
			/* address size switches 2/4 or 4/8 */
			if (insn->x86_64){
				insn->addr_bytes ^= 12;
			}else{
				insn->addr_bytes ^= 6;
			}
		}else if (inat_is_operand_size_prefix(attr)) {
			/* oprand size switches 2/4 */
			insn->opnd_bytes ^= 6;
		}
	found:
		prefixes->nbytes++;
		insn->next_byte++;
		lb = b;
		b = peek_next(insn_byte_t, insn);
		attr = inat_get_opcode_attribute(b);
	}
	/* Set the last prefix */
	if (lb && lb != insn->prefixes.bytes[3]) {
		if (unlikely(insn->prefixes.bytes[3])) {
			/* Swap the last prefix */
			b = insn->prefixes.bytes[3];
			for (i = 0; i < nb; i++){
				if (prefixes->bytes[i] == lb){
					insn_set_byte(prefixes, i, b);
				}
			}
		}
		insn_set_byte(&insn->prefixes, 3, lb);
	}

	/* Decode REX prefix */
	if (insn->x86_64) {
		b = peek_next(insn_byte_t, insn);
		attr = inat_get_opcode_attribute(b);
		if (inat_is_rex_prefix(attr)) {
			insn_field_set(&insn->rex_prefix, b, 1);
			insn->next_byte++;
			if (X86_REX_W(b)){
				/* REX.W overrides opnd_size */
				insn->opnd_bytes = 8;
			}
		}else if (inat_is_rex2_prefix(attr)) {
			insn_set_byte(&insn->rex_prefix, 0, b);
			b = peek_nbyte_next(insn_byte_t, insn, 1);
			insn_set_byte(&insn->rex_prefix, 1, b);
			insn->rex_prefix.nbytes = 2;
			insn->next_byte += 2;
			if (X86_REX_W(b)){
				/* REX.W overrides opnd_size */
				insn->opnd_bytes = 8;
			}
			insn->rex_prefix.got = 1;
			goto vex_end;
		}
	}
	insn->rex_prefix.got = 1;

	/* Decode VEX/XOP prefix */
	b = peek_next(insn_byte_t, insn);
	if (inat_is_vex_prefix(attr) || inat_is_xop_prefix(attr)) {
		insn_byte_t b2 = peek_nbyte_next(insn_byte_t, insn, 1);

		if (inat_is_xop_prefix(attr) && X86_MODRM_REG(b2) == 0) {
			/* Grp1A.0 is always POP Ev */
			goto vex_end;
		} else if (!insn->x86_64) {
			/*
			 * In 32-bits mode, if the [7:6] bits (mod bits of
			 * ModRM) on the second byte are not 11b, it is
			 * LDS or LES or BOUND.
			 */
			if (X86_MODRM_MOD(b2) != 3){
				goto vex_end;
			}
		}
		insn_set_byte(&insn->vex_prefix, 0, b);
		insn_set_byte(&insn->vex_prefix, 1, b2);
		if (inat_is_evex_prefix(attr)) {
			b2 = peek_nbyte_next(insn_byte_t, insn, 2);
			insn_set_byte(&insn->vex_prefix, 2, b2);
			b2 = peek_nbyte_next(insn_byte_t, insn, 3);
			insn_set_byte(&insn->vex_prefix, 3, b2);
			insn->vex_prefix.nbytes = 4;
			insn->next_byte += 4;
			if (insn->x86_64 && X86_VEX_W(b2))
				/* VEX.W overrides opnd_size */
				insn->opnd_bytes = 8;
		}else if (inat_is_vex3_prefix(attr) || inat_is_xop_prefix(attr)) {
			b2 = peek_nbyte_next(insn_byte_t, insn, 2);
			insn_set_byte(&insn->vex_prefix, 2, b2);
			insn->vex_prefix.nbytes = 3;
			insn->next_byte += 3;
			if (insn->x86_64 && X86_VEX_W(b2))
				/* VEX.W/XOP.W overrides opnd_size */
				insn->opnd_bytes = 8;
		}else{
			/*
			 * For VEX2, fake VEX3-like byte#2.
			 * Makes it easier to decode vex.W, vex.vvvv,
			 * vex.L and vex.pp. Masking with 0x7f sets vex.W == 0.
			 */
			insn_set_byte(&insn->vex_prefix, 2, b2 & 0x7f);
			insn->vex_prefix.nbytes = 2;
			insn->next_byte += 2;
		}
	}
	vex_end:
	insn->vex_prefix.got = 1;

	prefixes->got = 1;

	return ret;

	err_out:
	ret = -ENODATA;
	return ret;
}

static inline int insn_is_rex2(struct insn *insn){
	if (!insn->prefixes.got){
		insn_get_prefixes(insn);
	}
	return insn->rex_prefix.nbytes == 2;
}

static inline insn_byte_t insn_rex2_m_bit(struct insn *insn){
	return X86_REX2_M(insn->rex_prefix.bytes[1]);
}

static inline int insn_is_avx_or_xop(struct insn *insn){
	if (!insn->prefixes.got){
		insn_get_prefixes(insn);
	}
	return (insn->vex_prefix.value != 0);
}

static inline int insn_is_evex(struct insn *insn){
	if (!insn->prefixes.got){
		insn_get_prefixes(insn);
	}
	return (insn->vex_prefix.nbytes == 4);
}

/* If we already know this is AVX/XOP encoded */
static inline int avx_insn_is_xop(struct insn *insn){
	insn_attr_t attr = inat_get_opcode_attribute(insn->vex_prefix.bytes[0]);

	return inat_is_xop_prefix(attr);
}

void __text_gen_insn(void *buf, u8 opcode, const void *addr, const void *dest, int size){
	union text_poke_insn *insn = NULL;

	insn = buf;

	/*
	 * Hide the addresses to avoid the compiler folding in constants when
	 * referencing code, these can mess up annotations like
	 * ANNOTATE_NOENDBR.
	 */
	OPTIMIZER_HIDE_VAR(insn);
	OPTIMIZER_HIDE_VAR(addr);
	OPTIMIZER_HIDE_VAR(dest);

	insn->opcode = opcode;

	if (size > 1) {
		insn->disp = (long)dest - (long)(addr + size);
		if (size == 2) {
			/*
			 * Ensure that for JMP8 the displacement
			 * actually fits the signed byte.
			 */
		}
	}
}

/* Decode moffset16/32/64. Return 0 if failed */
static int __get_moffset(struct insn *insn){
	int ret = 0;

	switch (insn->addr_bytes) {
	case 2:
		insn_field_set(&insn->moffset1, get_next(short, insn), 2);
		break;
	case 4:
		insn_field_set(&insn->moffset1, get_next(int, insn), 4);
		break;
	case 8:
		insn_field_set(&insn->moffset1, get_next(int, insn), 4);
		insn_field_set(&insn->moffset2, get_next(int, insn), 4);
		break;
	default:	/* opnd_bytes must be modified manually */
		goto err_out;
	}
	insn->moffset1.got = insn->moffset2.got = 1;

	return 1;

	err_out:
	return ret;
}

/* Decode imm v32(Iz). Return 0 if failed */
static int __get_immv32(struct insn *insn){
	switch (insn->opnd_bytes) {
	case 2:
		insn_field_set(&insn->immediate, get_next(short, insn), 2);
		break;
	case 4:
	case 8:
		insn_field_set(&insn->immediate, get_next(int, insn), 4);
		break;
	default:	/* opnd_bytes must be modified manually */
		goto err_out;
	}

	return 1;

err_out:
	return 0;
}

/* Decode imm v64(Iv/Ov), Return 0 if failed */
static int __get_immv(struct insn *insn){
	switch (insn->opnd_bytes) {
	case 2:
		insn_field_set(&insn->immediate1, get_next(short, insn), 2);
		break;
	case 4:
		insn_field_set(&insn->immediate1, get_next(int, insn), 4);
		insn->immediate1.nbytes = 4;
		break;
	case 8:
		insn_field_set(&insn->immediate1, get_next(int, insn), 4);
		insn_field_set(&insn->immediate2, get_next(int, insn), 4);
		break;
	default:	/* opnd_bytes must be modified manually */
		goto err_out;
	}
	insn->immediate1.got = insn->immediate2.got = 1;

	return 1;
err_out:
	return 0;
}

/* Decode ptr16:16/32(Ap) */
static int __get_immptr(struct insn *insn){
	switch (insn->opnd_bytes) {
	case 2:
		insn_field_set(&insn->immediate1, get_next(short, insn), 2);
		break;
	case 4:
		insn_field_set(&insn->immediate1, get_next(int, insn), 4);
		break;
	case 8:
		/* ptr16:64 is not exist (no segment) */
		return 0;
	default:	/* opnd_bytes must be modified manually */
		goto err_out;
	}
	insn_field_set(&insn->immediate2, get_next(unsigned short, insn), 2);
	insn->immediate1.got = insn->immediate2.got = 1;

	return 1;
err_out:
	return 0;
}

int inat_get_last_prefix_id(insn_byte_t last_pfx){
	insn_attr_t lpfx_attr;

	lpfx_attr = inat_get_opcode_attribute(last_pfx);
	return inat_last_prefix_id(lpfx_attr);
}

/* Get the last prefix id from last prefix or VEX prefix */
static inline int insn_last_prefix_id(struct insn *insn)
{
	if (insn_is_avx_or_xop(insn)) {
		if (avx_insn_is_xop(insn)){
			return insn_xop_p_bits(insn);
		}
		return insn_vex_p_bits(insn);	/* VEX_p is a SIMD prefix id */
	}

	if (insn->prefixes.bytes[3]){
		return inat_get_last_prefix_id(insn->prefixes.bytes[3]);
	}
	return 0;
}

int insn_get_opcode(struct insn *insn){
	struct insn_field *opcode = &insn->opcode;
	int pfx_id, ret;
	insn_byte_t op;

	if (opcode->got)
		return 0;

	ret = insn_get_prefixes(insn);
	if (ret)
		return ret;

	/* Get first opcode */
	op = get_next(insn_byte_t, insn);
	insn_set_byte(opcode, 0, op);
	opcode->nbytes = 1;

	/* Check if there is VEX/XOP prefix or not */
	if (insn_is_avx_or_xop(insn)) {
		insn_byte_t m, p;

		/* XOP prefix has different encoding */
		if (unlikely(avx_insn_is_xop(insn))) {
			m = insn_xop_map_bits(insn);
			insn->attr = inat_get_xop_attribute(op, m);
			if (!inat_accept_xop(insn->attr)) {
				insn->attr = 0;
				return -EINVAL;
			}
			/* XOP has only 1 byte for opcode */
			goto end;
		}

		m = insn_vex_m_bits(insn);
		p = insn_vex_p_bits(insn);
		insn->attr = inat_get_avx_attribute(op, m, p);
		/* SCALABLE EVEX uses p bits to encode operand size */
		if (inat_evex_scalable(insn->attr) && !insn_vex_w_bit(insn) &&
		    p == INAT_PFX_OPNDSZ)
			insn->opnd_bytes = 2;
		if ((inat_must_evex(insn->attr) && !insn_is_evex(insn)) ||
		    (!inat_accept_vex(insn->attr) &&
		     !inat_is_group(insn->attr))) {
			/* This instruction is bad */
			insn->attr = 0;
			return -EINVAL;
		}
		/* VEX has only 1 byte for opcode */
		goto end;
	}

	/* Check if there is REX2 prefix or not */
	if (insn_is_rex2(insn)) {
		if (insn_rex2_m_bit(insn)) {
			/* map 1 is escape 0x0f */
			insn_attr_t esc_attr = inat_get_opcode_attribute(0x0f);

			pfx_id = insn_last_prefix_id(insn);
			insn->attr = inat_get_escape_attribute(op, pfx_id, esc_attr);
		} else {
			insn->attr = inat_get_opcode_attribute(op);
		}
		goto end;
	}

	insn->attr = inat_get_opcode_attribute(op);
	if (insn->x86_64 && inat_is_invalid64(insn->attr)) {
		/* This instruction is invalid, like UD2. Stop decoding. */
		insn->attr &= INAT_INV64;
	}

	while (inat_is_escape(insn->attr)) {
		/* Get escaped opcode */
		op = get_next(insn_byte_t, insn);
		opcode->bytes[opcode->nbytes++] = op;
		pfx_id = insn_last_prefix_id(insn);
		insn->attr = inat_get_escape_attribute(op, pfx_id, insn->attr);
	}

	if (inat_must_vex(insn->attr)) {
		/* This instruction is bad */
		insn->attr = 0;
		return -EINVAL;
	}

end:
	opcode->got = 1;
	return 0;

err_out:
	return -ENODATA;
}

int insn_get_modrm(struct insn *insn){
	int ret = 0;
	struct insn_field *modrm = &insn->modrm;
	insn_byte_t pfx_id, mod;

	if (modrm->got){
		return 0;
	}
	ret = insn_get_opcode(insn);
	if (ret){
		return ret;
	}
	if (inat_has_modrm(insn->attr)) {
		mod = get_next(insn_byte_t, insn);
		insn_field_set(modrm, mod, 1);
		if (inat_is_group(insn->attr)) {
			pfx_id = insn_last_prefix_id(insn);
			insn->attr = inat_get_group_attribute(mod, pfx_id,
							      insn->attr);
			if (insn_is_avx_or_xop(insn) && !inat_accept_vex(insn->attr) &&
			    !inat_accept_xop(insn->attr)) {
				/* Bad insn */
				insn->attr = 0;
				return -EINVAL;
			}
		}
	}

	if (insn->x86_64 && inat_is_force64(insn->attr)){
		insn->opnd_bytes = 8;
	}
	modrm->got = 1;
	return 0;

err_out:
	return -ENODATA;
}

int insn_get_sib(struct insn *insn){
	int ret = 0;
	insn_byte_t modrm;

	if (insn->sib.got){
		return 0;
	}
	ret = insn_get_modrm(insn);
	if (ret){
		return ret;
	}
	if (insn->modrm.nbytes) {
		modrm = insn->modrm.bytes[0];
		if (insn->addr_bytes != 2 &&
		    X86_MODRM_MOD(modrm) != 3 && X86_MODRM_RM(modrm) == 4) {
			insn_field_set(&insn->sib, get_next(insn_byte_t, insn), 1);
		}
	}
	insn->sib.got = 1;

	return 0;

err_out:
	return -ENODATA;
}

int insn_get_displacement(struct insn *insn){
	int ret = 0;
	insn_byte_t mod, rm, base;

	if (insn->displacement.got){
		return ret;
	}

	ret = insn_get_sib(insn);
	if (ret){
		return ret;
	}
	if (insn->modrm.nbytes) {
		/*
		 * Interpreting the modrm byte:
		 * mod = 00 - no displacement fields (exceptions below)
		 * mod = 01 - 1-byte displacement field
		 * mod = 10 - displacement field is 4 bytes, or 2 bytes if
		 * 	address size = 2 (0x67 prefix in 32-bit mode)
		 * mod = 11 - no memory operand
		 *
		 * If address size = 2...
		 * mod = 00, r/m = 110 - displacement field is 2 bytes
		 *
		 * If address size != 2...
		 * mod != 11, r/m = 100 - SIB byte exists
		 * mod = 00, SIB base = 101 - displacement field is 4 bytes
		 * mod = 00, r/m = 101 - rip-relative addressing, displacement
		 * 	field is 4 bytes
		 */
		mod = X86_MODRM_MOD(insn->modrm.value);
		rm = X86_MODRM_RM(insn->modrm.value);
		base = X86_SIB_BASE(insn->sib.value);
		if (mod == 3)
			goto out;
		if (mod == 1) {
			insn_field_set(&insn->displacement, get_next(signed char, insn), 1);
		} else if (insn->addr_bytes == 2) {
			if ((mod == 0 && rm == 6) || mod == 2) {
				insn_field_set(&insn->displacement, get_next(short, insn), 2);
			}
		} else {
			if ((mod == 0 && rm == 5) || mod == 2 || (mod == 0 && base == 5)) {
				insn_field_set(&insn->displacement,  get_next(int, insn), 4);
			}
		}
	}
out:
	insn->displacement.got = 1;
	return ret;

err_out:
	ret = -ENODATA;
	return ret;
}

int insn_get_immediate(struct insn *insn){
	int ret = 0;

	if (insn->immediate.got){
		return 0;
	}

	ret = insn_get_displacement(insn);
	if (ret){
		return ret;
	}
	if (inat_has_moffset(insn->attr)) {
		if (!__get_moffset(insn))
			goto err_out;
		goto done;
	}

	if (!inat_has_immediate(insn->attr)){
		goto done;
	}
	switch (inat_immediate_size(insn->attr)) {
	case INAT_IMM_BYTE:
		insn_field_set(&insn->immediate, get_next(signed char, insn), 1);
		break;
	case INAT_IMM_WORD:
		insn_field_set(&insn->immediate, get_next(short, insn), 2);
		break;
	case INAT_IMM_DWORD:
		insn_field_set(&insn->immediate, get_next(int, insn), 4);
		break;
	case INAT_IMM_QWORD:
		insn_field_set(&insn->immediate1, get_next(int, insn), 4);
		insn_field_set(&insn->immediate2, get_next(int, insn), 4);
		break;
	case INAT_IMM_PTR:
		if (!__get_immptr(insn)){
			goto err_out;
		}
		break;
	case INAT_IMM_VWORD32:
		if (!__get_immv32(insn))
			goto err_out;
		break;
	case INAT_IMM_VWORD:
		if (!__get_immv(insn))
			goto err_out;
		break;
	default:
		/* Here, insn must have an immediate, but failed */
		goto err_out;
	}
	if (inat_has_second_immediate(insn->attr)) {
		insn_field_set(&insn->immediate2, get_next(signed char, insn), 1);
	}
done:
	insn->immediate.got = 1;
	return 0;

err_out:
	return -ENODATA;
}

int insn_get_length(struct insn *insn){
	int ret = 0;

	if (insn->length){
		return ret;
	}

	ret = insn_get_immediate(insn);
	if (ret){
		return ret;
	}
	insn->length = (unsigned char)((unsigned long)insn->next_byte - (unsigned long)insn->kaddr);

	return ret;
}

static inline int insn_complete(struct insn *insn){
	return insn->opcode.got && insn->modrm.got && insn->sib.got && insn->displacement.got && insn->immediate.got;
}

void insn_init(struct insn *insn, const void *kaddr, int buf_len, int x86_64){
	/*
	 * Instructions longer than MAX_INSN_SIZE (15 bytes) are invalid
	 * even if the input buffer is long enough to hold them.
	 */
	if (buf_len > MAX_INSN_SIZE){
		buf_len = MAX_INSN_SIZE;
	}
	memset(insn, 0, sizeof(*insn));
	insn->kaddr = kaddr;
	insn->end_kaddr = kaddr + buf_len;
	insn->next_byte = kaddr;
	insn->x86_64 = x86_64;
	insn->opnd_bytes = 4;
	if (x86_64){
		insn->addr_bytes = 8;
	}else{
		insn->addr_bytes = 4;
	}
}

int insn_decode(struct insn *insn, const void *kaddr, int buf_len, enum insn_mode m){
	int ret = 0;

	if (m == INSN_MODE_KERN){
		insn_init(insn, kaddr, buf_len, true);
	}else{
		insn_init(insn, kaddr, buf_len, m == INSN_MODE_64);
	}

	ret = insn_get_length(insn);
	if (ret){
		return ret;
	}
	if (insn_complete(insn)){
		return 0;
	}
	return -EINVAL;
}

static bool skip_addr(void *dest){
	return false;
}

static void *call_get_dest(void *addr){
	int ret = 0;
	struct insn insn;
	void *dest = NULL;

	ret = insn_decode_kernel(&insn, addr);
	if (ret)
		return ERR_PTR(ret);

	/* Patched out call? */
	if (insn.opcode.bytes[0] != CALL_INSN_OPCODE){
		return NULL;
	}

	dest = addr + insn.length + insn.immediate.value;
	if (skip_addr(dest)){
		return NULL;
	}
	return dest;
}

static void add_nop(u8 *buf, unsigned int len){
	u8 *target = buf + len;

	if (len == 0){
		return;
	}
	if (len <= ASM_NOP_MAX) {
		memcpy(buf, x86_nops[len], len);
		return;
	}

	if (len < 128) {
		__text_gen_insn(buf, JMP8_INSN_OPCODE, buf, target, JMP8_INSN_SIZE);
		buf += JMP8_INSN_SIZE;
	} else {
		__text_gen_insn(buf, JMP32_INSN_OPCODE, buf, target, JMP32_INSN_SIZE);
		buf += JMP32_INSN_SIZE;
	}

	for (; buf < target; buf++){
		*buf = INT3_INSN_OPCODE;
	}
}


/*
 * Recognise typical NOP patterns for both 32bit and 64bit.
 *
 * Notably:
 *  - NOP, but not: REP NOP aka PAUSE
 *  - NOPL
 *  - MOV %reg, %reg
 *  - LEA 0(%reg),%reg
 *  - JMP +0
 *
 * Must not have false-positives; instructions identified as a NOP might be
 * emulated as a NOP (uprobe) or Run Length Encoded in a larger NOP
 * (alternatives).
 *
 * False-negatives are fine; need not be exhaustive.
 */
bool insn_is_nop(struct insn *insn){
	u8 b3 = 0, x3 = 0, r3 = 0;
	u8 b4 = 0, x4 = 0, r4 = 0, m = 0;
	u8 modrm, modrm_mod, modrm_reg, modrm_rm;
	u8 sib = 0, sib_scale, sib_index, sib_base;
	u8 nrex, rex;
	u8 p, rep = 0;

	if ((nrex = insn->rex_prefix.nbytes)) {
		rex = insn->rex_prefix.bytes[nrex-1];

		r3 = !!X86_REX_R(rex);
		x3 = !!X86_REX_X(rex);
		b3 = !!X86_REX_B(rex);
		if (nrex > 1) {
			r4 = !!X86_REX2_R(rex);
			x4 = !!X86_REX2_X(rex);
			b4 = !!X86_REX2_B(rex);
			m  = !!X86_REX2_M(rex);
		}

	} else if (insn->vex_prefix.nbytes) {
		/*
		 * Ignore VEX encoded NOPs
		 */
		return false;
	}

	if (insn->modrm.nbytes) {
		modrm = insn->modrm.bytes[0];
		modrm_mod = X86_MODRM_MOD(modrm);
		modrm_reg = X86_MODRM_REG(modrm) + 8*r3 + 16*r4;
		modrm_rm  = X86_MODRM_RM(modrm)  + 8*b3 + 16*b4;
		modrm = 1;
	}

	if (insn->sib.nbytes) {
		sib = insn->sib.bytes[0];
		sib_scale = X86_SIB_SCALE(sib);
		sib_index = X86_SIB_INDEX(sib) + 8*x3 + 16*x4;
		sib_base  = X86_SIB_BASE(sib)  + 8*b3 + 16*b4;
		sib = 1;

		modrm_rm = sib_base;
	}

	for_each_insn_prefix(insn, p) {
		if (p == 0xf3) /* REPE */
			rep = 1;
	}

	/*
	 * Opcode map munging:
	 *
	 * REX2: 0 - single byte opcode
	 *       1 - 0f second byte opcode
	 */
	switch (m) {
	case 0: break;
	case 1: insn->opcode.value <<= 8;
		insn->opcode.value |= 0x0f;
		break;
	default:
		return false;
	}

	switch (insn->opcode.bytes[0]) {
	case 0x0f: /* 2nd byte */
		break;

	case 0x89: /* MOV */
		if (modrm_mod != 3) /* register-direct */
			return false;

		/* native size */
		if (insn->opnd_bytes != 4 * (1 + insn->x86_64))
			return false;

		return modrm_reg == modrm_rm; /* MOV %reg, %reg */

	case 0x8d: /* LEA */
		if (modrm_mod == 0 || modrm_mod == 3) /* register-indirect with disp */
			return false;

		/* native size */
		if (insn->opnd_bytes != 4 * (1 + insn->x86_64))
			return false;

		if (insn->displacement.value != 0)
			return false;

		if (sib && (sib_scale != 0 || sib_index != 4)) /* (%reg, %eiz, 1) */
			return false;

		for_each_insn_prefix(insn, p) {
			if (p != 0x3e) /* DS */
				return false;
		}

		return modrm_reg == modrm_rm; /* LEA 0(%reg), %reg */

	case 0x90: /* NOP */
		if (b3 || b4) /* XCHG %r{8,16,24},%rax */
			return false;

		if (rep) /* REP NOP := PAUSE */
			return false;

		return true;

	case 0xe9: /* JMP.d32 */
	case 0xeb: /* JMP.d8 */
		return insn->immediate.value == 0; /* JMP +0 */

	default:
		return false;
	}

	switch (insn->opcode.bytes[1]) {
	case 0x1f:
		return modrm_reg == 0; /* 0f 1f /0 -- NOPL */

	default:
		return false;
	}
}

int skip_nops(u8 *buf, int offset, int len){
	struct insn insn = {};

	for (; offset < len; offset += insn.length) {
		if (insn_decode_kernel(&insn, &buf[offset])){
			break;
		}
		if (!insn_is_nop(&insn)){
			break;
		}
	}

	return offset;
}

static void __attribute__((noinline)) optimize_nops(const u8 * const instr, u8 *buf, size_t len){
	for (int next, i = 0; i < len; i = next) {
		struct insn insn = {};

		if (insn_decode_kernel(&insn, &buf[i])){
			return;
		}
		next = i + insn.length;

		if (insn_is_nop(&insn)) {
			int nop = i;

			/* Has the NOP already been optimized? */
			if (i + insn.length == len){
				return;
			}
			next = skip_nops(buf, next, len);

			add_nop(buf + nop, next - nop);
		}
	}
}

inline bool need_reloc(unsigned long offset, u8 *src, size_t src_len){
	u8 *target = src + offset;
	/*
	 * If the target is inside the patched block, it's relative to the
	 * block itself and does not need relocation.
	 */
	return (target < src || target > src + src_len);
}

inline void apply_reloc(int n, void *ptr, uintptr_t diff){
	switch (n) {
		case 1: apply_reloc_n(8, ptr, diff); break;
		case 2: apply_reloc_n(16, ptr, diff); break;
		case 4: apply_reloc_n(32, ptr, diff); break;
		default: break;
	}
}

int insn_rip_relative(struct insn *insn){
	int ret = 0;
	struct insn_field *modrm = &insn->modrm;

	if (!insn->x86_64){
		return 0;
	}
	ret = insn_get_modrm(insn);
	if (ret){
		return 0;
	}
	/*
	 * For rip-relative instructions, the mod field (top 2 bits)
	 * is zero and the r/m field (bottom 3 bits) is 0x5.
	 */
	return (modrm->nbytes && (modrm->bytes[0] & 0xc7) == 0x5);
}

static inline int insn_offset_rex_prefix(struct insn *insn){
	return insn->prefixes.nbytes;
}
static inline int insn_offset_vex_prefix(struct insn *insn){
	return insn_offset_rex_prefix(insn) + insn->rex_prefix.nbytes;
}
static inline int insn_offset_opcode(struct insn *insn){
	return insn_offset_vex_prefix(insn) + insn->vex_prefix.nbytes;
}
static inline int insn_offset_modrm(struct insn *insn){
	return insn_offset_opcode(insn) + insn->opcode.nbytes;
}
static inline int insn_offset_sib(struct insn *insn){
	return insn_offset_modrm(insn) + insn->modrm.nbytes;
}
static inline int insn_offset_displacement(struct insn *insn){
	return insn_offset_sib(insn) + insn->sib.nbytes;
}
static inline int insn_offset_immediate(struct insn *insn){
	return insn_offset_displacement(insn) + insn->displacement.nbytes;
}

void __apply_relocation(u8 *buf, const u8 * const instr, size_t instrlen, u8 *repl, size_t repl_len){
	for (int next, i = 0; i < instrlen; i = next) {
		struct insn insn = {};

		if (insn_decode_kernel(&insn, &buf[i])){
			return;
		}
		next = i + insn.length;

		switch (insn.opcode.bytes[0]) {
		case 0x0f:
			if (insn.opcode.bytes[1] < 0x80 ||
			    insn.opcode.bytes[1] > 0x8f)
				break;

			fallthrough;	/* Jcc.d32 */
		case 0x70 ... 0x7f:	/* Jcc.d8 */
		case JMP8_INSN_OPCODE:
		case JMP32_INSN_OPCODE:
		case CALL_INSN_OPCODE:
			if (need_reloc(next + insn.immediate.value, repl, repl_len)) {
				apply_reloc(insn.immediate.nbytes,
					    buf + i + insn_offset_immediate(&insn),
					    repl - instr);
			}

			/*
			 * Where possible, convert JMP.d32 into JMP.d8.
			 */
			if (insn.opcode.bytes[0] == JMP32_INSN_OPCODE) {
				s32 imm = insn.immediate.value;
				imm += repl - instr;
				imm += JMP32_INSN_SIZE - JMP8_INSN_SIZE;
				if ((imm >> 31) == (imm >> 7)) {
					buf[i+0] = JMP8_INSN_OPCODE;
					buf[i+1] = (s8)imm;

					memset(&buf[i+2], INT3_INSN_OPCODE, insn.length - 2);
				}
			}
			break;
		}

		if (insn_rip_relative(&insn)) {
			if (need_reloc(next + insn.displacement.value, repl, repl_len)) {
				apply_reloc(insn.displacement.nbytes,
					    buf + i + insn_offset_displacement(&insn),
					    repl - instr);
			}
		}
	}
}


void text_poke_apply_relocation(u8 *buf, const u8 * const instr, size_t instrlen, u8 *repl, size_t repl_len){
	__apply_relocation(buf, instr, instrlen, repl, repl_len);
	optimize_nops(instr, buf, instrlen);
}

void *patch_dest(void *dest, bool direct){
	unsigned int tsize = SKL_TMPL_SIZE;
	u8 insn_buff[MAX_PATCH_LEN];
	u8 *pad = dest - tsize;

	memcpy(insn_buff, skl_call_thunk_template, tsize);
	text_poke_apply_relocation(insn_buff, pad, tsize, skl_call_thunk_template, tsize);

	/* Already patched? */
	if (!memcmp(pad, insn_buff, tsize)){
		return pad;
	}

	/* Ensure there are nops */
	if (memcmp(pad, nops, tsize)) {
		printf("Invalid padding area for %p\n", dest);
		return NULL;
	}

	if (direct){
		memcpy(pad, insn_buff, tsize);
	}
	return pad;
}

void patch_call(void *addr, const struct core_text *ct){
	void *pad = NULL;
	void *dest = NULL;
	u8 bytes[8];

	dest = call_get_dest(addr);
	if(dest == NULL){
		return;
	}

	pad = patch_dest(dest, true);
	if (pad == NULL){
		return;
	}

	#ifdef ENABLE_DBG_PRINT
	printf("[*] patch call at: %p %p to %p %p -> %p \n", addr, addr, dest, dest, pad);
	#endif
	__text_gen_insn(bytes, CALL_INSN_OPCODE, addr, pad, CALL_INSN_SIZE);
	memcpy(addr, bytes, CALL_INSN_SIZE);
}

void patch_call_sites(s32 *start, s32 *end, const struct core_text *ct){
	s32 *s = NULL;

	for (s = start; s < end; s++){
		patch_call((void *)s + *s, ct);
	}
}

void callthunks_setup(struct callthunk_sites *cs, const struct core_text *ct){
	#ifdef ENABLE_DBG_PRINT
	printf("[*] patching call sites %s\n", ct->name);
	#endif
	patch_call_sites(cs->call_start, cs->call_end, ct);
	#ifdef ENABLE_DBG_PRINT
	printf("[*] patching call sites done: %s\n", ct->name);
	#endif
}

void __attribute__((noinline)) callthunks_patch_module_calls(struct callthunk_sites *cs, struct module *mod){
	struct core_text ct = {
		.base = (unsigned long)mod->mem[MOD_TEXT].base,
		.end  = (unsigned long)mod->mem[MOD_TEXT].base + mod->mem[MOD_TEXT].size,
		.name = mod->name,
	};

	callthunks_setup(cs, &ct);
}

__attribute__((section(".cpuidle.text"))) void BUG_func(void){
	/* goofy ways for simulating BUG() */
	exit(0);
}

static unsigned int alt_replace_call(u8 *instr, u8 *insn_buff, struct alt_instr *a){
	int ret = 0;
	s32 disp = 0;
	void *target = NULL;
	void *bug = &BUG_func;
	if (a->replacementlen != 5 || insn_buff[0] != CALL_INSN_OPCODE) {
		#ifdef ENABLE_DBG_PRINT
		printf("ALT_FLAG_DIRECT_CALL set for a non-call replacement instruction\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	if (a->instrlen != 6 || instr[0] != CALL_RIP_REL_OPCODE || instr[1] != CALL_RIP_REL_MODRM) {
		#ifdef ENABLE_DBG_PRINT
		printf("ALT_FLAG_DIRECT_CALL set for unrecognized indirect call\n");
		#endif
		ret = -EINVAL;
		return ret;
	}

	/* Skip CALL_RIP_REL_OPCODE and CALL_RIP_REL_MODRM */
	disp = *(s32 *)(instr + 2);
	/* ff 15 00 00 00 00   call   *0x0(%rip) */
	/* target address is stored at "next instruction + disp". */
	target = *(void **)(instr + a->instrlen + disp);
	if (target == NULL){
		target = bug;
	}
	/* (BUG_func - .) + (target - BUG_func) := target - . */
	*(s32 *)(insn_buff + 1) += target - bug;

	if (target == bug){
		return 0;
	}

	return 5;
}

/*
 * Replace instructions with better alternatives for this CPU type. This runs
 * before SMP is initialized to avoid SMP problems with self modifying code.
 * This implies that asymmetric systems where APs have less capabilities than
 * the boot processor are not handled. Tough. Make sure you disable such
 * features by hand.
 *
 * Marked "noinline" to cause control flow change and thus insn cache
 * to refetch changed I$ lines.
 */
void __attribute__((noinline)) apply_alternatives(struct alt_instr *start, struct alt_instr *end){
	u8 insn_buff[MAX_PATCH_LEN];
	u8 *instr, *replacement;
	struct alt_instr *a = NULL;
	struct alt_instr *b = NULL;

	#ifdef ENABLE_DBG_PRINT
	printf("alt table %p, -> %p", start, end);
	#endif

	/*
	 * The scan order should be from start to end. A later scanned
	 * alternative code can overwrite previously scanned alternative code.
	 * Some kernel functions (e.g. memcpy, memset, etc) use this order to
	 * patch code.
	 *
	 * So be careful if you want to change the scan order to any other
	 * order.
	 */
	for (a = start; a < end; a++) {
		unsigned int insn_buff_sz = 0;

		/*
		 * In case of nested ALTERNATIVE()s the outer alternative might
		 * add more padding. To ensure consistent patching find the max
		 * padding for all alt_instr entries for this site (nested
		 * alternatives result in consecutive entries).
		 */
		for (b = a+1; b < end && instr_va(b) == instr_va(a); b++) {
			u8 len = max(a->instrlen, b->instrlen);
			a->instrlen = b->instrlen = len;
		}

		instr = instr_va(a);
		replacement = (u8 *)&a->repl_offset + a->repl_offset;

		#if 0
		/*
		 * Patch if either:
		 * - feature is present
		 * - feature not present but ALT_FLAG_NOT is set to mean,
		 *   patch if feature is *NOT* present.
		 */
		if (!boot_cpu_has(a->cpuid) == !(a->flags & ALT_FLAG_NOT)) {
			memcpy(insn_buff, instr, a->instrlen);
			optimize_nops(instr, insn_buff, a->instrlen);
			memcpy(instr, insn_buff, a->instrlen);
			continue;
		}
		#endif

		#ifdef ENABLE_DBG_PRINT
		printf("feat: %d*32+%d, old: (%p (%p) len: %d), repl: (%p, len: %d) flags: 0x%x",
			a->cpuid >> 5,
			a->cpuid & 0x1f,
			instr, instr, a->instrlen,
			replacement, a->replacementlen, a->flags);
		#endif
		memcpy(insn_buff, replacement, a->replacementlen);
		insn_buff_sz = a->replacementlen;

		if (a->flags & (1 << 1)){
			insn_buff_sz = alt_replace_call(instr, insn_buff, a);
		}
		for (; insn_buff_sz < a->instrlen; insn_buff_sz++){
			insn_buff[insn_buff_sz] = 0x90;
		}
		text_poke_apply_relocation(insn_buff, instr, a->instrlen, replacement, a->replacementlen);

		#ifdef ENABLE_DBG_PRINT
		DUMP_BYTES(ALT, instr, a->instrlen, "%p:   old_insn: ", instr);
		DUMP_BYTES(ALT, replacement, a->replacementlen, "%p:   rpl_insn: ", replacement);
		DUMP_BYTES(ALT, insn_buff, insn_buff_sz, "%p: final_insn: ", instr);
		#endif

		memcpy(instr, insn_buff, insn_buff_sz);
	}
}

int patch_return(void *addr, struct insn *insn, u8 *bytes){
	int i = 0;

	/* Patch the custom return thunks... */
	i = JMP32_INSN_SIZE;
	__text_gen_insn(bytes, JMP32_INSN_OPCODE, addr, x86_return_thunk, i);

	for (; i < insn->length;){
		bytes[i++] = INT3_INSN_OPCODE;
	}
	return i;
}

/*
 * This is called by apply_returns() to fix up static call trampolines,
 * specifically ARCH_DEFINE_STATIC_CALL_NULL_TRAMP which is recorded as
 * having a return trampoline.
 *
 * The problem is that static_call() is available before determining
 * X86_FEATURE_RETHUNK and, by implication, running alternatives.
 *
 * This means that __static_call_transform() above can have overwritten the
 * return trampoline and we now need to fix things up to be consistent.
 */
bool __static_call_fixup(void *tramp, u8 op, void *dest){
	unsigned long addr = (unsigned long)tramp;
	static const u8 tramp_ud[] = { 0x0f, 0xb9, 0xcc };
	/*
	 * Not all .return_sites are a static_call trampoline (most are not).
	 * Check if the 3 bytes after the return are still kernel text, if not,
	 * then this definitely is not a trampoline and we need not worry
	 * further.
	 *
	 * This avoids the memcmp() below tripping over pagefaults etc..
	 */
	if (((addr >> PAGE_SHIFT) != ((addr + 7) >> PAGE_SHIFT))){
		return false;
	}

	if (memcmp(tramp + 5, tramp_ud, 3)) {
		/* Not a trampoline site, not our problem. */
		return false;
	}

	if (op == RET_INSN_OPCODE || dest == &__x86_return_thunk){
		//__static_call_transform(tramp, RET, NULL, true);
	}

	return true;
}

void __attribute__((noinline)) apply_returns(s32 *start, s32 *end){
	s32 *s = NULL;

	for (s = start; s < end; s++) {
		void *dest = NULL, *addr = (void *)s + *s;
		struct insn insn;
		int len, ret;
		u8 bytes[16];
		u8 op;

		ret = insn_decode_kernel(&insn, addr);
		if (ret < 0){
			continue;
		}
		op = insn.opcode.bytes[0];
		if (op == JMP32_INSN_OPCODE){
			dest = addr + insn.length + insn.immediate.value;
		}
		if (__static_call_fixup(addr, op, dest) || dest != &__x86_return_thunk){
			continue;
		}

		#ifdef ENABLE_DBG_PRINT
		printf("return thunk at: %p (%p) len: %d to: %p",
			addr, addr, insn.length,
			addr + insn.length + insn.immediate.value);
		#endif
		len = patch_return(addr, &insn, bytes);
		if (len == insn.length) {
			#ifdef ENABLE_DBG_PRINT
			DUMP_BYTES(RET, ((u8*)addr),  len, "%p: orig: ", addr);
			DUMP_BYTES(RET, ((u8*)bytes), len, "%p: repl: ", addr);
			#endif
			memcpy(addr, bytes, len);
		}
	}
}

u32 decode_caller_hash(void *addr){
	u8 *p = addr;

	/* 41 ba 88 a9 cb ed       mov    $(-0x12345678),%r10d */
	if (p[0] == 0x41 && p[1] == 0xba){
		return -*(u32 *)(addr + 2);
	}
	/* e8 0c 88 a9 cb ed	   jmp.d8  +12 */
	if (p[0] == JMP8_INSN_OPCODE && p[1] == fineibt_caller_jmp){
		return -*(u32 *)(addr + 2);
	}
	return 0; /* invalid hash value */
}

int cfi_disable_callers(s32 *start, s32 *end){
	/*
	 * Disable kCFI by patching in a JMP.d8, this leaves the hash immediate
	 * in tact for later usage. Also see decode_caller_hash() and
	 * cfi_rewrite_callers().
	 */
	const u8 jmp[] = { JMP8_INSN_OPCODE, fineibt_caller_jmp };
	s32 *s = NULL;

	for (s = start; s < end; s++) {
		void *addr = (void *)s + *s;
		u32 hash = 0;

		addr -= fineibt_caller_size;
		hash = decode_caller_hash(addr);
		if (!hash){ /* nocfi callers */
			continue;
		}
		memcpy(addr, jmp, 2);
	}
	return 0;
}

void __apply_fineibt(s32 *start_retpoline, s32 *end_retpoline, s32 *start_cfi, s32 *end_cfi, bool builtin){
	int ret = 0;
	/*
	 * Rewrite the callers to not use the __cfi_ stubs, such that we might
	 * rewrite them. This disables all CFI. If this succeeds but any of the
	 * later stages fails, we're without CFI.
	 */
	#ifdef ENABLE_DBG_PRINT
	printf("CFI: disabling all indirect call checking\n");
	#endif
	ret = cfi_disable_callers(start_retpoline, end_retpoline);
}


void apply_fineibt(s32 *start_retpoline, s32 *end_retpoline, s32 *start_cfi, s32 *end_cfi){
	__apply_fineibt(start_retpoline, end_retpoline, start_cfi, end_cfi, /* .builtin = */ false);
}

int reg_from_indirect_thunk(void *thunk_addr){
	const char *p = NULL;
	const char *name = NULL;
	static const struct {
		const char *suffix;
		int reg;
	}map[] = {
		{ "_rax", 0 },
		{ "_rcx", 1 },
		{ "_rdx", 2 },
		{ "_rbx", 3 },
		{ "_rbp", 5 },
		{ "_rsi", 6 },
		{ "_rdi", 7 },
		{ "_r8",  8 },
		{ "_r9",  9 },
		{ "_r10",10 },
		{ "_r11",11 },
		{ "_r12",12 },
		{ "_r13",13 },
		{ "_r14",14 },
		{ "_r15",15 },
	};

	for (int i = 0; sym_table[i].function_ptr != NULL; i++) {
		if (sym_table[i].function_ptr != thunk_addr){
			continue;
		}
		name = (const char *)sym_table[i].function_name;
		if (!strstr(name, "__x86_indirect_thunk_")){
			continue;
		}
		for (int j = 0; j < (int)(sizeof(map)/sizeof(map[0])); j++) {
			p = strstr(name, map[j].suffix);
			if (p && p[strlen(map[j].suffix)] == '\0'){
				return map[j].reg;
			}
		}
	}
	return -1;
}

void apply_retpolines(s32 *start, s32 *end){
	for (s32 *s = start; s < end; s++) {
		void       *addr = (void *)((uintptr_t)s + (uintptr_t)*s);
		struct insn insn  = {};
		void       *thunk = NULL;
		int         reg = 0;
		u8          buf[8];
		int         write_len;

		if (insn_decode_kernel(&insn, addr)){
			continue;
		}

		if (insn.opcode.bytes[0] != CALL_INSN_OPCODE){
			continue;
		}

		thunk = (u8 *)addr + insn.length + insn.immediate.value;

		reg = reg_from_indirect_thunk(thunk);
		if (reg < 0) {
			#ifdef ENABLE_DBG_PRINT
			printf("[*] %s: unknown thunk at %p for site %p\n", __FUNCTION__, thunk, addr);
			#endif
			continue;
		}

		write_len = insn.length;
		memset(buf, 0x90, write_len);
		if (reg < 8) {
			buf[0] = 0xFF;
			buf[1] = 0xD0 | (u8)(reg & 7);
		}else{
			buf[0] = 0x41;
			buf[1] = 0xFF;
			buf[2] = 0xD0 | (u8)(reg & 7);
		}

		#ifdef ENABLE_DBG_PRINT
		printf("[*] %s: site %p -> %s (r%d): %02x %02x %02x %02x %02x\n", __FUNCTION__, addr, (reg < 8 ? "low" : "r8+"), reg,
		buf[0], buf[1], buf[2], buf[3], buf[4]);
		#endif
		memcpy(addr, buf, write_len);
		__builtin___clear_cache((char *)addr, (char *)addr + write_len);
	}
}

int module_finalize(const Elf(Ehdr) *hdr, const Elf(Shdr) *sechdrs, struct module *mod){
	unsigned int rsize      	= 0;
	unsigned int csize      	= 0;
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
	void *rseg              	= NULL;
	void *cseg              	= NULL;
	char *secstrings  		= (void *)hdr + sechdrs[hdr->e_shstrndx].sh_offset;
	struct callthunk_sites cs 	= {};

        for (s = sechdrs; s < sechdrs + hdr->e_shnum; s++) {
                if (!strcmp(".altinstructions", secstrings + s->sh_name)){
			alt = s;
                }

                if (!strcmp(".smp_locks", secstrings + s->sh_name)){
			locks = s;
                }

                if (!strcmp(".orc_unwind", secstrings + s->sh_name)){
			orc = s;
                }

                if (!strcmp(".orc_unwind_ip", secstrings + s->sh_name)){
			orc_ip = s;
                }

                if (!strcmp(".retpoline_sites", secstrings + s->sh_name)){
			retpolines = s;
                }

                if (!strcmp(".return_sites", secstrings + s->sh_name)){
			returns = s;
                }

                if (!strcmp(".call_sites", secstrings + s->sh_name)){
			calls = s;
                }
		if (!strcmp(".cfi_sites", secstrings + s->sh_name)){
			cfi = s;
                }

                if (!strcmp(".ibt_endbr_seal", secstrings + s->sh_name)){
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
                /*
		 * Build the kernel with support for Indirect Branch Tracking, a
		 * hardware support course-grain forward-edge Control Flow Integrity
		 * protection. It enforces that all indirect calls must land on
		 * an ENDBR instruction, as such, the compiler will instrument the
		 * code with them to make this happen.
		 * In addition to building the kernel with IBT, seal all functions that
		 * are not indirect call targets, avoiding them ever becoming one.
		 * This requires LTO like objtool runs and will slow down the build. It
		 * does significantly reduce the number of ENDBR instructions in the
		 * kernel image.
		 */

		apply_fineibt(rseg, rseg + rsize, cseg, cseg + csize);
        }
        if (retpolines) {
		rseg              = (void *)retpolines->sh_addr;

                /*
		 *
		 * Compile kernel with the retpoline compiler options to guard against
		 * kernel-to-user data leaks by avoiding speculative indirect
		 * branches. Requires a compiler with -mindirect-branch=thunk-extern
		 * support for full protection. The kernel may run slower.
		 */

		apply_retpolines(rseg, rseg + retpolines->sh_size);
        }
        if (returns) {
                void *rseg              = (void *)returns->sh_addr;

                /*
		 * Compile kernel with the retpoline compiler options to guard against
		 * kernel-to-user data leaks by avoiding speculative indirect
		 * branches. Requires a compiler with -mindirect-branch=thunk-extern
		 * support for full protection. The kernel may run slower.
		 */

                apply_returns(rseg, rseg + returns->sh_size);
        }
        if (alt) {
                void *aseg              = (void *)alt->sh_addr;

                /*
		 *
		 * Replace instructions with better alternatives for this CPU type. This runs
		 * before SMP is initialized to avoid SMP problems with self modifying code.
		 * This implies that asymmetric systems where APs have less capabilities than
		 * the boot processor are not handled. Tough. Make sure you disable such
		 * features by hand.

		 * Marked "noinline" to cause control flow change and thus insn cache
		 * to refetch changed I$ lines.
 		 */

		apply_alternatives(aseg, aseg + alt->sh_size);
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

                /* defined for CONFIG_MODULES, so this is necessary to implement. */
                callthunks_patch_module_calls(&cs, mod);
        }
        if (ibt_endbr) {
                void *iseg              = (void *)ibt_endbr->sh_addr;

                /*
		 *
		 * Build the kernel with support for Indirect Branch Tracking, a
		 * hardware support course-grain forward-edge Control Flow Integrity
		 * protection. It enforces that all indirect calls must land on
		 * an ENDBR instruction, as such, the compiler will instrument the
		 * code with them to make this happen.
		 * In addition to building the kernel with IBT, seal all functions that
		 * are not indirect call targets, avoiding them ever becoming one.
		 * This requires LTO like objtool runs and will slow down the build. It
		 * does significantly reduce the number of ENDBR instructions in the
		 * kernel image.
		 */

                //apply_seal_endbr(iseg, iseg + ibt_endbr->sh_size);
        }
        if (locks) {
                void *lseg              = (void *)locks->sh_addr;
                void *text              = mod->mem[MOD_TEXT].base;
                void *text_end          = text + mod->mem[MOD_TEXT].size;

                /*
		 * This enables support for systems with more than one CPU. If you have
		 * a system with only one CPU, say N. If you have a system with more
		 * than one CPU, say Y.
		 * If you say N here, the kernel will run on uni- and multiprocessor
		 * machines, but will use only one CPU of a multiprocessor machine. If
		 * you say Y here, the kernel will run on many, but not all,
		 * uniprocessor machines. On a uniprocessor machine, the kernel
		 * will run faster if you say N here.
		 * Note that if you say Y here and choose architecture "586" or
		 * "Pentium" under "Processor family", the kernel will not work on 486
		 * architectures. Similarly, multiprocessor kernels for the "PPro"
		 * architecture may not work on all Pentium based boards.
		 * People using multiprocessor machines who say Y here should also say
		 * Y to "Enhanced Real Time Clock Support", below. The "Advanced Power
		 * Management" code will be disabled if you say Y here.
		 */

                alternatives_smp_module_add(mod, mod->name, lseg, lseg + locks->sh_size, text, text_end);
        }

        if (orc && orc_ip){
                /*
		 * This option enables the ORC (Oops Rewind Capability) unwinder for
		 * unwinding kernel stack traces.  It uses a custom data format which is
		 * a simplified version of the DWARF Call Frame Information standard.
		 * This unwinder is more accurate across interrupt entry frames than the
		 * frame pointer unwinder.  It also enables a 5-10% performance
		 * improvement across the entire kernel compared to frame pointers.
		 * Enabling this option will increase the kernel's runtime memory usage
		 * by roughly 2-4MB, depending on your kernel config.
		 */

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
		size					= 0;

                loc					= (void *)sechdrs[sechdrs[relsec].sh_info].sh_addr + rel[i].r_offset;
                sym					= (Elf(Sym) *)sechdrs[symindex].sh_addr + ELF64_R_SYM(rel[i].r_info);
                val					= sym->st_value + rel[i].r_addend;

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

#if defined(ENABLE_UNWIND_DEBUGGING) && defined(__linux__)
void unwind_backtrace(void){
	int counter = 0;
	size_t rip = 0;
	size_t rsp = 0;
	unw_context_t context;
	unw_cursor_t cursor;
	unsigned char *function_name = NULL;

	unw_getcontext(&context);
	unw_init_local(&cursor, &context);

	do {
		unw_get_reg(&cursor, UNW_X86_64_RIP, &rip);
		unw_get_reg(&cursor, UNW_X86_64_RSP, &rsp);
		fast_get_name_in_range((void *)rip, &function_name);
		/* second chance: if 'function_name' is NULL, iterate the loaded libraries for getting the symbol name */
		if(function_name == NULL){
			fast_get_name_in_so((void *)rip, &function_name);
		}

		/* for every rip value, search a function associated to it */
		printf("[%d] rip: 0x%08lx rsp: 0x%08lx -> %s\n", counter, rip, rsp, (function_name != NULL) ? function_name : "undefined");
		counter++;
		if(counter > MAX_UNWIND_ITERATIONS){
			goto end;
		}
	} while (unw_step(&cursor) > 0);
	end:
}
#endif /* ENABLE_UNWIND_DEBUGGING */
#endif /* __x86_64__ */
