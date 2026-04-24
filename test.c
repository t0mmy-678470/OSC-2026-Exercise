#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cpio_t {
    char magic[6];
    char ino[8];
    char mode[8];
    char uid[8];
    char gid[8];
    char nlink[8];
    char mtime[8];
    char filesize[8];
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];
    char check[8];
};

#define NUM_PAGES 0x280000
int main() {
	
	printf("size: %ld\n", sizeof(struct cpio_t));

    return 0;
}


// /* Reuses the bits in struct page */
// struct slab {
// 	unsigned long __page_flags;

// 	struct kmem_cache *slab_cache;
//     struct {
//         union {
//             struct list_head slab_list;
// #ifdef CONFIG_SLUB_CPU_PARTIAL
//             struct {
//                 struct slab *next;
//                 int slabs;	/* Nr of slabs left */
//             };
// #endif
//         };
//         /* Double-word boundary */
//         struct {
//             void *freelist;		/* first free object */
//             union {
//                 unsigned long counters;
//                 struct {
//                     unsigned inuse:16;      // num of active objects
//                     unsigned objects:15;    // num of objects
//                     unsigned frozen:1;
//                 };
//             };
//         };
//     };
// 	unsigned int __unused;

// 	atomic_t __page_refcount;
// #ifdef CONFIG_MEMCG
// 	unsigned long memcg_data;
// #endif
// };

// /*
//  * When changing the layout, make sure freelist and tid are still compatible
//  * with this_cpu_cmpxchg_double() alignment requirements.
//  */
// struct kmem_cache_cpu {
// 	union {
// 		struct {
// 			void **freelist;	/* Pointer to next available object */
// 			unsigned long tid;	/* Globally unique transaction id */
// 		};
// 		freelist_aba_t freelist_tid;
// 	};
// 	struct slab *slab;	/* The slab from which we are allocating */
// #ifdef CONFIG_SLUB_CPU_PARTIAL
// 	struct slab *partial;	/* Partially allocated slabs */
// #endif
// 	local_lock_t lock;	/* Protects the fields above */
// #ifdef CONFIG_SLUB_STATS
// 	unsigned int stat[NR_SLUB_STAT_ITEMS];
// #endif
// };

// struct kmem_cache_node {
// 	spinlock_t list_lock;
// 	unsigned long nr_partial;
// 	struct list_head partial;
// // #ifdef CONFIG_SLUB_DEBUG
// 	atomic_long_t nr_slabs;
// 	atomic_long_t total_objects;
// 	struct list_head full;
// // #endif
// };

// /*
//  * Slab cache management.
//  */
// struct kmem_cache {
// #ifndef CONFIG_SLUB_TINY
// 	struct kmem_cache_cpu __percpu *cpu_slab;
// #endif
// 	/* Used for retrieving partial slabs, etc. */
// 	slab_flags_t flags;
// 	unsigned long min_partial;
// 	unsigned int size;		/* Object size including metadata */
// 	unsigned int object_size;	/* Object size without metadata */
// 	struct reciprocal_value reciprocal_size;
// 	unsigned int offset;		/* Free pointer offset */
// #ifdef CONFIG_SLUB_CPU_PARTIAL
// 	/* Number of per cpu partial objects to keep around */
// 	unsigned int cpu_partial;
// 	/* Number of per cpu partial slabs to keep around */
// 	unsigned int cpu_partial_slabs;
// #endif
// 	struct kmem_cache_order_objects oo;

// 	/* Allocation and freeing of slabs */
// 	struct kmem_cache_order_objects min;
// 	gfp_t allocflags;		/* gfp flags to use on each alloc */
// 	int refcount;			/* Refcount for slab cache destroy */
// 	void (*ctor)(void *object);	/* Object constructor */
// 	unsigned int inuse;		/* Offset to metadata */
// 	unsigned int align;		/* Alignment */
// 	unsigned int red_left_pad;	/* Left redzone padding size */
// 	const char *name;		/* Name (only for display!) */
// 	struct list_head list;		/* List of slab caches */
// #ifdef CONFIG_SYSFS
// 	struct kobject kobj;		/* For sysfs */
// #endif
// #ifdef CONFIG_SLAB_FREELIST_HARDENED
// 	unsigned long random;
// #endif

// #ifdef CONFIG_NUMA
// 	/*
// 	 * Defragmentation by allocating from a remote node.
// 	 */
// 	unsigned int remote_node_defrag_ratio;
// #endif

// #ifdef CONFIG_SLAB_FREELIST_RANDOM
// 	unsigned int *random_seq;
// #endif

// #ifdef CONFIG_KASAN_GENERIC
// 	struct kasan_cache kasan_info;
// #endif

// #ifdef CONFIG_HARDENED_USERCOPY
// 	unsigned int useroffset;	/* Usercopy region offset */
// 	unsigned int usersize;		/* Usercopy region size */
// #endif

// 	struct kmem_cache_node *node[MAX_NUMNODES];
// };


