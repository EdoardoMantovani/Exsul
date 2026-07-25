#ifndef __UTILS_H__
#define __UTILS_H__

#if defined(__x86_64__)
#define CONFIG_HAVE_ARCH_PREL32_RELOCATIONS	1
#elif defined(__arm64__)
#define CONFIG_HAVE_ARCH_PREL32_RELOCATIONS	1
#endif

/* due to the fact that tracking every memory allocation can cause some troubles, I decided to create a simple API which allocates memory and
 * add them to a queue present in the main context.
 */
#define MALLOC(alloc_ptr, alloc_size, alloc_flags)	alloc_ptr = malloc(alloc_size);										\
							new_allocation(ALLOCATION_TYPE_MALLOC, alloc_ptr, alloc_size, alloc_flags, director.allocation_list);

#define MMAP(alloc_ptr, alloc_size, alloc_flags)	alloc_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);		\
							new_allocation(ALLOCATION_TYPE_MMAP, alloc_ptr, alloc_size, alloc_flags, director.allocation_list);

/* empty macro used for indicating to Build.pl to add a specific symbol to the cb_table */
#define SYMBOL_EXPORT(sym)

/*
 * DECLARE_FLEX_ARRAY() - Declare a flexible array usable in a union
 *
 * @TYPE: The type of each flexible array element
 * @NAME: The name of the flexible array member
 *
 * In order to have a flexible array member in a union or alone in a
 * struct, it needs to be wrapped in an anonymous struct with at least 1
 * named member, but that member can be empty.
 */
#define DECLARE_FLEX_ARRAY(TYPE, NAME)			__DECLARE_FLEX_ARRAY(TYPE, NAME)

#ifndef SPEED_1600000
#define SPEED_1600000		1600000
#endif

#define	RB_RED			0
#define	RB_BLACK		1

#define OFFSET_TABLE_SECTION	__attribute__((section(".offset")))
#define MODULE_INPUT_MIN_LEN 	4

#define UL(x)			(_UL(x))
#define ULL(x)			(_ULL(x))
#define BIT(nr)			(UL(1) << (nr))
#define BIT_ULL(nr)		(ULL(1) << (nr))

#define BIT_MASK(nr)		(UL(1) << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)	(ULL(1) << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)	((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE		8
#define BITS_PER_TYPE(type)	(sizeof(type) * BITS_PER_BYTE)

#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr)	__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(long))
#define BITS_TO_U64(nr)		__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(u64))
#define BITS_TO_U32(nr)		__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(u32))
#define BITS_TO_BYTES(nr)	__KERNEL_DIV_ROUND_UP(nr, BITS_PER_TYPE(char))

#define _MAX(a, b)		(((a)>(b))?(a):(b))

#define ARRAY_SIZE(array)	(sizeof(array)/sizeof(array[0]))

#define __cmp_op_min <
#define __cmp_op_max >

#define __cmp(op, x, y)	((x) __cmp_op_##op (y) ? (x) : (y))
#define __cmp_once_unique(op, type, x, y, ux, uy) ({ type ux = (x); type uy = (y); __cmp(op, ux, uy); })
#define __cmp_once(op, type, x, y) __cmp_once_unique(op, type, x, y, __UNIQUE_ID(x_), __UNIQUE_ID(y_))

#define offsetof(TYPE, MEMBER)	__builtin_offsetof(TYPE, MEMBER)

#define __put_unaligned_t(type, val, ptr) do {                                          \
        struct { type x; } __packed * __put_pptr = (typeof(__put_pptr))(ptr);           \
        __put_pptr->x = (val);                                                          \
} while (0)

/* The "volatile" is due to gcc bugs */
# define barrier()		__asm__ __volatile__("": : :"memory")

/*
 * sizeof_field() - Report the size of a struct field in bytes
 *
 * @TYPE: The structure containing the field of interest
 * @MEMBER: The field to return the size of
 */
#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))

/*
 * offsetofend() - Report the offset of a struct field within the struct
 *
 * @TYPE: The type of the structure
 * @MEMBER: The member within the structure to get the end offset of
 */
#define offsetofend(TYPE, MEMBER) (offsetof(TYPE, MEMBER) + sizeof_field(TYPE, MEMBER))

/*
 * min_t - return minimum of two values, using the specified type
 * @type: data type to use
 * @x: first value
 * @y: second value
 */
#define min_t(type, x, y) __cmp_once(min, type, x, y)

/*
 * max_t - return maximum of two values, using the specified type
 * @type: data type to use
 * @x: first value
 * @y: second value
 */
#define max_t(type, x, y) __cmp_once(max, type, x, y)

/*
 * LIST_HEAD_INIT - initialize a &struct list_head's links to point to itself
 * @name: name of the list_head
 */
#define LIST_HEAD_INIT(name) { &(name), &(name) }

/*
 * LIST_HEAD - definition of a &struct list_head with initialization values
 * @name: name of the list_head
 */
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define CHECK_DATA_CORRUPTION(condition, addr, fmt, ...)		 \
	check_data_corruption(({					 \
		bool corruption = unlikely(condition);			 \
		corruption;						 \
	}))

#define MAX_JIFFY_OFFSET ((LONG_MAX >> 1)-1)

#define IS_ERR_VALUE(x) unlikely((unsigned long)(void *)(x) >= (unsigned long)-MAX_ERRNO)

/* NOTE:  these are not the standard USB_ENDPOINT_XFER_* values!! */
/* (yet ... they're the values used by usbfs) */
#define PIPE_ISOCHRONOUS			0
#define PIPE_INTERRUPT				1
#define PIPE_CONTROL				2
#define PIPE_BULK				3

#define NULLS_MARKER(value)			(1UL | (((long)value) << 1))
#define INIT_HLIST_NULLS_HEAD(ptr, nulls) 	((ptr)->first = (struct hlist_nulls_node *) NULLS_MARKER(nulls))
#define HLIST_NULLS_HEAD_INIT(nulls) 		{.first = (struct hlist_nulls_node *)NULLS_MARKER(nulls)}

#define DECLARE_BITMAP(name,bits)		unsigned long name[BITS_TO_LONGS(bits)]
#define __ETHTOOL_DECLARE_LINK_MODE_MASK(name)	DECLARE_BITMAP(name, __ETHTOOL_LINK_MODE_MASK_NBITS)

#define alloc_ordered_workqueue(fmt, flags, args)			\
	alloc_workqueue(fmt, WQ_UNBOUND | __WQ_ORDERED | (flags), 1, args)

#define create_workqueue(name)						\
	alloc_workqueue("%s", __WQ_LEGACY | WQ_MEM_RECLAIM | WQ_PERCPU, 1, (name))

#define create_freezable_workqueue(name)				\
	alloc_workqueue("%s", __WQ_LEGACY | WQ_FREEZABLE | WQ_UNBOUND |	\
			WQ_MEM_RECLAIM, 1, (name))
#define create_singlethread_workqueue(name)				\
	alloc_ordered_workqueue("%s", __WQ_LEGACY | WQ_MEM_RECLAIM, name)

#define from_work(var, callback_work, work_fieldname)	\
	container_of(callback_work, typeof(*var), work_fieldname)

/*
 * small_const_nbits(n) is true precisely when it is known at compile-time
 * that BITMAP_SIZE(n) is 1, i.e. 1 <= n <= BITS_PER_LONG. This allows
 * various bit/bitmap APIs to provide a fast inline implementation. Bitmaps
 * of size 0 are very rare, and a compile-time-known-size 0 is most likely
 * a sign of error. They will be handled correctly by the bit/bitmap APIs,
 * but using the out-of-line functions, so that the inline implementations
 * can unconditionally dereference the pointer(s).
 */
