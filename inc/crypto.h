/* contains the structs, enums and functions related for cryptographic operations, mainly for the ieee80211 layer */

#define cra_cipher			cra_u.cipher
/* Set this bit to handle partial blocks in the API. */
#define CRYPTO_AHASH_ALG_BLOCK_ONLY	0x01000000

/* Set this bit if final requires at least one byte. */
#define CRYPTO_AHASH_ALG_FINAL_NONZERO	0x02000000

/* Set this bit if finup can deal with multiple blocks. */
#define CRYPTO_AHASH_ALG_FINUP_MAX	0x04000000

/* This bit is set by the Crypto API if export_core is not supported. */
#define CRYPTO_AHASH_ALG_NO_EXPORT_CORE	0x08000000

/*
 * Transform masks and values (for crt_flags).
 */
#define CRYPTO_TFM_NEED_KEY		0x00000001

#define CRYPTO_TFM_REQ_MASK		0x000fff00
#define CRYPTO_TFM_REQ_FORBID_WEAK_KEYS	0x00000100
#define CRYPTO_TFM_REQ_MAY_SLEEP	0x00000200
#define CRYPTO_TFM_REQ_MAY_BACKLOG	0x00000400
#define CRYPTO_TFM_REQ_ON_STACK		0x00000800

#define CRYPTO_MAX_ALG_NAME		128

/*
 * Algorithm masks and types.
 */
#define CRYPTO_ALG_TYPE_MASK		0x0000000f
#define CRYPTO_ALG_TYPE_CIPHER		0x00000001
#define CRYPTO_ALG_TYPE_AEAD		0x00000003
#define CRYPTO_ALG_TYPE_LSKCIPHER	0x00000004
#define CRYPTO_ALG_TYPE_SKCIPHER	0x00000005
#define CRYPTO_ALG_TYPE_AKCIPHER	0x00000006
#define CRYPTO_ALG_TYPE_SIG		0x00000007
#define CRYPTO_ALG_TYPE_KPP		0x00000008
#define CRYPTO_ALG_TYPE_ACOMPRESS	0x0000000a
#define CRYPTO_ALG_TYPE_SCOMPRESS	0x0000000b
#define CRYPTO_ALG_TYPE_RNG		0x0000000c
#define CRYPTO_ALG_TYPE_HASH		0x0000000e
#define CRYPTO_ALG_TYPE_SHASH		0x0000000e
#define CRYPTO_ALG_TYPE_AHASH		0x0000000f

#define CRYPTO_ALG_TYPE_ACOMPRESS_MASK	0x0000000e

#define CRYPTO_ALG_LARVAL		0x00000010
#define CRYPTO_ALG_DEAD			0x00000020
#define CRYPTO_ALG_DYING		0x00000040
#define CRYPTO_ALG_ASYNC		0x00000080

/*
 * Set if the algorithm (or an algorithm which it uses) requires another
 * algorithm of the same type to handle corner cases.
 */
#define CRYPTO_ALG_NEED_FALLBACK	0x00000100

/*
 * Set if the algorithm data structure should be duplicated into
 * kmalloc memory before registration.  This is useful for hardware
 * that can be disconnected at will.  Do not use this if the data
 * structure is embedded into a bigger one.  Duplicate the overall
 * data structure in the driver in that case.
 */
#define CRYPTO_ALG_DUP_FIRST		0x00000200

/*
 * Set if the algorithm has passed automated run-time testing.  Note that
 * if there is no run-time testing for a given algorithm it is considered
 * to have passed.
 */

#define CRYPTO_ALG_TESTED		0x00000400

/*
 * Set if the algorithm is an instance that is built from templates.
 */
#define CRYPTO_ALG_INSTANCE		0x00000800

/* Set this bit if the algorithm provided is hardware accelerated but
 * not available to userspace via instruction set or so.
 */
#define CRYPTO_ALG_KERN_DRIVER_ONLY	0x00001000

/*
 * Mark a cipher as a service implementation only usable by another
 * cipher and never by a normal user of the kernel crypto API
 */
#define CRYPTO_ALG_INTERNAL		0x00002000

/*
 * Set if the algorithm has a ->setkey() method but can be used without
 * calling it first, i.e. there is a default key.
 */
#define CRYPTO_ALG_OPTIONAL_KEY		0x00004000

/*
 * Don't trigger module loading
 */
