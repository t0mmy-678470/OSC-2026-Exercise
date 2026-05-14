extern char uart_getc(void);
extern void uart_putc(char c);
extern void uart_puts(const char* s);
extern void uart_hex(unsigned long h);
extern unsigned long _start, _end;
/* Memory map */
#define PAGE_OFFSET   0xffffffc000000000UL
#define PAGE_SIZE     (1UL << 12) 
#define PMD_SIZE      (1UL << 21)
#define PGD_SIZE      (1UL << 30)

/* VA bit-field shifts (Sv39) */
#define PGD_SHIFT     30 
#define PMD_SHIFT     21
#define PTE_SHIFT     12

#define ENTRIES_PER_TABLE  512

#define KERNEL_PGD_INDEX   ((PAGE_OFFSET >> PGD_SHIFT) & 0x1FF)

#define LINEAR_MAP_GIB     4

/* PTE descriptor bits (Sv39) */
#define PTE_V  (1UL << 0)  
#define PTE_R  (1UL << 1)
#define PTE_W  (1UL << 2)
#define PTE_X  (1UL << 3)
#define PTE_U  (1UL << 4)
#define PTE_G  (1UL << 5)
#define PTE_A  (1UL << 6)
#define PTE_D  (1UL << 7)

#define PROT_KERNEL  (PTE_V | PTE_R | PTE_W | PTE_X | PTE_G | PTE_A | PTE_D)
#define PROT_NON_LEAF  (PTE_V | PTE_G)
// #define PROT_MMIO    (PTE_V | PTE_R | PTE_W |   0   | PTE_G | PTE_A | PTE_D)

#define SATP_SV39           (8UL << 60)
#define MAKE_SATP(pgd_pa)   (SATP_SV39 | ((unsigned long)(pgd_pa) >> 12))

#define MAKE_PTE(pa, flags) ((((unsigned long)(pa)) >> 12) << 10 | (flags))

#define ID_MAP_idx  0
#define HH_MAP_idx  1
#define UART_MAP_idx 2
#define SUPER_L1 (0xffffffffffffffffUL<<21)

static unsigned long __attribute__((section(".data"), aligned(PAGE_SIZE)))
    pgd[ENTRIES_PER_TABLE] = { 0 };

static unsigned long __attribute__((section(".data"), aligned(PAGE_SIZE)))
    pmd[LINEAR_MAP_GIB][ENTRIES_PER_TABLE] = { { 0 } };

void setup_vm(void) {
    // TODO: Set up page tables for identity mapping and kernel mapping
    unsigned long uart_phy = 0x0000000010000000;
    unsigned long uart_vir = 0xffffffc010000000;
    unsigned long start_high_vir = PAGE_OFFSET + (unsigned long)&_start;

    // hight-half mapping
    pgd[(start_high_vir>>PGD_SHIFT)&0x1ff] = MAKE_PTE((unsigned long)&pmd[HH_MAP_idx], PROT_NON_LEAF);
    // identity mapping
    pgd[(((unsigned long)&_start)>>PGD_SHIFT)&0x1ff] = MAKE_PTE((unsigned long)&pmd[ID_MAP_idx], PROT_NON_LEAF);
    for(int i=0 ; i<=(&_end-&_start)/PAGE_SIZE ; i++) {
        unsigned long pa = (unsigned long)(&_start) + i*PAGE_SIZE;
        unsigned long va = pa + PAGE_OFFSET;
        // hight-half mapping
        pmd[HH_MAP_idx][(va>>PMD_SHIFT)&0x1ff] = MAKE_PTE(pa&SUPER_L1, PROT_KERNEL); 
        // identity mapping
        pmd[ID_MAP_idx][(pa>>PMD_SHIFT)&0x1ff] = MAKE_PTE(pa&SUPER_L1, PROT_KERNEL); 
    }
    // hight-half mapping
    pgd[(uart_vir>>PGD_SHIFT)&0x1ff] = MAKE_PTE((unsigned long)&pmd[UART_MAP_idx], PROT_NON_LEAF);
    pmd[UART_MAP_idx][(uart_vir>>PMD_SHIFT)&0x1ff] = MAKE_PTE(uart_phy&SUPER_L1, PROT_KERNEL); 

    asm volatile(
        "csrw satp, %0\n"
        "sfence.vma zero, zero\n"
        ::"r"(MAKE_SATP(pgd))
        :"memory"
    );
}

void drop_identity_map(void) {
    // TODO: Drop identity mapping
    // identity mapping
    unsigned long phy_start = (((unsigned long)&_start)-PAGE_OFFSET);
    pgd[(phy_start>>PGD_SHIFT)&0x1ff] = MAKE_PTE(0, 0);
    for(int i=0 ; i<=(&_end-&_start)/PAGE_SIZE ; i++) {
        unsigned long pa = (unsigned long)(&_start) + i*PAGE_SIZE - PAGE_OFFSET;
        // identity mapping
        pmd[ID_MAP_idx][(pa>>PMD_SHIFT)&0x1ff] = MAKE_PTE(0, 0); 
    }

    asm volatile(
        "sfence.vma zero, zero\n"
        :::"memory"
    );
}

void start_kernel(void) {
    uart_puts("\nStarting kernel at : ");
    uart_hex((unsigned long)start_kernel);
    uart_puts("\n");
    while (1) {
        uart_putc(uart_getc());
    }
}