#define small_const_nbits(nbits) (__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG && (nbits) > 0)
#define hlist_nulls_entry(ptr, type, member) container_of(ptr,type,member)

#define hlist_nulls_entry_safe(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   !is_a_nulls(____ptr) ? hlist_nulls_entry(____ptr, type, member) : NULL; \
	})

#define timer_container_of(var, callback_timer, timer_fieldname)	\
container_of(callback_timer, typeof(*var), timer_fieldname)

/*
 * NULLS_MARKER() expects a hash value with the low
 * bits mostly likely to be significant, and it discards
 * the msb.
 * We give it an address, in which the bottom bit is
 * always 0, and the msb might be significant.
 * So we shift the address down one bit to align with
 * expectations and avoid losing a significant bit.
 *
 * We never store the NULLS_MARKER in the hash table
 * itself as we need the lsb for locking.
 * Instead we store a NULL
 */
#define	RHT_NULLS_MARKER(ptr)		((void *)NULLS_MARKER(((unsigned long) (ptr)) >> 1))
#define INIT_RHT_NULLS_HEAD(ptr)	((ptr) = NULL)

/*
 *       even if it isn't "correct", let's simplify 'usleep_range' in this way
 */
#define usleep_range(min_sleep, max_sleep)	usleep((min_sleep + max_sleep) / 2)

#define ms_to_us(ms)				((useconds_t)((ms) * 1000))
#define udelay(us)				usleep(us)
#define prep_zero_mask(a, bits, c)		(bits)
#define fls(x)					generic_fls(x)
#define __fls(word)				generic___fls(word)
#define find_zero(bits)				(__ffs(bits) >> 3)

#define copy_from_user				_copy_from_user
#define copy_to_user				_copy_to_user

#define linkmode_test_bit			test_bit
#define linkmode_set_bit			__set_bit
#define linkmode_clear_bit			__clear_bit
#define linkmode_mod_bit			__assign_bit

/* pr_xxx functions */
#define pr_fmt(fmt)        	fmt
#define pr_notice(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_debug(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...)	printk(pr_fmt(fmt), ##__VA_ARGS__)

#define kobj_to_dev(__kobj)	container_of_const(__kobj, struct device, kobj)

/* define the function attributes macro in the linux kernel as empty macro here */
#define __must_check
#define __signed_wrap
#define __fix_address
#define __kernel
#define __user
#define __iomem
#define __percpu
#define __rcu

#define __packed __attribute__((packed))
#define __force __attribute__((force))
#define __aligned(x) __attribute__((aligned(x)))
#define fallthrough __attribute__((fallthrough))

/*
 * Optional: only supported since gcc >= 15
 * Optional: only supported since clang >= 18
 *
 *   gcc: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=108896
 * clang: https://github.com/llvm/llvm-project/pull/76348
 *
 * __bdos on clang < 19.1.2 can erroneously return 0:
 * https://github.com/llvm/llvm-project/pull/110497
 *
 * __bdos on clang < 19.1.3 can be off by 4:
 * https://github.com/llvm/llvm-project/pull/112636
 */
#ifdef CONFIG_CC_HAS_COUNTED_BY
#define __counted_by(member)		__attribute__((__counted_by__(member)))
#else
#define __counted_by(member)
#endif

#ifndef cpu_to_be16
#define cpu_to_be16 __cpu_to_be16
#endif

#ifndef cpu_to_le16
#define cpu_to_le16 __cpu_to_le16
#endif

#ifndef le16_to_cpu
#define le16_to_cpu __le16_to_cpu
#endif

#ifndef le32_to_cpu
#define le32_to_cpu __le32_to_cpu
#endif

#ifndef cpu_to_le32
#define cpu_to_le32 __cpu_to_le32
#endif

#ifndef be32_to_cpu
#define be32_to_cpu __be32_to_cpu
#endif

#ifndef le64_to_cpu
#define le64_to_cpu __le64_to_cpu
#endif

/*
 * do_div - returns 2 values: calculate remainder and update new dividend
 * @n: uint64_t dividend (will be updated)
 * @base: uint32_t divisor
 *
 * Summary:
 * ``uint32_t remainder = n % base;``
 * ``n = n / base;``
 *
 * Return: (uint32_t)remainder
 *
 * NOTE: macro parameter @n is evaluated multiple times,
 * beware of side effects!
 */
# define do_div(n,base) ({					\
	uint32_t __base = (base);				\
	uint32_t __rem;						\
	__rem = ((uint64_t)(n)) % __base;			\
	(n) = ((uint64_t)(n)) / __base;				\
	__rem;							\
 })

#ifndef htonl
#if __BYTE_ORDER == __BIG_ENDIAN
#define htonl(x) x
#else
#define htonl(x) __builtin_bswap32(x)
#endif
#endif

#define rb_parent(r)   ((struct rb_node *)((r)->__rb_parent_color & ~3))
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)
#define RB_ROOT (struct rb_root) { NULL, }
#define RB_ROOT_CACHED (struct rb_root_cached) { {NULL, }, NULL }

#define __rb_color(pc)     ((pc) & 1)
#define __rb_is_black(pc)  __rb_color(pc)
#define __rb_is_red(pc)    (!__rb_color(pc))
#define rb_color(rb)       __rb_color((rb)->__rb_parent_color)
#define rb_is_red(rb)      __rb_is_red((rb)->__rb_parent_color)
#define rb_is_black(rb)    __rb_is_black((rb)->__rb_parent_color)

#define rcu_assign_pointer(p, v)					      \
do {									      \
	uintptr_t _r_a_p__v = (uintptr_t)(v);				      \
									      \
	if (!__builtin_constant_p(v) || (_r_a_p__v) != (uintptr_t)NULL)      \
		WRITE_ONCE((p), (typeof(p))(_r_a_p__v));		      \
	else								      \
		WRITE_ONCE((p), (typeof(p))(uintptr_t)NULL);		      \
} while (0)

/*
 * rcu_dereference_all() - fetch RCU-all-protected pointer for dereferencing
 * @p: The pointer to read, prior to dereferencing
 *
 * Makes rcu_dereference_check() do the dirty work.
 */
#define rcu_dereference_all(p) rcu_dereference_all_check(p, 0)

/*
 * rcu_access_pointer() - fetch RCU pointer with no dereferencing
 * @p: The pointer to read
 *
 * Return the value of the specified RCU-protected pointer, but omit the
 * lockdep checks for being in an RCU read-side critical section.  This is
 * useful when the value of this pointer is accessed, but the pointer is
 * not dereferenced, for example, when testing an RCU-protected pointer
 * against NULL.  Although rcu_access_pointer() may also be used in cases
 * where update-side locks prevent the value of the pointer from changing,
 * you should instead use rcu_dereference_protected() for this use case.
 * Within an RCU read-side critical section, there is little reason to
 * use rcu_access_pointer().
 *
 * It is usually best to test the rcu_access_pointer() return value
 * directly in order to avoid accidental dereferences being introduced
 * by later inattentive changes.  In other words, assigning the
 * rcu_access_pointer() return value to a local variable results in an
 * accident waiting to happen.
 *
 * It is also permissible to use rcu_access_pointer() when read-side
 * access to the pointer was removed at least one grace period ago, as is
 * the case in the context of the RCU callback that is freeing up the data,
 * or after a synchronize_rcu() returns.  This can be useful when tearing
 * down multi-linked structures after a grace period has elapsed.  However,
 * rcu_dereference_protected() is normally preferred for this use case.
 */