#define CRYPTO_NOLOAD			0x00008000

/*
 * struct hash_alg_common - define properties of message digest
 * @digestsize: Size of the result of the transformation. A buffer of this size
 *	        must be available to the @final and @finup calls, so they can
 *	        store the resulting hash into it. For various predefined sizes,
 *	        search include/crypto/ using
 *	        git grep _DIGEST_SIZE include/crypto.
 * @statesize: Size of the block for partial state of the transformation. A
 *	       buffer of this size must be passed to the @export function as it
 *	       will save the partial state of the transformation into it. On the
 *	       other side, the @import function will load the state from a
 *	       buffer of this size as well.
 * @base: Start of data structure of cipher algorithm. The common data
 *	  structure of crypto_alg contains information common to all ciphers.
 *	  The hash_alg_common data structure now adds the hash-specific
 *	  information.
 */
#define HASH_ALG_COMMON {		\
	unsigned int digestsize;	\
	unsigned int statesize;		\
					\
	struct crypto_alg base;		\
}

/* Crypto notification events. */
enum {
	CRYPTO_MSG_ALG_REQUEST,
	CRYPTO_MSG_ALG_REGISTER,
	CRYPTO_MSG_ALG_LOADED,
};

/**
 * struct cipher_alg - single-block symmetric ciphers definition
 * @cia_min_keysize: Minimum key size supported by the transformation. This is
 *		     the smallest key length supported by this transformation
 *		     algorithm. This must be set to one of the pre-defined
 *		     values as this is not hardware specific. Possible values
 *		     for this field can be found via git grep "_MIN_KEY_SIZE"
 *		     include/crypto/
 * @cia_max_keysize: Maximum key size supported by the transformation. This is
 *		    the largest key length supported by this transformation
 *		    algorithm. This must be set to one of the pre-defined values
 *		    as this is not hardware specific. Possible values for this
 *		    field can be found via git grep "_MAX_KEY_SIZE"
 *		    include/crypto/
 * @cia_setkey: Set key for the transformation. This function is used to either
 *	        program a supplied key into the hardware or store the key in the
 *	        transformation context for programming it later. Note that this
 *	        function does modify the transformation context. This function
 *	        can be called multiple times during the existence of the
 *	        transformation object, so one must make sure the key is properly
 *	        reprogrammed into the hardware. This function is also
 *	        responsible for checking the key length for validity.
 * @cia_encrypt: Encrypt a single block. This function is used to encrypt a
 *		 single block of data, which must be @cra_blocksize big. This
 *		 always operates on a full @cra_blocksize and it is not possible
 *		 to encrypt a block of smaller size. The supplied buffers must
 *		 therefore also be at least of @cra_blocksize size. Both the
 *		 input and output buffers are always aligned to @cra_alignmask.
 *		 In case either of the input or output buffer supplied by user
 *		 of the crypto API is not aligned to @cra_alignmask, the crypto
 *		 API will re-align the buffers. The re-alignment means that a
 *		 new buffer will be allocated, the data will be copied into the
 *		 new buffer, then the processing will happen on the new buffer,
 *		 then the data will be copied back into the original buffer and
 *		 finally the new buffer will be freed. In case a software
 *		 fallback was put in place in the @cra_init call, this function
 *		 might need to use the fallback if the algorithm doesn't support
 *		 all of the key sizes. In case the key was stored in
 *		 transformation context, the key might need to be re-programmed
 *		 into the hardware in this function. This function shall not
 *		 modify the transformation context, as this function may be
 *		 called in parallel with the same transformation object.
 * @cia_decrypt: Decrypt a single block. This is a reverse counterpart to
 *		 @cia_encrypt, and the conditions are exactly the same.
 *
 * All fields are mandatory and must be filled.
 */
struct cipher_alg {
	unsigned int cia_min_keysize;
	unsigned int cia_max_keysize;
	int (*cia_setkey)(struct crypto_tfm *tfm, const u8 *key, unsigned int keylen);
	void (*cia_encrypt)(struct crypto_tfm *tfm, u8 *dst, const u8 *src);
	void (*cia_decrypt)(struct crypto_tfm *tfm, u8 *dst, const u8 *src);
};

