typedef const int  tracepoint_ptr_t;

struct core_text {
	long unsigned int          base;                 /*     0     8 */
	long unsigned int          end;                  /*     8     8 */
	const char  *              name;                 /*    16     8 */

	/* size: 24, cachelines: 1, members: 3 */
	/* last cacheline: 24 bytes */
};

struct ddebug_class_map {
	struct list_head           link;                 /*     0    16 */
	struct module *            mod;                  /*    16     8 */
	const char  *              mod_name;             /*    24     8 */
	const char  * *            class_names;          /*    32     8 */
	const int                  length;               /*    40     4 */
	const int                  base;                 /*    44     4 */
	u32			   map_type;             /*    48     4 */

	/* size: 56, cachelines: 1, members: 7 */
	/* padding: 4 */
	/* last cacheline: 56 bytes */
};

struct mod_arch_specific {
	u8 pad[16];

	/* XXX last struct has 4 bytes of padding */

	/* size: 16, cachelines: 1, members: 1 */
	/* paddings: 1, sum paddings: 4 */
	/* last cacheline: 16 bytes */
};

struct module_kobject {
	u8 pad[96];

	/* size: 96, cachelines: 2, members: 5 */
	/* last cacheline: 32 bytes */
};

struct static_key {
	atomic_t                   enabled;              /*     0     4 */

	/* XXX 4 bytes hole, try to pack */

	union {
		long unsigned int  type;                 /*     8     8 */
		struct jump_entry * entries;             /*     8     8 */
		struct static_key_mod * next;            /*     8     8 */
	};                                               /*     8     8 */

	/* size: 16, cachelines: 1, members: 2 */
	/* sum members: 12, holes: 1, sum holes: 4 */
	/* last cacheline: 16 bytes */
};

struct static_key_true {
        struct static_key          key;                  /*     0    16 */

        /* size: 16, cachelines: 1, members: 1 */
        /* last cacheline: 16 bytes */
};

struct static_key_false {
        struct static_key          key;                  /*     0    16 */

        /* size: 16, cachelines: 1, members: 1 */
        /* last cacheline: 16 bytes */
};

struct _ddebug {
	const char  *              modname;              /*     0     8 */
	const char  *              function;             /*     8     8 */
	const char  *              filename;             /*    16     8 */
	const char  *              format;               /*    24     8 */
	unsigned int               lineno:18;            /*    32: 0  4 */
	unsigned int               class_id:6;           /*    32:18  4 */
	unsigned int               flags:8;              /*    32:24  4 */

	/* XXX 4 bytes hole, try to pack */

	union {
		struct static_key_true dd_key_true;      /*    40    16 */
		struct static_key_false dd_key_false;    /*    40    16 */
	} key;                                           /*    40    16 */

	/* size: 56, cachelines: 1, members: 8 */
	/* sum members: 48, holes: 1, sum holes: 4 */
	/* sum bitfield members: 32 bits (4 bytes) */
	/* last cacheline: 56 bytes */
};

struct _ddebug_info {
	struct _ddebug *           descs;                /*     0     8 */
	struct ddebug_class_map *  classes;              /*     8     8 */
	unsigned int               num_descs;            /*    16     4 */
	unsigned int               num_classes;          /*    20     4 */

	/* size: 24, cachelines: 1, members: 4 */
	/* last cacheline: 24 bytes */
};

struct module {
	enum module_state          state;                /*     0     4 */

	/* XXX 4 bytes hole, try to pack */

	struct list_head           list;                 /*     8    16 */
	char                       name[56];             /*    24    56 */
	/* --- cacheline 1 boundary (64 bytes) was 16 bytes ago --- */
	struct module_kobject      mkobj;                /*    80    96 */
	/* --- cacheline 2 boundary (128 bytes) was 48 bytes ago --- */
	struct module_attribute *  modinfo_attrs;        /*   176     8 */
	const char  *              version;              /*   184     8 */
	/* --- cacheline 3 boundary (192 bytes) --- */
	const char  *              srcversion;           /*   192     8 */
	struct kobject *           holders_dir;          /*   200     8 */
	const struct kernel_symbol  * syms;              /*   208     8 */
	const u32  *               crcs;                 /*   216     8 */
	unsigned int               num_syms;             /*   224     4 */

	/* XXX 4 bytes hole, try to pack */

	struct mutex               param_lock;           /*   232    32 */
	/* --- cacheline 4 boundary (256 bytes) was 8 bytes ago --- */
	struct kernel_param *      kp;                   /*   264     8 */
	unsigned int               num_kp;               /*   272     4 */
	unsigned int               num_gpl_syms;         /*   276     4 */
	const struct kernel_symbol  * gpl_syms;          /*   280     8 */
	const u32  *               gpl_crcs;             /*   288     8 */
	bool                       using_gplonly_symbols; /*   296     1 */
	bool                       sig_ok;               /*   297     1 */
	bool                       async_probe_requested; /*   298     1 */