#define rcu_access_pointer(p) __rcu_access_pointer((p), __UNIQUE_ID(rcu), __rcu)

/*
 * Prefer C11 _Generic for better compile-times and simpler code. Note: 'char'
 * is not type-compatible with 'signed char', and we define a separate case.
 */
#define __scalar_type_to_expr_cases(type)				\
		unsigned type:	(unsigned type)0,			\
		signed type:	(signed type)0

#define __unqual_scalar_typeof(x) typeof(				\
		_Generic((x),						\
			 char:	(char)0,				\
			 __scalar_type_to_expr_cases(char),		\
			 __scalar_type_to_expr_cases(short),		\
			 __scalar_type_to_expr_cases(int),		\
			 __scalar_type_to_expr_cases(long),		\
			 __scalar_type_to_expr_cases(long long),	\
			 default: (x)))

#ifndef __READ_ONCE
#define __READ_ONCE(x)	(*(const volatile __unqual_scalar_typeof(x) *)&(x))
#endif

#define READ_ONCE(x)							\
({									\
	__READ_ONCE(x);							\
})

#define __WRITE_ONCE(x, val)						\
do {									\
	*(volatile typeof(x) *)&(x) = (val);				\
} while (0)

#define WRITE_ONCE(x, val)						\
do {									\
	__WRITE_ONCE(x, val);						\
} while (0)

#define container_of(ptr, type, member)                                        \
        ({                                                                     \
                const typeof(((type *)0)->member) *__mptr = ((void *)(ptr));   \
                (type *)((char *)__mptr - offsetof(type, member));             \
        })

/*
 * container_of_const - cast a member of a structure out to the containing
 *			structure and preserve the const-ness of the pointer
 * @ptr:		the pointer to the member
 * @type:		the type of the container struct this is embedded in.
 * @member:		the name of the member within the struct.
 *
 * Always prefer container_of_const() instead of container_of() in new code.
 */
#define container_of_const(ptr, type, member)				\
	_Generic(ptr,							\
		const typeof(*(ptr)) *: ((const type *)container_of(ptr, type, member)),\
		default: ((type *)container_of(ptr, type, member))	\
	)

#define __branch_check__(x, expect, is_constant)                               \
        ({                                                                     \
                long ______r;                                                  \
                ______r = __builtin_expect(!!(x), expect);                     \
                ______r;                                                       \
        })

/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#define typecheck(type,x) \
({	type __dummy; \
	typeof(x) __dummy2; \
	(void)(&__dummy == &__dummy2); \
	1; \
})

/*
 * Check at compile time that 'function' is a certain type, or is a pointer
 * to that type (needs to use typedef for the function type.)
 */
#define typecheck_fn(type,function) \
({	typeof(type) __tmp = function; \
	(void)__tmp; \
})

/*
 * Check at compile time that something is a pointer type.
 */
#define typecheck_pointer(x) \
({	typeof(x) __dummy; \
	(void)sizeof(*__dummy); \
	1; \
})

/*
 * time_after - returns true if the time a is after time b.
 * @a: first comparable as unsigned long
 * @b: second comparable as unsigned long
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 *
 * Return: %true is time a is after time b, otherwise %false.
 */
#define time_after(a,b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)((b) - (a)) < 0))
/*
 * time_before - returns true if the time a is before time b.
 * @a: first comparable as unsigned long
 * @b: second comparable as unsigned long
 *
 * Return: %true is time a is before time b, otherwise %false.
 */
#define time_before(a,b)	time_after(b,a)

#ifndef likely
#define likely(x) (__branch_check__(x, 1, __builtin_constant_p(x)))
#endif

#ifndef unlikely
#define unlikely(x) (__branch_check__(x, 0, __builtin_constant_p(x)))
#endif

#define __get_unaligned_t(type, ptr) ({                                                         \
        const struct { type x; } __attribute__((packed)) *__pptr = (typeof(__pptr))(ptr);       \
        __pptr->x;                                                                              \
})

#define REPEAT_BYTE(x)		((~0ul / 0xff) * (x))
#define WORD_AT_A_TIME_CONSTANTS { REPEAT_BYTE(0x01), REPEAT_BYTE(0x80) }

#define hex_asc_lo(x)		hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)		hex_asc[((x) & 0xf0) >> 4]

#define get_unaligned(ptr)      __get_unaligned_t(typeof(*(ptr)), (ptr))
#define __get_next(t, insn)     ({ t r = get_unaligned((t *)(insn)->next_byte); (insn)->next_byte += sizeof(t); leXX_to_cpu(t, r); })

#define validate_next(t, insn, n) ((insn)->next_byte + sizeof(t) + n <= (insn)->end_kaddr)
#define get_next(t, insn)       ({ if (unlikely(!validate_next(t, insn, 0))) goto err_out; __get_next(t, insn); })

/* element iteration helpers */
#define for_each_element(_elem, _data, _datalen)			\
	for (_elem = (const struct element *)(_data);			\
	     (const u8 *)(_data) + (_datalen) - (const u8 *)_elem >=	\
		(int)sizeof(*_elem) &&					\
	     (const u8 *)(_data) + (_datalen) - (const u8 *)_elem >=	\
		(int)sizeof(*_elem) + _elem->datalen;			\
	     _elem = (const struct element *)(_elem->data + _elem->datalen))

#define for_each_element_id(element, _id, data, datalen)		\
	for_each_element(element, data, datalen)			\
		if (element->id == (_id))

#define mod_mem_type_is_data(type) (!mod_mem_type_is_text(type))

#define for_each_mod_mem_type(type)                     \
        for (enum mod_mem_type (type) = 0;              \
             (type) < MOD_MEM_NUM_TYPES; (type)++)

#define for_class_mod_mem_type(type, class)             \
        for_each_mod_mem_type(type)                     \
                if (mod_mem_type_is_##class(type))

#define mod_mem_type_is_core_data(type)                 \
        (mod_mem_type_is_core(type) &&                  \
         mod_mem_type_is_data(type))

#define mod_mem_type_is_init(type)      \
        ((type) == MOD_INIT_TEXT ||     \
         (type) == MOD_INIT_DATA ||     \
         (type) == MOD_INIT_RODATA)

#define mod_mem_type_is_core(type) (!mod_mem_type_is_init(type))

#define mod_mem_type_is_text(type)      \
         ((type) == MOD_TEXT ||         \
          (type) == MOD_INIT_TEXT)

#ifdef __WIN64__
#define PAGE_ALIGN(addr)	ALIGN(addr, 4096)
#else
#define PAGE_ALIGN(addr)        ALIGN(addr, sysconf(_SC_PAGE_SIZE))
#endif
#define ALIGN(x, a)             __ALIGN_KERNEL((x), (a))
#define SH_ENTSIZE_TYPE_BITS    4
#define SH_ENTSIZE_TYPE_SHIFT   (BITS_PER_LONG - SH_ENTSIZE_TYPE_BITS)
#define SH_ENTSIZE_TYPE_MASK    ((1ULL << SH_ENTSIZE_TYPE_BITS) - 1ULL)
#define SH_ENTSIZE_OFFSET_MASK  ((1ULL << (BITS_PER_LONG - SH_ENTSIZE_TYPE_BITS)) - 1ULL)

#define NSEC_PER_SEC 1000000000ULL
#define USEC_PER_SEC 1000000ULL
#define MSEC_PER_SEC 1000ULL

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))

#define bitmap_size(nbits)	(ALIGN(nbits, BITS_PER_LONG) / BITS_PER_BYTE)

/*
 * Common helper for find_bit() function family
 * @FETCH: The expression that fetches and pre-processes each word of bitmap(s)
 * @MUNGE: The expression that post-processes a word containing found bit (may be empty)
 * @size: The bitmap size in bits
 */