struct crypto_type {
	unsigned int (*ctxsize)(struct crypto_alg *alg, u32 type, u32 mask);
	unsigned int (*extsize)(struct crypto_alg *alg);
	int (*init_tfm)(struct crypto_tfm *tfm);
	void (*show)(struct seq_file *m, struct crypto_alg *alg);
	int (*report)(struct sk_buff *skb, struct crypto_alg *alg);
	void (*free)(struct crypto_instance *inst);
	void (*destroy)(struct crypto_alg *alg);

	unsigned int type;
	unsigned int maskclear;
	unsigned int maskset;
	unsigned int tfmsize;
	unsigned int algsize;
};

/**
 * struct crypto_alg - definition of a cryptograpic cipher algorithm
 * @cra_flags: Flags describing this transformation. See include/linux/crypto.h
 *	       CRYPTO_ALG_* flags for the flags which go in here. Those are
 *	       used for fine-tuning the description of the transformation
 *	       algorithm.
 * @cra_blocksize: Minimum block size of this transformation. The size in bytes
 *		   of the smallest possible unit which can be transformed with
 *		   this algorithm. The users must respect this value.
 *		   In case of HASH transformation, it is possible for a smaller
 *		   block than @cra_blocksize to be passed to the crypto API for
 *		   transformation, in case of any other transformation type, an
 * 		   error will be returned upon any attempt to transform smaller
 *		   than @cra_blocksize chunks.
 * @cra_ctxsize: Size of the operational context of the transformation. This
 *		 value informs the kernel crypto API about the memory size
 *		 needed to be allocated for the transformation context.
 * @cra_alignmask: For cipher, skcipher, lskcipher, and aead algorithms this is
 *		   1 less than the alignment, in bytes, that the algorithm
 *		   implementation requires for input and output buffers.  When
 *		   the crypto API is invoked with buffers that are not aligned
 *		   to this alignment, the crypto API automatically utilizes
 *		   appropriately aligned temporary buffers to comply with what
 *		   the algorithm needs.  (For scatterlists this happens only if
 *		   the algorithm uses the skcipher_walk helper functions.)  This
 *		   misalignment handling carries a performance penalty, so it is
 *		   preferred that algorithms do not set a nonzero alignmask.
 *		   Also, crypto API users may wish to allocate buffers aligned
 *		   to the alignmask of the algorithm being used, in order to
 *		   avoid the API having to realign them.  Note: the alignmask is
 *		   not supported for hash algorithms and is always 0 for them.
 * @cra_reqsize: Size of the request context for this algorithm.
 * @cra_priority: Priority of this transformation implementation. In case
 *		  multiple transformations with same @cra_name are available to
 *		  the Crypto API, the kernel will use the one with highest
 *		  @cra_priority.
 * @cra_name: Generic name (usable by multiple implementations) of the
 *	      transformation algorithm. This is the name of the transformation
 *	      itself. This field is used by the kernel when looking up the
 *	      providers of particular transformation.
 * @cra_driver_name: Unique name of the transformation provider. This is the
 *		     name of the provider of the transformation. This can be any
 *		     arbitrary value, but in the usual case, this contains the
 *		     name of the chip or provider and the name of the
 *		     transformation algorithm.
 * @cra_type: Type of the cryptographic transformation. This is a pointer to
 *	      struct crypto_type, which implements callbacks common for all
 *	      transformation types. There are multiple options, such as
 *	      &crypto_skcipher_type, &crypto_ahash_type, &crypto_rng_type.
 *	      This field might be empty. In that case, there are no common
 *	      callbacks. This is the case for: cipher.
 * @cra_u: Callbacks implementing the transformation. This is a union of
 *	   multiple structures. Depending on the type of transformation selected
 *	   by @cra_type and @cra_flags above, the associated structure must be
 *	   filled with callbacks. This field might be empty. This is the case
 *	   for ahash, shash.
 * @cra_init: Deprecated, do not use.
 * @cra_exit: Deprecated, do not use.
 * @cra_u.cipher: Union member which contains a single-block symmetric cipher
 *		  definition. See @struct @cipher_alg.
 * @cra_module: Owner of this transformation implementation. Set to THIS_MODULE
 * @cra_list: internally used
 * @cra_users: internally used
 * @cra_refcnt: internally used
 * @cra_destroy: internally used
 *
 * The struct crypto_alg describes a generic Crypto API algorithm and is common
 * for all of the transformations. Any variable not documented here shall not
 * be used by a cipher implementation as it is internal to the Crypto API.
 */