	/* XXX 1 byte hole, try to pack */

	unsigned int               num_exentries;        /*   300     4 */
	struct exception_table_entry * extable;          /*   304     8 */
	int                        (*init)(void);        /*   312     8 */
	/* --- cacheline 5 boundary (320 bytes) --- */
	struct module_memory       mem[7] __attribute__((__aligned__(64))); /*   320   560 */
	/* --- cacheline 13 boundary (832 bytes) was 48 bytes ago --- */
	struct mod_arch_specific   arch;                 /*   880    16 */
	/* --- cacheline 14 boundary (896 bytes) --- */
	long unsigned int          taints;               /*   896     8 */
	unsigned int               num_bugs;             /*   904     4 */

	/* XXX 4 bytes hole, try to pack */

	struct list_head           bug_list;             /*   912    16 */
	struct bug_entry *         bug_table;            /*   928     8 */
	struct mod_kallsyms *      kallsyms;             /*   936     8 */
	struct mod_kallsyms        core_kallsyms;        /*   944    32 */
	/* --- cacheline 15 boundary (960 bytes) was 16 bytes ago --- */
	struct module_sect_attrs * sect_attrs;           /*   976     8 */
	struct module_notes_attrs * notes_attrs;         /*   984     8 */
	char *                     args;                 /*   992     8 */
	void *                     percpu;               /*  1000     8 */
	unsigned int               percpu_size;          /*  1008     4 */

	/* XXX 4 bytes hole, try to pack */

	void *                     noinstr_text_start;   /*  1016     8 */
	/* --- cacheline 16 boundary (1024 bytes) --- */
	unsigned int               noinstr_text_size;    /*  1024     4 */
	unsigned int               num_tracepoints;      /*  1028     4 */
	tracepoint_ptr_t *         tracepoints_ptrs;     /*  1032     8 */
	unsigned int               num_srcu_structs;     /*  1040     4 */

	/* XXX 4 bytes hole, try to pack */

	struct srcu_struct * *     srcu_struct_ptrs;     /*  1048     8 */
	unsigned int               num_bpf_raw_events;   /*  1056     4 */

	/* XXX 4 bytes hole, try to pack */

	struct bpf_raw_event_map * bpf_raw_events;       /*  1064     8 */
	unsigned int               btf_data_size;        /*  1072     4 */
	unsigned int               btf_base_data_size;   /*  1076     4 */
	void *                     btf_data;             /*  1080     8 */
	/* --- cacheline 17 boundary (1088 bytes) --- */
	void *                     btf_base_data;        /*  1088     8 */
	struct jump_entry *        jump_entries;         /*  1096     8 */
	unsigned int               num_jump_entries;     /*  1104     4 */
	unsigned int               num_trace_bprintk_fmt; /*  1108     4 */
	const char  * *            trace_bprintk_fmt_start; /*  1112     8 */
	struct trace_event_call * * trace_events;        /*  1120     8 */
	unsigned int               num_trace_events;     /*  1128     4 */

	/* XXX 4 bytes hole, try to pack */

	struct trace_eval_map * *  trace_evals;          /*  1136     8 */
	unsigned int               num_trace_evals;      /*  1144     4 */
	unsigned int               num_ftrace_callsites; /*  1148     4 */
	/* --- cacheline 18 boundary (1152 bytes) --- */
	long unsigned int *        ftrace_callsites;     /*  1152     8 */
	void *                     kprobes_text_start;   /*  1160     8 */
	unsigned int               kprobes_text_size;    /*  1168     4 */

	/* XXX 4 bytes hole, try to pack */

	long unsigned int *        kprobe_blacklist;     /*  1176     8 */
	unsigned int               num_kprobe_blacklist; /*  1184     4 */
	int                        num_static_call_sites; /*  1188     4 */
	struct static_call_site *  static_call_sites;    /*  1192     8 */
	bool                       klp;                  /*  1200     1 */
	bool                       klp_alive;            /*  1201     1 */

	/* XXX 6 bytes hole, try to pack */

	struct klp_modinfo *       klp_info;             /*  1208     8 */
	/* --- cacheline 19 boundary (1216 bytes) --- */
	struct list_head           source_list;          /*  1216    16 */
	struct list_head           target_list;          /*  1232    16 */
	void                       (*exit)(void);        /*  1248     8 */
	atomic_t                   refcnt;               /*  1256     4 */

	/* XXX 4 bytes hole, try to pack */

	struct error_injection_entry * ei_funcs;         /*  1264     8 */
	unsigned int               num_ei_funcs;         /*  1272     4 */

	/* XXX 4 bytes hole, try to pack */

	/* --- cacheline 20 boundary (1280 bytes) --- */
	struct _ddebug_info        dyndbg_info;          /*  1280    24 */

	/* size: 1344, cachelines: 21, members: 74 */
	/* sum members: 1257, holes: 12, sum holes: 47 */
	/* padding: 40 */
	/* forced alignments: 1 */
} __attribute__((__aligned__(64)));