#define FIND_FIRST_BIT(FETCH, MUNGE, size)					\
({										\
	unsigned long idx, val, sz = (size);					\
										\
	for (idx = 0; idx * BITS_PER_LONG < sz; idx++) {			\
		val = (FETCH);							\
		if (val) {							\
			sz = min(idx * BITS_PER_LONG + __ffs(MUNGE(val)), sz);	\
			break;							\
		}								\
	}									\
										\
	sz;									\
})

/*
 * Common helper for find_next_bit() function family
 * @FETCH: The expression that fetches and pre-processes each word of bitmap(s)
 * @MUNGE: The expression that post-processes a word containing found bit (may be empty)
 * @size: The bitmap size in bits
 * @start: The bitnumber to start searching at
 */
#define FIND_NEXT_BIT(FETCH, MUNGE, size, start)				\
({										\
	unsigned long mask, idx, tmp, sz = (size), __start = (start);		\
										\
	if (unlikely(__start >= sz))						\
		goto out;							\
										\
	mask = MUNGE(BITMAP_FIRST_WORD_MASK(__start));				\
	idx = __start / BITS_PER_LONG;						\
										\
	for (tmp = (FETCH) & mask; !tmp; tmp = (FETCH)) {			\
		if ((idx + 1) * BITS_PER_LONG >= sz)				\
			goto out;						\
		idx++;								\
	}									\
										\
	sz = min(idx * BITS_PER_LONG + __ffs(MUNGE(tmp)), sz);			\
out:										\
	sz;									\
})

#define FIND_NTH_BIT(FETCH, size, num)						\
({										\
	unsigned long sz = (size), nr = (num), idx, w, tmp;			\
										\
	for (idx = 0; (idx + 1) * BITS_PER_LONG <= sz; idx++) {			\
		if (idx * BITS_PER_LONG + nr >= sz)				\
			goto out;						\
										\
		tmp = (FETCH);							\
		w = hweight_long(tmp);						\
		if (w > nr)							\
			goto found;						\
										\
		nr -= w;							\
	}									\
										\
	if (sz % BITS_PER_LONG)							\
		tmp = (FETCH) & BITMAP_LAST_WORD_MASK(sz);			\
found:										\
	sz = idx * BITS_PER_LONG + fns(tmp, nr);				\
out:										\
	sz;									\
})

#ifndef PAGE_SHIFT
#define PAGE_SHIFT 12
#endif

#undef  SHF_WRITE
#undef  SHF_ALLOC
#undef  SHF_EXECINSTR

#define SHF_WRITE               0x1
#define SHF_ALLOC               0x2
#define SHF_EXECINSTR           0x4
#define SHF_RELA_LIVEPATCH      0x00100000
#define SHF_RO_AFTER_INIT       0x00200000
#define SHF_MASKPROC            0xf0000000

#undef ELF_ST_BIND
#undef ELF_ST_TYPE
#undef ELF32_ST_BIND
#undef ELF32_ST_TYPE
#undef ELF64_ST_BIND
#undef ELF64_ST_TYPE

#define ELF_ST_BIND(x)          ((x) >> 4)
#define ELF_ST_TYPE(x)          ((x) & 0xf)
#define ELF32_ST_BIND(x)        ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x)        ELF_ST_TYPE(x)
#define ELF64_ST_BIND(x)        ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x)        ELF_ST_TYPE(x)

/*
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/*
 * list_entry_rcu - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_head within the struct.
 *
 * This primitive may safely run concurrently with the _rcu list-mutation
 * primitives such as list_add_rcu() as long as it's guarded by rcu_read_lock().
 */
#define list_entry_rcu(ptr, type, member) \
	container_of(READ_ONCE(ptr), type, member)

/*
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/*
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

/*
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) ({ \
	struct list_head *head__ = (ptr); \
	struct list_head *pos__ = READ_ONCE(head__->next); \
	pos__ != head__ ? list_entry(pos__, type, member) : NULL; \
})

/*
 * list_last_entry_or_null - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_last_entry_or_null(ptr, type, member) ({ \
	struct list_head *head__ = (ptr); \
	struct list_head *pos__ = READ_ONCE(head__->prev); \
	pos__ != head__ ? list_entry(pos__, type, member) : NULL; \
})

/*
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/*
 * list_next_entry_circular - get the next element in list
 * @pos:	the type * to cursor.
 * @head:	the list head to take the element from.
 * @member:	the name of the list_head within the struct.
 *
 * Wraparound if pos is the last element (return the first element).
 * Note, that list is expected to be not empty.
 */
#define list_next_entry_circular(pos, head, member) \
	(list_is_last(&(pos)->member, head) ? \
	list_first_entry(head, typeof(*(pos)), member) : list_next_entry(pos, member))

/*
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

/*
 * list_prev_entry_circular - get the prev element in list
 * @pos:	the type * to cursor.
 * @head:	the list head to take the element from.
 * @member:	the name of the list_head within the struct.
 *
 * Wraparound if pos is the first element (return the last element).
 * Note, that list is expected to be not empty.
 */
#define list_prev_entry_circular(pos, head, member) \
	(list_is_first(&(pos)->member, head) ? \
	list_last_entry(head, typeof(*(pos)), member) : list_prev_entry(pos, member))

/*
 * list_entry_is_head - test if the entry points to the head of the list
 * @pos:	the type * to cursor
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_entry_is_head(pos, head, member)				\
	list_is_head(&pos->member, (head))

/*
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; !list_is_head(pos, (head)); pos = pos->next)

/*
 * list_for_each_continue - continue iteration over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * Continue to iterate over a list, continuing after the current position.
 */
#define list_for_each_continue(pos, head) \
	for (pos = pos->next; !list_is_head(pos, (head)); pos = pos->next)

/*
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; !list_is_head(pos, (head)); pos = pos->prev)

/*
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; \
	     !list_is_head(pos, (head)); \
	     pos = n, n = pos->next)

/*
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     !list_is_head(pos, (head)); \
	     pos = n, n = pos->prev)

/*
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     !list_entry_is_head(pos, head, member);			\
	     pos = list_next_entry(pos, member))

/*
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_first_entry(head, typeof(*pos), member),	\
		n = list_next_entry(pos, member);			\
	     !list_entry_is_head(pos, head, member); 			\
	     pos = n, n = list_next_entry(n, member))

/*
 * list_for_each_entry_rcu	-	iterate over rcu list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 * @cond:	optional lockdep expression if called from non-RCU protection.
 *
 * This list-traversal primitive may safely run concurrently with
 * the _rcu list-mutation primitives such as list_add_rcu()
 * as long as the traversal is guarded by rcu_read_lock().
 */
#define list_for_each_entry_rcu(pos, head, member, cond...)		\
	for (pos = list_entry_rcu((head)->next, typeof(*pos), member);	\
		&pos->member != (head);					\
		pos = list_entry_rcu(pos->member.next, typeof(*pos), member))

#define rht_dereference(p, ht) \
	rcu_dereference_protected(p, lockdep_rht_mutex_is_held(ht))

#define rht_dereference_rcu(p, ht) \
	rcu_dereference_all_check(p, lockdep_rht_mutex_is_held(ht))

#define rht_dereference_bucket(p, tbl, hash) \
	rcu_dereference_protected(p, lockdep_rht_bucket_is_held(tbl, hash))

#define rht_dereference_bucket_rcu(p, tbl, hash) \
	rcu_dereference_all_check(p, lockdep_rht_bucket_is_held(tbl, hash))