struct crypto_alg {
	struct list_head cra_list;
	struct list_head cra_users;

	u32 cra_flags;
	unsigned int cra_blocksize;
	unsigned int cra_ctxsize;
	unsigned int cra_alignmask;
	unsigned int cra_reqsize;

	int cra_priority;
	refcount_t cra_refcnt;

	char cra_name[CRYPTO_MAX_ALG_NAME];
	char cra_driver_name[CRYPTO_MAX_ALG_NAME];

	const struct crypto_type *cra_type;

	union {
		struct cipher_alg cipher;
	} cra_u;

	int (*cra_init)(struct crypto_tfm *tfm);
	void (*cra_exit)(struct crypto_tfm *tfm);
	void (*cra_destroy)(struct crypto_alg *alg);
};

/*
 * Transforms: user-instantiated objects which encapsulate algorithms
 * and core processing logic.  Managed via crypto_alloc_*() and
 * crypto_free_*(), as well as the various helpers below.
 */
struct crypto_tfm {
	refcount_t refcnt;

	u32 crt_flags;

	int node;

	struct crypto_tfm *fb;

	void (*exit)(struct crypto_tfm *tfm);

	struct crypto_alg *__crt_alg;

	void *__crt_ctx[];
};

struct crypto_shash {
	struct crypto_tfm base;
};

struct crypto_cipher {
	struct crypto_tfm base;
};

struct crypto_aead {
	unsigned int authsize;
	unsigned int reqsize;

	struct crypto_tfm base;
};

struct crypto_sync_aead {
	struct crypto_aead base;
};

struct shash_desc {
	struct crypto_shash *tfm;
	void *__ctx[];
};

/**
 * struct shash_alg - synchronous message digest definition
 * @init: see struct ahash_alg
 * @update: see struct ahash_alg
 * @final: see struct ahash_alg
 * @finup: see struct ahash_alg
 * @digest: see struct ahash_alg
 * @export: see struct ahash_alg
 * @import: see struct ahash_alg
 * @export_core: see struct ahash_alg
 * @import_core: see struct ahash_alg
 * @setkey: see struct ahash_alg
 * @init_tfm: Initialize the cryptographic transformation object.
 *	      This function is called only once at the instantiation
 *	      time, right after the transformation context was
 *	      allocated. In case the cryptographic hardware has
 *	      some special requirements which need to be handled
 *	      by software, this function shall check for the precise
 *	      requirement of the transformation and put any software
 *	      fallbacks in place.
 * @exit_tfm: Deinitialize the cryptographic transformation object.
 *	      This is a counterpart to @init_tfm, used to remove
 *	      various changes set in @init_tfm.
 * @clone_tfm: Copy transform into new object, may allocate memory.
 * @descsize: Size of the operational state for the message digest. This state
 * 	      size is the memory size that needs to be allocated for
 *	      shash_desc.__ctx
 * @halg: see struct hash_alg_common
 * @HASH_ALG_COMMON: see struct hash_alg_common
 */
struct shash_alg {
	int (*init)(struct shash_desc *desc);
	int (*update)(struct shash_desc *desc, const u8 *data, unsigned int len);
	int (*final)(struct shash_desc *desc, u8 *out);
	int (*finup)(struct shash_desc *desc, const u8 *data, unsigned int len, u8 *out);
	int (*digest)(struct shash_desc *desc, const u8 *data, unsigned int len, u8 *out);
	int (*export)(struct shash_desc *desc, void *out);
	int (*import)(struct shash_desc *desc, const void *in);
	int (*export_core)(struct shash_desc *desc, void *out);
	int (*import_core)(struct shash_desc *desc, const void *in);
	int (*setkey)(struct crypto_shash *tfm, const u8 *key, unsigned int keylen);
	int (*init_tfm)(struct crypto_shash *tfm);
	void (*exit_tfm)(struct crypto_shash *tfm);
	int (*clone_tfm)(struct crypto_shash *dst, struct crypto_shash *src);

	unsigned int descsize;
	union {
		struct HASH_ALG_COMMON;
	};
};

struct crypto_spawn {
	struct list_head list;
	struct crypto_alg *alg;
	union {
		/* Back pointer to instance after registration.*/
		struct crypto_instance *inst;
		/* Spawn list pointer prior to registration. */
		struct crypto_spawn *next;
	};
	const struct crypto_type *frontend;
	u32 mask;
	bool dead;
	bool registered;
};

