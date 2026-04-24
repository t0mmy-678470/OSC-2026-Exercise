extern char uart_getc(void);
extern void uart_putc(char c);
extern void uart_puts(const char* s);
extern void uart_hex(unsigned long h);
extern int hextoi(const char* s, int n);
extern unsigned long align(unsigned long n, unsigned long byte);
extern int memcmp(const void* s1, const void* s2, int n);
extern void* alloc_page();

// TODO: Check the RAM disk base address
#define INITRD_BASE 0xa0200000
#define STACK_SIZE  0x1000

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

int exec(const char* filename) {
    char* p = (char*)INITRD_BASE;
    while (memcmp(p + sizeof(struct cpio_t), "TRAILER!!!", 10)) {
        struct cpio_t* hdr = (struct cpio_t*)p;
        int namesize = hextoi(hdr->namesize, 8);
        int filesize = hextoi(hdr->filesize, 8);
        int headsize = align(sizeof(struct cpio_t) + namesize, 4);
        int datasize = align(filesize, 4);
        if (!memcmp(p + sizeof(struct cpio_t), filename, namesize)) {
            // TODO: Finish this function
            // go to U-Mode
            uart_puts("p = ");
            uart_hex((unsigned long)p);
            uart_puts("\n");
            unsigned long epc = align((unsigned long)p + sizeof(struct cpio_t) + namesize, 4);
            uart_puts("sepc set to ");
            uart_hex(epc);
            uart_puts("\n");
            asm volatile(
                // set sstatus: go to user mode
                "csrr t0, sstatus   \n"
                "li t1, 0x100       \n"
                "not t1, t1         \n"
                "and t0, t0, t1    \n"  // SPP = 1<<8 = 0x100
                "csrw sstatus, t0   \n"
                // set sepc: set user process addr
                "csrw sepc, %0      \n"
                "sret               \n"
                 : // output
                 : "r"((unsigned long)epc)  // input
                 : "t0", "t1", "memory");
        }
        p += headsize + datasize;
    }
    return -1;
}

// TODO: Define the trap frame structure
struct pt_regs {
    unsigned long ra;
    unsigned long sscratch;
    unsigned long gp;
    unsigned long tp;
    unsigned long t0;
    unsigned long t1;
    unsigned long t2;
    unsigned long s0;
    unsigned long s1;
    unsigned long a0;
    unsigned long a1;
    unsigned long a2;
    unsigned long a3;
    unsigned long a4;
    unsigned long a5;
    unsigned long a6;
    unsigned long a7;
    unsigned long s2;
    unsigned long s3;
    unsigned long s4;
    unsigned long s5;
    unsigned long s6;
    unsigned long s7;
    unsigned long s8;
    unsigned long s9;
    unsigned long s10;
    unsigned long s11;
    unsigned long t3;
    unsigned long t4;
    unsigned long t5;
    unsigned long t6;
    unsigned long sepc;
    unsigned long sstatus;
    unsigned long scause;
    unsigned long stval;
};

void do_trap(struct pt_regs* regs) {
    // TODO: Implement this function
    // (1) Print the sepc and scause registers
    // (2) Increment the sepc register by 4 for traps
    uart_puts("spec: ");
    uart_hex(regs->sepc);
    uart_puts(", scause: ");
    uart_hex(regs->scause);    
    uart_puts("\n");
    regs->sepc += 4;
}

void start_kernel() {
    uart_puts("\nStarting kernel ...\n");
    if (exec("prog.bin"))
        uart_puts("Failed to exec user program!\n");
    while (1) {
        uart_putc(uart_getc());
    }
}