#define rht_entry(tpos, pos, member) \
	({ tpos = container_of(pos, typeof(*tpos), member); 1; })

/*
 * rhl_for_each_rcu - iterate over rcu hash table list
 * @pos:	the &struct rlist_head to use as a loop cursor.
 * @list:	the head of the list
 *
 * This hash chain list-traversal primitive should be used on the
 * list returned by rhltable_lookup.
 */
#define rhl_for_each_rcu(pos, list)					\
	for (pos = list; pos; pos = rcu_dereference_all(pos->next))

/*
 * rhl_for_each_entry_rcu - iterate over rcu hash table list of given type
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct rlist_head to use as a loop cursor.
 * @list:	the head of the list
 * @member:	name of the &struct rlist_head within the hashable struct.
 *
 * This hash chain list-traversal primitive should be used on the
 * list returned by rhltable_lookup.
 */
#define rhl_for_each_entry_rcu(tpos, pos, list, member)			\
	for (pos = list; pos && rht_entry(tpos, pos, member);		\
	     pos = rcu_dereference_all(pos->next))

#define struct_group_tagged(TAG, NAME, MEMBERS...)                             \
        __struct_group(TAG, NAME, /* no attrs */, MEMBERS)
#define struct_group(NAME, MEMBERS...)                                         \
        __struct_group(/* no tag */, NAME, /* no attrs */, MEMBERS)

#define __scalar_type_to_expr_cases(type)                               		\
                unsigned type:  (unsigned type)0,                       		\
                signed type:    (signed type)0

#define __unqual_scalar_typeof(x) typeof(                               		\
                _Generic((x),                                           		\
                         char:  (char)0,                                		\
                         __scalar_type_to_expr_cases(char),             		\
                         __scalar_type_to_expr_cases(short),            		\
                         __scalar_type_to_expr_cases(int),              		\
                         __scalar_type_to_expr_cases(long),             		\
                         __scalar_type_to_expr_cases(long long),        		\
                         default: (x)))

#ifndef __READ_ONCE
        #define __READ_ONCE(x)  (*(const volatile __unqual_scalar_typeof(x) *)&(x))
#endif

#define READ_ONCE(x)                                                    		\
({                                                                      		\
        __READ_ONCE(x);                                                 		\
})

#define ___PASTE(a,b) 			a##b
#define __PASTE(a,b) 			___PASTE(a,b)

#define __UNIQUE_ID(prefix) 		__PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#define rcu_check_sparse(p, space)
#define RCU_INITIALIZER(v)		(typeof(*(v)) __force __rcu *)(v)
#define __rcu_dereference_check(p, local, c, space) \
({ \
	/* Dependency order vs. p above. */ \
	typeof(*p) *local = (typeof(*p) *__force)READ_ONCE(p); \
	rcu_check_sparse(p, space); \
	((typeof(*p) __force __kernel *)(local)); \
})
#define __rcu_access_pointer(p, local, space) \
({ \
	typeof(*p) *local = (typeof(*p) *__force)READ_ONCE(p); \
	rcu_check_sparse(p, space); \
	((typeof(*p) __force __kernel *)(local)); \
})
#define __rcu_dereference_protected(p, local, c, space) \
({ \
	rcu_check_sparse(p, space); \
	((typeof(*p) __force __kernel *)(p)); \
})

/*
 * RCU_INIT_POINTER() - initialize an RCU protected pointer
 * @p: The pointer to be initialized.
 * @v: The value to initialized the pointer to.
 *
 * Initialize an RCU-protected pointer in special cases where readers
 * do not need ordering constraints on the CPU or the compiler.  These
 * special cases are:
 *
 * 1.	This use of RCU_INIT_POINTER() is NULLing out the pointer *or*
 * 2.	The caller has taken whatever steps are required to prevent
 *	RCU readers from concurrently accessing this pointer *or*
 * 3.	The referenced data structure has already been exposed to
 *	readers either at compile time or via rcu_assign_pointer() *and*
 *
 *	a.	You have not made *any* reader-visible changes to
 *		this structure since then *or*
 *	b.	It is OK for readers accessing this structure from its
 *		new location to see the old state of the structure.  (For
 *		example, the changes were to statistical counters or to
 *		other state where exact synchronization is not required.)
 *
 * Failure to follow these rules governing use of RCU_INIT_POINTER() will
 * result in impossible-to-diagnose memory corruption.  As in the structures
 * will look OK in crash dumps, but any concurrent RCU readers might
 * see pre-initialized values of the referenced data structure.  So
 * please be very careful how you use RCU_INIT_POINTER()!!!
 *
 * If you are creating an RCU-protected linked structure that is accessed
 * by a single external-to-structure RCU-protected pointer, then you may
 * use RCU_INIT_POINTER() to initialize the internal RCU-protected
 * pointers, but you must use rcu_assign_pointer() to initialize the
 * external-to-structure pointer *after* you have completely initialized
 * the reader-accessible portions of the linked structure.
 *
 * Note that unlike rcu_assign_pointer(), RCU_INIT_POINTER() provides no
 * ordering guarantees for either the CPU or the compiler.
 */
#define RCU_INIT_POINTER(p, v) \
	do { \
		rcu_check_sparse(p, __rcu); \
		WRITE_ONCE(p, RCU_INITIALIZER(v)); \
	} while (0)

#define rcu_dereference_check(p, c) 	__rcu_dereference_check((p), __UNIQUE_ID(rcu), c || 0, __rcu)

#define rcu_dereference(p)		rcu_dereference_check(p, 0)
#define rcu_dereference_protected(p, c)	__rcu_dereference_protected((p), __UNIQUE_ID(rcu), (c), __rcu)

#define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _n, X...) _n

#define COUNT_ARGS(X...) 		__COUNT_ARGS(, ##X, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define CONCATENATE(a, b)		__CONCAT(a, b)

#ifdef __WIN64__
#define strscpy(dst, src, ...)		exsul_sized_strscpy(dst, src, __VA_ARGS__)
#else
#define strscpy(dst, src, ...)		CONCATENATE(__strscpy, COUNT_ARGS(__VA_ARGS__))(dst, src, __VA_ARGS__)
#endif

#define __strscpy1(dst, src, size)	exsul_sized_strscpy(dst, src, size)

#define tolower(c)			__tolower(c)
#define toupper(c)			__toupper(c)

#define __ismask(x)			(_ctype[(int)(unsigned char)(x)])

#define isalnum(c)			((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)			((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)			((__ismask(c)&(_C)) != 0)

#define isgraph(c)			((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)			((__ismask(c)&(_L)) != 0)
#define isprint(c)			((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)			((__ismask(c)&(_P)) != 0)

/* Note: isspace() must return false for %NUL-terminator */
#define isspace(c)			((__ismask(c)&(0x20)) != 0)
#define isupper(c)			((__ismask(c)&(_U)) != 0)
#define isxdigit(c)			((__ismask(c)&(_D|_X)) != 0)

#ifdef ENABLE_UNWIND_DEBUGGING
#undef _U
#undef _L
#undef _D
#undef _C
#undef _P
#undef _S
#undef _X
#undef _SP
#endif

#define _U				0x01	/* upper */
#define _L				0x02	/* lower */
#define _D				0x04	/* digit */
#define _C				0x08	/* cntrl */
#define _P				0x10	/* punct */
#define _S				0x20	/* white space (space/lf/tab) */
#define _X				0x40	/* hex digit */
#define _SP				0x80	/* hard space (0x20) */

/*
 * roundup_pow_of_two - round the given value up to nearest power of two
 * @n: parameter
 *
 * round the given value up to the nearest power of two
 * - the result is undefined when n == 0
 * - this can be used to initialise global variables from constant data
 */