/*
 * A helper struct for waiting for completion of async crypto ops
 */
struct crypto_wait {
	struct completion completion;
	int err;
};

struct crypto_instance {
	struct crypto_alg alg;

	struct crypto_template *tmpl;

	union {
		/* Node in list of instances after registration. */
		struct hlist_node list;
		/* List of attached spawns before registration. */
		struct crypto_spawn *spawns;
	};

	void *__ctx[];
};

struct shash_instance {
	void (*free)(struct shash_instance *inst);
	union {
		struct {
			char head[offsetof(struct shash_alg, base)];
			struct crypto_instance base;
		} s;
		struct shash_alg alg;
	};
};

struct crypto_larval {
	struct crypto_alg alg;
	struct crypto_alg *adult;
	struct completion completion;
	u32 mask;
	bool test_started;
};

/**
 * struct aead_alg - AEAD cipher definition
 * @maxauthsize: Set the maximum authentication tag size supported by the
 *		 transformation. A transformation may support smaller tag sizes.
 *		 As the authentication tag is a message digest to ensure the
 *		 integrity of the encrypted data, a consumer typically wants the
 *		 largest authentication tag possible as defined by this
 *		 variable.
 * @setauthsize: Set authentication size for the AEAD transformation. This
 *		 function is used to specify the consumer requested size of the
 * 		 authentication tag to be either generated by the transformation
 *		 during encryption or the size of the authentication tag to be
 *		 supplied during the decryption operation. This function is also
 *		 responsible for checking the authentication tag size for
 *		 validity.
 * @setkey: see struct skcipher_alg
 * @encrypt: see struct skcipher_alg
 * @decrypt: see struct skcipher_alg
 * @ivsize: see struct skcipher_alg
 * @chunksize: see struct skcipher_alg
 * @init: Initialize the cryptographic transformation object. This function
 *	  is used to initialize the cryptographic transformation object.
 *	  This function is called only once at the instantiation time, right
 *	  after the transformation context was allocated. In case the
 *	  cryptographic hardware has some special requirements which need to
 *	  be handled by software, this function shall check for the precise
 *	  requirement of the transformation and put any software fallbacks
 *	  in place.
 * @exit: Deinitialize the cryptographic transformation object. This is a
 *	  counterpart to @init, used to remove various changes set in
 *	  @init.
 * @base: Definition of a generic crypto cipher algorithm.
 *
 * All fields except @ivsize is mandatory and must be filled.
 */
struct aead_alg {
	int (*setkey)(struct crypto_aead *tfm, const u8 *key, unsigned int keylen);
	int (*setauthsize)(struct crypto_aead *tfm, unsigned int authsize);
	int (*encrypt)(struct aead_request *req);
	int (*decrypt)(struct aead_request *req);
	int (*init)(struct crypto_aead *tfm);
	void (*exit)(struct crypto_aead *tfm);

	unsigned int ivsize;
	unsigned int maxauthsize;
	unsigned int chunksize;

	struct crypto_alg base;
};

struct aead_instance {
	void (*free)(struct aead_instance *inst);
	union {
		struct {
			char head[offsetof(struct aead_alg, base)];
			struct crypto_instance base;
		} s;
		struct aead_alg alg;
	};
};

/* function prototypes guard for avoiding compilation errors */
int shash_no_setkey(struct crypto_shash *tfm, const u8 *key, unsigned int keyle);
unsigned int crypto_alg_extsize(struct crypto_alg *alg);

static inline struct aead_instance *aead_instance(struct crypto_instance *inst){
	return container_of(&inst->alg, struct aead_instance, alg.base);
}

static inline int crypto_is_larval(struct crypto_alg *alg){
	return alg->cra_flags & CRYPTO_ALG_LARVAL;
}

static inline int crypto_is_dead(struct crypto_alg *alg){
	return alg->cra_flags & CRYPTO_ALG_DEAD;
}

static inline int crypto_is_moribund(struct crypto_alg *alg){
	return alg->cra_flags & (CRYPTO_ALG_DEAD | CRYPTO_ALG_DYING);
}

static inline struct crypto_tfm *crypto_aead_tfm(struct crypto_aead *tfm){
	return &tfm->base;
}