// struct page {
// 	unsigned long flags;		/* Atomic flags, some possibly
// 					 * updated asynchronously */
// 	/*
// 	 * Five words (20/40 bytes) are available in this union.
// 	 * WARNING: bit 0 of the first word is used for PageTail(). That
// 	 * means the other users of this union MUST NOT use the bit to
// 	 * avoid collision and false-positive PageTail().
// 	 */
// 	union {
// 		struct {	/* Page cache and anonymous pages */
// 			/**
// 			 * @lru: Pageout list, eg. active_list protected by
// 			 * lruvec->lru_lock.  Sometimes used as a generic list
// 			 * by the page owner.
// 			 */
// 			union {
// 				struct list_head lru;

// 				/* Or, for the Unevictable "LRU list" slot */
// 				struct {
// 					/* Always even, to negate PageTail */
// 					void *__filler;
// 					/* Count page's or folio's mlocks */
// 					unsigned int mlock_count;
// 				};

// 				/* Or, free page */
// 				struct list_head buddy_list;
// 				struct list_head pcp_list;
// 			};
// 			/* See page-flags.h for PAGE_MAPPING_FLAGS */
// 			struct address_space *mapping;
// 			union {
// 				pgoff_t index;		/* Our offset within mapping. */
// 				unsigned long share;	/* share count for fsdax */
// 			};
// 			/**
// 			 * @private: Mapping-private opaque data.
// 			 * Usually used for buffer_heads if PagePrivate.
// 			 * Used for swp_entry_t if PageSwapCache.
// 			 * Indicates order in the buddy system if PageBuddy.
// 			 */
// 			unsigned long private;
// 		};
// 		struct {	/* page_pool used by netstack */
// 			/**
// 			 * @pp_magic: magic value to avoid recycling non
// 			 * page_pool allocated pages.
// 			 */
// 			unsigned long pp_magic;
// 			struct page_pool *pp;
// 			unsigned long _pp_mapping_pad;
// 			unsigned long dma_addr;
// 			atomic_long_t pp_ref_count;
// 		};
// 		struct {	/* Tail pages of compound page */
// 			unsigned long compound_head;	/* Bit zero is set */
// 		};
// 		struct {	/* ZONE_DEVICE pages */
// 			/** @pgmap: Points to the hosting device page map. */
// 			struct dev_pagemap *pgmap;
// 			void *zone_device_data;
// 			/*
// 			 * ZONE_DEVICE private pages are counted as being
// 			 * mapped so the next 3 words hold the mapping, index,
// 			 * and private fields from the source anonymous or
// 			 * page cache page while the page is migrated to device
// 			 * private memory.
// 			 * ZONE_DEVICE MEMORY_DEVICE_FS_DAX pages also
// 			 * use the mapping, index, and private fields when
// 			 * pmem backed DAX files are mapped.
// 			 */
// 		};

// 		/** @rcu_head: You can use this to free a page by RCU. */
// 		struct rcu_head rcu_head;
// 	};

// 	union {		/* This union is 4 bytes in size. */
// 		/*
// 		 * If the page can be mapped to userspace, encodes the number
// 		 * of times this page is referenced by a page table.
// 		 */
// 		atomic_t _mapcount;

// 		/*
// 		 * If the page is neither PageSlab nor mappable to userspace,
// 		 * the value stored here may help determine what this page
// 		 * is used for.  See page-flags.h for a list of page types
// 		 * which are currently stored here.
// 		 */
// 		unsigned int page_type;
// 	};

// 	/* Usage count. *DO NOT USE DIRECTLY*. See page_ref.h */
// 	atomic_t _refcount;

// #ifdef CONFIG_MEMCG
// 	unsigned long memcg_data;
// #endif

// 	/*
// 	 * On machines where all RAM is mapped into kernel address space,
// 	 * we can simply calculate the virtual address. On machines with
// 	 * highmem some memory is mapped into kernel virtual memory
// 	 * dynamically, so we need a place to store that address.
// 	 * Note that this field could be 16 bits on x86 ... ;)
// 	 *
// 	 * Architectures with slow multiplication can define
// 	 * WANT_PAGE_VIRTUAL in asm/page.h
// 	 */
// #if defined(WANT_PAGE_VIRTUAL)
// 	void *virtual;			/* Kernel virtual address (NULL if
// 					   not kmapped, ie. highmem) */
// #endif /* WANT_PAGE_VIRTUAL */

// #ifdef LAST_CPUPID_NOT_IN_PAGE_FLAGS
// 	int _last_cpupid;
// #endif

// #ifdef CONFIG_KMSAN
// 	/*
// 	 * KMSAN metadata for this page:
// 	 *  - shadow page: every bit indicates whether the corresponding
// 	 *    bit of the original page is initialized (0) or not (1);
// 	 *  - origin page: every 4 bytes contain an id of the stack trace
// 	 *    where the uninitialized value was created.
// 	 */
// 	struct page *kmsan_shadow;
// 	struct page *kmsan_origin;
// #endif
// }