#define roundup_pow_of_two(n)			\
(						\
	__builtin_constant_p(n) ? (		\
		((n) == 1) ? 1 :		\
		(1UL << (ilog2((n) - 1) + 1))	\
				   ) :		\
	__roundup_pow_of_two(n)			\
 )

/*
 * rounddown_pow_of_two - round the given value down to nearest power of two
 * @n: parameter
 *
 * round the given value down to the nearest power of two
 * - the result is undefined when n == 0
 * - this can be used to initialise global variables from constant data
 */
#define rounddown_pow_of_two(n)			\
(						\
	__builtin_constant_p(n) ? (		\
		(1UL << ilog2(n))) :		\
	__rounddown_pow_of_two(n)		\
 )

#ifndef ARCH_INDEPENDENT_BITOPS
/*
 * Many architecture-specific non-atomic bitops contain inline asm code and due
 * to that the compiler can't optimize them to compile-time expressions or
 * constants. In contrary, generic_*() helpers are defined in pure C and
 * compilers optimize them just well.
 * Therefore, to make `unsigned long foo = 0; __set_bit(BAR, &foo)` effectively
 * equal to `unsigned long foo = BIT(BAR)`, pick the generic C alternative when
 * the arguments can be resolved at compile time. That expression itself is a
 * constant and doesn't bring any functional changes to the rest of cases.
 * The casts to `uintptr_t` are needed to mitigate `-Waddress` warnings when
 * passing a bitmap from .bss or .data (-> `!!addr` is always true).
 */
#define bitop(op, nr, addr)						\
	((__builtin_constant_p(nr) &&					\
	  __builtin_constant_p((uintptr_t)(addr) != (uintptr_t)NULL) &&	\
	  (uintptr_t)(addr) != (uintptr_t)NULL &&			\
	  __builtin_constant_p(*(const unsigned long *)(addr))) ?	\
	 const##op(nr, addr) : op(nr, addr))

/*
 * The following macros are non-atomic versions of their non-underscored
 * counterparts.
 */
#define __set_bit(nr, addr)		bitop(___set_bit, nr, addr)
#define __clear_bit(nr, addr)		bitop(___clear_bit, nr, addr)
#define __change_bit(nr, addr)		bitop(___change_bit, nr, addr)
#define __test_and_set_bit(nr, addr)	bitop(___test_and_set_bit, nr, addr)
#define __test_and_clear_bit(nr, addr)	bitop(___test_and_clear_bit, nr, addr)
#define __test_and_change_bit(nr, addr)	bitop(___test_and_change_bit, nr, addr)

#define test_bit(nr, addr)		bitop(_test_bit, nr, addr)
#define test_bit_acquire(nr, addr)	bitop(_test_bit_acquire, nr, addr)

#endif

/* Nanosecond scalar representation for kernel time values */
typedef s64	ktime_t;

/*
 * The type used for indexing onto a disc or disc partition.
 *
 * Linux always considers sectors to be 512 bytes long independently
 * of the devices real block size.
 *
 * blkcnt_t is the type of the inode's block count.
 */
typedef u64 sector_t;

typedef struct refcount_struct {
	atomic_uint refs;
}refcount_t;

struct kref {
	refcount_t refcount;
};

enum {
	LINK_CAPA_10HD,
	LINK_CAPA_10FD,
	LINK_CAPA_100HD,
	LINK_CAPA_100FD,
	LINK_CAPA_1000HD,
	LINK_CAPA_1000FD,
	LINK_CAPA_2500FD,
	LINK_CAPA_5000FD,
	LINK_CAPA_10000FD,
	LINK_CAPA_20000FD,
	LINK_CAPA_25000FD,
	LINK_CAPA_40000FD,
	LINK_CAPA_50000FD,
	LINK_CAPA_56000FD,
	LINK_CAPA_100000FD,
	LINK_CAPA_200000FD,
	LINK_CAPA_400000FD,
	LINK_CAPA_800000FD,
	LINK_CAPA_1600000FD,

	__LINK_CAPA_MAX,
};

static const unsigned char _ctype[] = {
_C,_C,_C,_C,_C,_C,_C,_C,				/* 0-7 */
_C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,			/* 8-15 */
_C,_C,_C,_C,_C,_C,_C,_C,				/* 16-23 */
_C,_C,_C,_C,_C,_C,_C,_C,				/* 24-31 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,				/* 32-39 */
_P,_P,_P,_P,_P,_P,_P,_P,				/* 40-47 */
_D,_D,_D,_D,_D,_D,_D,_D,				/* 48-55 */
_D,_D,_P,_P,_P,_P,_P,_P,				/* 56-63 */
_P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,		/* 64-71 */
_U,_U,_U,_U,_U,_U,_U,_U,				/* 72-79 */
_U,_U,_U,_U,_U,_U,_U,_U,				/* 80-87 */
_U,_U,_U,_P,_P,_P,_P,_P,				/* 88-95 */
_P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,		/* 96-103 */
_L,_L,_L,_L,_L,_L,_L,_L,				/* 104-111 */
_L,_L,_L,_L,_L,_L,_L,_L,				/* 112-119 */
_L,_L,_L,_P,_P,_P,_P,_C,				/* 120-127 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,			/* 128-143 */
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,			/* 144-159 */
_S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,	/* 160-175 */
_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,	/* 176-191 */
_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,	/* 192-207 */
_U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,	/* 208-223 */
_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,	/* 224-239 */
_L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};	/* 240-255 */

static const char hex_asc[] = "0123456789abcdef";

struct word_at_a_time {
	const unsigned long one_bits;
	const unsigned long high_bits;
};

/* allocation list used for mantaining the track of the memory allocated during the module runtime loading/execution */
struct allocation_list {
	u8 alloc_type;
	u8 alloc_flags;
	u8 alloc_inmem_type;
	int alloc_len;
	void *alloc_ptr;
	struct allocation_list *alloc_next;
};

/* section for the crash dump file */
struct crashdump_section {
	int start_offset;
	int length;
};

static inline bool check_data_corruption(bool v){
	return v;
}

#ifdef ARCH_INDEPENDENT_BITOPS
/* functions taken from 'https://github.com/chris1111/AppleIntelE1000e/blob/0604bc4bdfb1a12be7af6b7fe2ce5b9ad8582310/kcompat.h#L423-L440C2' */
static inline int test_bit(int nr, const volatile unsigned long addr) {
	return (addr & (1<<nr)) != 0;
}

static inline void set_bit(int nr, volatile unsigned long addr) {
	addr |= (1 << nr);
}

static inline void clear_bit(int nr, volatile unsigned long addr) {
	addr &= ~(1 << nr);
}

/*
 * change_bit - Toggle a bit in memory
 */
static inline void change_bit(int nr, unsigned long *addr){
	*addr ^= BIT_MASK(nr);
}

static inline int test_and_set_bit(int nr, volatile unsigned long addr) {
	int rc = test_bit(nr, addr);
	set_bit(nr,addr);
	return rc;
}

/*
 * test_and_clear_bit - Clear a bit and return its old value
 */
static inline int test_and_clear_bit(int nr, unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long old = *addr;

	*addr = old & ~mask;
	return (old & mask) != 0;
}

/*
 * test_and_change_bit - Toggle a bit and return its old value
 */
static inline int test_and_change_bit(int nr, unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long old = *addr;

	*addr = old ^ mask;
	return (old & mask) != 0;
}
#endif

static inline const int generic_fls(unsigned int x){
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}

static inline unsigned int generic___fls(unsigned long word){
	unsigned int num = BITS_PER_LONG - 1;

#if BITS_PER_LONG == 64
	if (!(word & (~0ul << 32))) {
		num -= 32;
		word <<= 32;
	}
#endif
	if (!(word & (~0ul << (BITS_PER_LONG-16)))) {
		num -= 16;
		word <<= 16;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-8)))) {
		num -= 8;
		word <<= 8;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-4)))) {
		num -= 4;
		word <<= 4;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-2)))) {
		num -= 2;
		word <<= 2;
	}
	if (!(word & (~0ul << (BITS_PER_LONG-1))))
		num -= 1;
	return num;
}