static inline struct crypto_tfm *crypto_shash_tfm(struct crypto_shash *tfm){
	return &tfm->base;
}

static inline struct crypto_shash *__crypto_shash_cast(struct crypto_tfm *tfm){
	return container_of(tfm, struct crypto_shash, base);
}

static inline struct shash_instance *shash_instance(struct crypto_instance *inst){
	return container_of(inst, struct shash_instance, s.base);
}

static inline struct aead_alg *crypto_aead_alg(struct crypto_aead *tfm){
	return container_of(crypto_aead_tfm(tfm)->__crt_alg, struct aead_alg, base);
}

static void crypto_aead_free_instance(struct crypto_instance *inst){
	struct aead_instance *aead = aead_instance(inst);
	aead->free(aead);
}

static inline void crypto_free_aead(struct crypto_aead *tfm){
	crypto_destroy_tfm(tfm, crypto_aead_tfm(tfm));
}

/**
 * crypto_free_shash() - zeroize and free the message digest handle
 * @tfm: cipher handle to be freed
 *
 * If @tfm is a NULL or error pointer, this function does nothing.
 */
static inline void crypto_free_shash(struct crypto_shash *tfm){
	crypto_destroy_tfm(tfm, crypto_shash_tfm(tfm));
}

/*
 * Transform helpers which query the underlying algorithm.
 */
static inline const char *crypto_tfm_alg_name(struct crypto_tfm *tfm){
	return tfm->__crt_alg->cra_name;
}

static inline const char *crypto_tfm_alg_driver_name(struct crypto_tfm *tfm){
	return tfm->__crt_alg->cra_driver_name;
}

static inline unsigned int crypto_tfm_alg_blocksize(struct crypto_tfm *tfm){
	return tfm->__crt_alg->cra_blocksize;
}

static inline unsigned int crypto_tfm_alg_alignmask(struct crypto_tfm *tfm){
	return tfm->__crt_alg->cra_alignmask;
}

static inline unsigned int crypto_tfm_alg_reqsize(struct crypto_tfm *tfm){
	return tfm->__crt_alg->cra_reqsize;
}

static inline u32 crypto_tfm_get_flags(struct crypto_tfm *tfm){
	return tfm->crt_flags;
}

static inline struct crypto_tfm *crypto_cipher_tfm(struct crypto_cipher *tfm){
	return &tfm->base;
}

static inline struct cipher_alg *crypto_cipher_alg(struct crypto_cipher *tfm){
	return &crypto_cipher_tfm(tfm)->__crt_alg->cra_cipher;
}

static inline unsigned int crypto_cipher_alignmask(struct crypto_cipher *tfm){
	return crypto_tfm_alg_alignmask(crypto_cipher_tfm(tfm));
}

static inline void crypto_tfm_set_flags(struct crypto_tfm *tfm, u32 flags){
	tfm->crt_flags |= flags;
}

static inline void crypto_tfm_clear_flags(struct crypto_tfm *tfm, u32 flags){
	tfm->crt_flags &= ~flags;
}

static inline unsigned int crypto_aead_alignmask(struct crypto_aead *tfm){
	return crypto_tfm_alg_alignmask(crypto_aead_tfm(tfm));
}

static inline void crypto_aead_clear_flags(struct crypto_aead *tfm, u32 flags){
	crypto_tfm_clear_flags(crypto_aead_tfm(tfm), flags);
}

static inline unsigned int crypto_tfm_ctx_alignment(void){
	struct crypto_tfm *tfm = NULL;
	return __alignof__(tfm->__crt_ctx);
}

static inline struct shash_alg *__crypto_shash_alg(struct crypto_alg *alg){
	return container_of(alg, struct shash_alg, base);
}

static inline struct shash_alg *crypto_shash_alg(struct crypto_shash *tfm){
	return __crypto_shash_alg(crypto_shash_tfm(tfm)->__crt_alg);
}

/**
 * crypto_shash_descsize() - obtain the operational state size
 * @tfm: cipher handle
 *
 * The size of the operational state the cipher needs during operation is
 * returned for the hash referenced with the cipher handle. This size is
 * required to calculate the memory requirements to allow the caller allocating
 * sufficient memory for operational state.
 *
 * The operational state is defined with struct shash_desc where the size of
 * that data structure is to be calculated as
 * sizeof(struct shash_desc) + crypto_shash_descsize(alg)
 *
 * Return: size of the operational state
 */
static inline unsigned int crypto_shash_descsize(struct crypto_shash *tfm){
	return crypto_shash_alg(tfm)->descsize;
}

/**
 * crypto_shash_digestsize() - obtain message digest size
 * @tfm: cipher handle
 *
 * The size for the message digest created by the message digest cipher
 * referenced with the cipher handle is returned.
 *
 * Return: digest size of cipher
 */
static inline unsigned int crypto_shash_digestsize(struct crypto_shash *tfm){
	return crypto_shash_alg(tfm)->digestsize;
}

static inline unsigned int crypto_shash_statesize(struct crypto_shash *tfm){
	return crypto_shash_alg(tfm)->statesize;
}

static inline u32 crypto_shash_get_flags(struct crypto_shash *tfm){
	return crypto_tfm_get_flags(crypto_shash_tfm(tfm));
}

static inline void crypto_shash_set_flags(struct crypto_shash *tfm, u32 flags){
	crypto_tfm_set_flags(crypto_shash_tfm(tfm), flags);
}

static inline void crypto_shash_clear_flags(struct crypto_shash *tfm, u32 flags){
	crypto_tfm_clear_flags(crypto_shash_tfm(tfm), flags);
}

static inline void *shash_desc_ctx(struct shash_desc *desc){
	return desc->__ctx;
}

static inline unsigned int crypto_aead_authsize(struct crypto_aead *tfm){
	return tfm->authsize;
}

static inline unsigned int crypto_sync_aead_authsize(struct crypto_sync_aead *tfm){
	return crypto_aead_authsize(&tfm->base);
}

static inline unsigned int crypto_aead_alg_maxauthsize(struct aead_alg *alg){
	return alg->maxauthsize;
}

static inline unsigned int crypto_aead_maxauthsize(struct crypto_aead *aead){
	return crypto_aead_alg_maxauthsize(crypto_aead_alg(aead));
}

static inline unsigned int crypto_sync_aead_maxauthsize(struct crypto_sync_aead *tfm){
	return crypto_aead_maxauthsize(&tfm->base);
}

static inline bool crypto_shash_block_only(struct crypto_shash *tfm){
	return crypto_shash_alg(tfm)->base.cra_flags & CRYPTO_AHASH_ALG_BLOCK_ONLY;
}

static int __crypto_shash_init(struct shash_desc *desc){
	struct crypto_shash *tfm = NULL;

	tfm = desc->tfm;

	if (crypto_shash_block_only(tfm)) {
		u8 *buf = shash_desc_ctx(desc);

		buf += crypto_shash_descsize(tfm) - 1;
		*buf = 0;
	}

	return crypto_shash_alg(tfm)->init(desc);
}

static void crypto_shash_exit_tfm(struct crypto_tfm *tfm){
	struct crypto_shash *hash = __crypto_shash_cast(tfm);
	struct shash_alg *alg = crypto_shash_alg(hash);

	alg->exit_tfm(hash);
}

static inline bool crypto_shash_alg_has_setkey(struct shash_alg *alg){
	return alg->setkey != shash_no_setkey;
}

static inline bool crypto_shash_alg_needs_key(struct shash_alg *alg){
	return crypto_shash_alg_has_setkey(alg) && !(alg->base.cra_flags & CRYPTO_ALG_OPTIONAL_KEY);
}

static void shash_set_needkey(struct crypto_shash *tfm, struct shash_alg *alg){
	if (crypto_shash_alg_needs_key(alg)){
		crypto_shash_set_flags(tfm, CRYPTO_TFM_NEED_KEY);
	}
}

static int crypto_shash_init_tfm(struct crypto_tfm *tfm){
	int ret = 0;
	struct crypto_shash *hash = __crypto_shash_cast(tfm);
	struct shash_alg *alg = crypto_shash_alg(hash);

	shash_set_needkey(hash, alg);

	if (alg->exit_tfm){
		tfm->exit = crypto_shash_exit_tfm;
	}

	if (!alg->init_tfm){
		return ret;
	}

	ret = alg->init_tfm(hash);
	return ret;
}

static void crypto_shash_free_instance(struct crypto_instance *inst){
	struct shash_instance *shash = shash_instance(inst);

	shash->free(shash);
}