static inline int ilog2(u64 n){
	return n ? (63 - __builtin_clzll(n)) : -1;
}

#if BITS_PER_LONG == 32
static inline const int fls64(u64 x){
	__u32 h = x >> 32;
	if (h){
		return fls(h) + 32;
	}
	return fls(x);
}
#elif BITS_PER_LONG == 64
static inline const int fls64(u64 x){
	if (x == 0){
		return 0;
	}
	return __fls(x) + 1;
}
#endif

static inline unsigned int fls_long(unsigned long l){
	if (sizeof(l) == 4){
		return fls(l);
	}
	return fls64(l);
}

/*
 * __roundup_pow_of_two() - round up to nearest power of two
 * @n: value to round up
 */
static inline const unsigned long __roundup_pow_of_two(unsigned long n){
	return 1UL << fls_long(n - 1);
}

/*
 * __rounddown_pow_of_two() - round down to nearest power of two
 * @n: value to round down
 */
static inline const unsigned long __rounddown_pow_of_two(unsigned long n){
	return 1UL << (fls_long(n) - 1);
}

/*
 * generic___set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static inline void generic___set_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p  |= mask;
}

static inline void generic___clear_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p &= ~mask;
}

/*
 * generic___change_bit - Toggle a bit in memory
 * @nr: the bit to change
 * @addr: the address to start counting from
 *
 * Unlike change_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
static inline void generic___change_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);

	*p ^= mask;
}

/*
 * generic___test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static inline bool generic___test_and_set_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old | mask;
	return (old & mask) != 0;
}

/*
 * generic___test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is non-atomic and can be reordered.
 * If two examples of this operation race, one can appear to succeed
 * but actually fail.  You must protect multiple accesses with a lock.
 */
static inline bool generic___test_and_clear_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old & ~mask;
	return (old & mask) != 0;
}

/* WARNING: non atomic and it can be reordered! */
static inline bool generic___test_and_change_bit(unsigned long nr, volatile unsigned long *addr){
	unsigned long mask = BIT_MASK(nr);
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;

	*p = old ^ mask;
	return (old & mask) != 0;
}

/*
 * generic_test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static inline bool generic_test_bit(unsigned long nr, const volatile unsigned long *addr){
	/*
	 * Unlike the bitops with the '__' prefix above, this one *is* atomic,
	 * so `volatile` must always stay here with no cast-aways. See
	 * `Documentation/atomic_bitops.txt` for the details.
	 */
	return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG-1)));
}

/*
 * ror32 - rotate a 32-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u32 ror32(__u32 word, unsigned int shift){
	return (word >> (shift & 31)) | (word << ((-shift) & 31));
}

/*
 * rol32 - rotate a 32-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline u32 rol32(__u32 word, unsigned int shift){
	return (word << (shift & 31)) | (word >> ((-shift) & 31));
}

static inline __sum16 csum_fold(__wsum csum){
        u32 sum = (__force u32)csum;
        return (__force __sum16)((~sum - ror32(sum, 16)) >> 16);
}

static inline u16 get_unaligned_le16(const void *p){
	return le16_to_cpu(__get_unaligned_t(__le16, p));
}

static inline u32 get_unaligned_le32(const void *p){
	return le32_to_cpu(__get_unaligned_t(__le32, p));
}

static inline u64 get_unaligned_le64(const void *p){
	return le64_to_cpu(__get_unaligned_t(__le64, p));
}

static inline void put_unaligned_le16(u16 val, void *p){
	__put_unaligned_t(__le16, cpu_to_le16(val), p);
}

static inline void put_unaligned_le32(u32 val, void *p){
	__put_unaligned_t(__le32, cpu_to_le32(val), p);
}

static inline void put_unaligned_le64(u64 val, void *p){
	__put_unaligned_t(__le64, cpu_to_le64(val), p);
}

static inline int lockdep_rht_mutex_is_held(struct rhashtable *ht){
	return 1;
}

static inline int lockdep_rht_bucket_is_held(const struct bucket_table *tbl, u32 hash){
	return 1;
}

static inline bool strstarts(const char *str, const char *prefix){
        return strncmp(str, prefix, strlen(prefix)) == 0;
}

#ifndef __WIN64__
static inline int isdigit(int c){
        return '0' <= c && c <= '9';
}
#endif

static inline unsigned char __tolower(unsigned char c){
	if (isupper(c)){
		c -= 'A'-'a';
	}
	return c;
}

static inline unsigned char __toupper(unsigned char c){
	if (islower(c)){
		c -= 'a'-'A';
	}
	return c;
}

/* rewrite memdup here */
static void *memdup(void *memarea, unsigned int size){
	void *new_memarea = NULL;

	if(memarea == NULL){
		return NULL;
	}
	if(size > 0){
		new_memarea = (void *)malloc(size);
		memcpy(new_memarea, memarea, size);
		return new_memarea;
	}else{
		return NULL;
	}
}

static uint64_t nsec_now_monotonic(void){
	struct timespec ts = {};

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * NSEC_PER_SEC + (uint64_t)ts.tv_nsec;
}

static inline struct timespec timespec_add_ms(struct timespec t, unsigned long ms){
	t.tv_sec += (time_t)(ms / 1000);
	t.tv_nsec += (long)(ms % 1000) * 1000000L;
	if (t.tv_nsec >= 1000000000L) {
		t.tv_sec++;
		t.tv_nsec -= 1000000000L;
	}
	return t;
}

static inline unsigned int __ffs(unsigned long word){
	unsigned int num = 0;
#if BITS_PER_LONG == 64
	if ((word & 0xffffffff) == 0) {
		num += 32;
		word >>= 32;
	}
#endif
	if ((word & 0xffff) == 0) {
		num += 16;
		word >>= 16;
	}
	if ((word & 0xff) == 0) {
		num += 8;
		word >>= 8;
	}
	if ((word & 0xf) == 0) {
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1) == 0)
		num += 1;
	return num;
}

/*
 * fns - find N'th set bit in a word
 * @word: The word to search
 * @n: Bit to find
 */
static inline unsigned int fns(unsigned long word, unsigned int n){
	while (word && n--){
		word &= word - 1;
	}
	return word ? __ffs(word) : BITS_PER_LONG;
}

static unsigned long read_word_at_a_time(const void *addr){
        return *(unsigned long *)addr;
}

static inline unsigned long has_zero(unsigned long a, unsigned long *bits, const struct word_at_a_time *c){
	unsigned long mask = ((a - c->one_bits) & ~a) & c->high_bits;
	*bits = mask;
	return mask;
}

#if defined(__LP64__) || defined(__LLP64__)
#define create_zero_mask(bits) (bits)
#else
static inline unsigned long create_zero_mask(unsigned long bits){
	bits = (bits - 1) & ~bits;
	return bits >> 7;
}
#endif

static unsigned long zero_bytemask(unsigned long bits);

static ssize_t exsul_sized_strscpy(char *dest, const char *src, size_t count){
	int ret = 0;
	long res = 0;
	size_t max = 0;
	const struct word_at_a_time constants = WORD_AT_A_TIME_CONSTANTS;

	max = count;
	if (count == 0 || (count > 99999999)){
		ret = -EINVAL;
		return ret;
	}

        if ((long)src & (sizeof(long) - 1)) {
                size_t limit = 4096 - ((long)src & (4096 - 1));
                if (limit < max){
                        max = limit;
                }
        }

        while (max >= sizeof(unsigned long)) {
                unsigned long c                         = 0;
                unsigned long data                      = 0;

                c                                       = read_word_at_a_time(src+res);
                if (has_zero(c, &data, &constants)) {
                        data                            = prep_zero_mask(c, data, &constants);
                        data                            = create_zero_mask(data);
                        *(unsigned long *)(dest+res)    = c & zero_bytemask(data);
                        return res + find_zero(data);
                }
                *(unsigned long *)(dest+res)            = c;
                res                                    += sizeof(unsigned long);
                count                                  -= sizeof(unsigned long);
                max                                    -= sizeof(unsigned long);
        }

        while (count) {
                char c                                  = 0;

                c                                       = src[res];
                dest[res]                               = c;

                if (! ( c ) ){
                        return res;
                }

                res++;
                count--;
        }

        if (res){
                dest[res-1]                             = '\0';
        }

        ret = -E2BIG;
	return ret;
}

/*
 * is_power_of_2() - check if a value is a power of two
 * @n: the value to check
 *
 * Determine whether some value is a power of two, where zero is
 * *not* considered a power of two.
 * Return: true if @n is a power of 2, otherwise false.
 */
static inline const bool is_power_of_2(unsigned long n){
	return (n != 0 && ((n & (n - 1)) == 0));
}

/* variant of 'futex_wait' which isn't exported in the kernel API. */
static inline int futex_wait_timeout(atomic_int *addr, int expected, struct timespec *rel_timeout){
	#ifdef _WIN64
	if (expected == 1) {
		WakeByAddressSingle(addr);
		return true;
	}
	WakeByAddressAll(addr);
	return expected;
	#else
	return (int)syscall(SYS_futex, (int *)addr, FUTEX_WAIT, expected, rel_timeout, NULL, 0);
	#endif
}

static bool is_a_number(unsigned char character){
	bool ret = false;
	if(character >= '0' && character <= '9'){
		ret = true;
	}
	return ret;
}

static int speed_duplex_to_capa(int speed, unsigned int duplex){
	int ret = 0;

	if (duplex == DUPLEX_UNKNOWN || (speed > SPEED_1000 && duplex != DUPLEX_FULL)){
		ret = -EINVAL;
		return ret;
	}
	switch (speed) {
		case SPEED_10: return duplex == DUPLEX_FULL ? LINK_CAPA_10FD : LINK_CAPA_10HD;
		case SPEED_100: return duplex == DUPLEX_FULL ? LINK_CAPA_100FD : LINK_CAPA_100HD;
		case SPEED_1000: return duplex == DUPLEX_FULL ? LINK_CAPA_1000FD : LINK_CAPA_1000HD;
		case SPEED_2500: return LINK_CAPA_2500FD;
		case SPEED_5000: return LINK_CAPA_5000FD;
		case SPEED_10000: return LINK_CAPA_10000FD;
		case SPEED_20000: return LINK_CAPA_20000FD;
		case SPEED_25000: return LINK_CAPA_25000FD;
		case SPEED_40000: return LINK_CAPA_40000FD;
		case SPEED_50000: return LINK_CAPA_50000FD;
		case SPEED_56000: return LINK_CAPA_56000FD;
		case SPEED_100000: return LINK_CAPA_100000FD;
		case SPEED_200000: return LINK_CAPA_200000FD;
		case SPEED_400000: return LINK_CAPA_400000FD;
		case SPEED_800000: return LINK_CAPA_800000FD;
		case SPEED_1600000: return LINK_CAPA_1600000FD;
	}

	ret = -EINVAL;
	return ret;
}

/* functions for handling the allocation list */
static struct allocation_list *add_top_memory_zone(struct allocation_list *current_allocation, struct allocation_list *head){
	if(head != NULL){
		struct allocation_list *next_memory_zone = head;
		head = (struct allocation_list *)malloc(sizeof(struct allocation_list));
                head->alloc_type  = current_allocation->alloc_type;
                head->alloc_ptr   = current_allocation->alloc_ptr;
                head->alloc_len   = current_allocation->alloc_len;
		head->alloc_flags = current_allocation->alloc_flags;
		head->alloc_inmem_type  = current_allocation->alloc_inmem_type;
		head->alloc_next  = next_memory_zone;
	}else{
		head = (struct allocation_list *)malloc(sizeof(struct allocation_list));
                head->alloc_type  = current_allocation->alloc_type;
                head->alloc_ptr   = current_allocation->alloc_ptr;
                head->alloc_len   = current_allocation->alloc_len;
		head->alloc_flags = current_allocation->alloc_flags;
		head->alloc_inmem_type  = current_allocation->alloc_inmem_type;
		head->alloc_next  = NULL;
	}
	return head;
}

static struct allocation_list *add_memory_zone(struct allocation_list *current_allocation, struct allocation_list *head){
	struct allocation_list *next_memory_area = NULL;

	if(head != NULL){
		next_memory_area = head;
		while(next_memory_area->alloc_next != NULL){
			next_memory_area = next_memory_area->alloc_next;
		}
		next_memory_area->alloc_next = add_top_memory_zone(current_allocation, NULL);
	}else{
		head = add_top_memory_zone(current_allocation, head);
	}
	return head;
}

static void dump_memory_allocation_list(struct allocation_list *head){
	int counter = 0;
	int total_bytes_used = 0;
	if(head != NULL){
		while(head != NULL){
			printf("memory area: %d\n", counter);
			printf(" - alloc_ptr: 0x%08x\n", head->alloc_ptr);
			printf(" - alloc_len: %d\n", head->alloc_len);
			printf(" - alloc_flags: %d\n", head->alloc_flags);
			total_bytes_used += head->alloc_len;
			head = head->alloc_next;
			counter++;
		}
	}
	printf("[*] actually used %d bytes\n", total_bytes_used);
}

/* high level function which adds an allocation node to the main context */
static struct allocation_list *new_allocation(u8 allocation_type, void *allocation_ptr, int allocation_length, u8 allocation_flags, u8 allocation_inmem_type, struct allocation_list *allocation_head){
	struct allocation_list *head = NULL;
	struct allocation_list current_allocation = {
		.alloc_type  = allocation_type,
		.alloc_ptr   = allocation_ptr,
		.alloc_flags = allocation_flags,
		.alloc_len   = allocation_length,
		.alloc_inmem_type = allocation_inmem_type,
	};
	head = add_memory_zone(&current_allocation, allocation_head);
	return head;
}

/*
 * simple function which converts the vermagic into a series of numbers
 * example of a vermagic: vermagic=6.14.0-36-generic
 */
static int convert_vermagic(unsigned char *vermagic_string, u8 *version, u8 *subversion, u8 *modification){
	int ret = 0;
	unsigned char *current_ver = NULL;

	if(vermagic_string == NULL){
		ret = -EINVAL;
		return ret;
	}

	current_ver = strtok(vermagic_string, ".");
	if(current_ver != NULL){
		*version = atoi(current_ver);
	}else{
		*version = 0;
	}

	return ret;
}

static void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

#ifdef ENABLE_UNWIND_DEBUGGING
#undef _U
#undef _L
#undef _D
#undef _C
#undef _P
#undef _S
#undef _X
#undef _SP
#endif
#endif