static inline void *crypto_alloc_tfm(const char *alg_name, void *frontend, u32 type, u32 mask){
	const struct crypto_type crypto_shash_type = {
		.extsize = crypto_alg_extsize,
		.init_tfm = crypto_shash_init_tfm,
		.free = crypto_shash_free_instance,
		.maskclear = ~CRYPTO_ALG_TYPE_MASK,
		.maskset = CRYPTO_ALG_TYPE_MASK,
		.type = CRYPTO_ALG_TYPE_SHASH,
		.tfmsize = offsetof(struct crypto_shash, base),
		.algsize = offsetof(struct shash_alg, base),
	};

	return crypto_alloc_tfm_node(alg_name, &crypto_shash_type, type, mask, 0);
}

static struct crypto_aead *aead_key_setup_encrypt(const char *alg, const u8 key[], size_t key_len, size_t mic_len){
	int ret = 0;
	struct crypto_aead *tfm = NULL;

	tfm = crypto_alloc_aead(alg, 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(tfm)){
		return tfm;
	}

	ret = crypto_aead_setkey(tfm, key, key_len);
	if (ret){
		goto free_aead;
	}

	ret = crypto_aead_setauthsize(tfm, mic_len);
	if (ret){
		goto free_aead;
	}

	return tfm;

free_aead:
	crypto_free_aead(tfm);
	return ERR_PTR(ret);
}

static inline void crypto_aead_set_reqsize(struct crypto_aead *aead, unsigned int reqsize){
	aead->reqsize = reqsize;
}

static inline struct crypto_aead *__crypto_aead_cast(struct crypto_tfm *tfm){
	return container_of(tfm, struct crypto_aead, base);
}

static void crypto_aead_exit_tfm(struct crypto_tfm *tfm){
	struct crypto_aead *aead = __crypto_aead_cast(tfm);
	struct aead_alg *alg = crypto_aead_alg(aead);

	alg->exit(aead);
}

static inline void crypto_aead_set_flags(struct crypto_aead *tfm, u32 flags){
	crypto_tfm_set_flags(crypto_aead_tfm(tfm), flags);
}

static int crypto_aead_init_tfm(struct crypto_tfm *tfm){
	int ret = 0;
	struct crypto_aead *aead = __crypto_aead_cast(tfm);
	struct aead_alg *alg = crypto_aead_alg(aead);

	crypto_aead_set_flags(aead, CRYPTO_TFM_NEED_KEY);
	crypto_aead_set_reqsize(aead, crypto_tfm_alg_reqsize(tfm));

	aead->authsize = alg->maxauthsize;

	if (alg->exit){
		aead->base.exit = crypto_aead_exit_tfm;
	}

	if (alg->init){
		ret = alg->init(aead);
		return ret;
	}
	return ret;
}

static void *crypto_alloc_tfmmem(struct crypto_alg *alg, struct crypto_type *frontend, int node, int gfp){
	unsigned int total = 0;
	unsigned int tfmsize = 0;
	char *mem = NULL;
	struct crypto_tfm *tfm = NULL;

	tfmsize = frontend->tfmsize;
	total = tfmsize + sizeof(*tfm) + frontend->extsize(alg);

	mem = malloc(total);
	if (mem == NULL){
		return ERR_PTR(-ENOMEM);
	}

	tfm = (struct crypto_tfm *)(mem + tfmsize);
	tfm->__crt_alg = alg;
	tfm->node = node;
	refcount_set(&tfm->refcnt, 1);

	return mem;
}

static int setkey_unaligned(struct crypto_cipher *tfm, const u8 *key, unsigned int keylen){
	int ret = 0;
	unsigned long absize = 0;
	struct cipher_alg *cia = crypto_cipher_alg(tfm);
	unsigned long alignmask = crypto_cipher_alignmask(tfm);
	u8 *buffer = NULL;
	u8 *alignbuffer = NULL;

	absize = keylen + alignmask;
	buffer = malloc(absize);
	if (buffer == NULL){
		ret = -ENOMEM;
		return ret;
	}

	alignbuffer = (u8 *)ALIGN((unsigned long)buffer, alignmask + 1);
	memcpy(alignbuffer, key, keylen);
	ret = cia->cia_setkey(crypto_cipher_tfm(tfm), alignbuffer, keylen);
	kfree_sensitive(buffer);
	return ret;

}
