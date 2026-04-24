extern char uart_getc(void);
extern void uart_putc(char c);
extern void uart_puts(const char* s);
extern void uart_hex(unsigned long h);

#define UART_BASE 0x10000000UL
#define UART_RBR  (unsigned char*)(UART_BASE + 0x0)
#define UART_THR  (unsigned char*)(UART_BASE + 0x0)
#define UART_IER  (unsigned char*)(UART_BASE + 0x1) // 0x1/0x4
#define UART_IIR  (unsigned char*)(UART_BASE + 0x2) // 0x2/0x8
#define UART_MCR  (unsigned char*)(UART_BASE + 0x4) // 0x4/0x10
#define UART_LSR  (unsigned char*)(UART_BASE + 0x5) // 0x5/0x14
#define LSR_DR    (1 << 0)
#define LSR_TDRQ  (1 << 5)
#define UART_IRQ  0x0a      // UART Interrupt Request

#define PLIC_BASE            0xc000000UL
#define PLIC_PRIORITY(irq)   (PLIC_BASE + (irq) * 4)
#define PLIC_ENABLE(hart)    (PLIC_BASE + 0x002080 + (hart) * 0x0100)
#define PLIC_THRESHOLD(hart) (PLIC_BASE + 0x201000 + (hart) * 0x2000)
#define PLIC_CLAIM(hart)     (PLIC_BASE + 0x201004 + (hart) * 0x2000)

unsigned long boot_cpu_hartid = 0;

void uart_init() {
    // TODO: Enable RX interrupt
    *(volatile unsigned char*)UART_IER |= 1<<0;
    // TODO: Enable UART interrupt
    *(volatile unsigned char*)UART_MCR |= 1<<3;
}

void irq_enable() {
    asm volatile("csrsi sstatus, (1 << 1)");
}

void enable_external_interrupt() {
    asm volatile(
        "li t0, (1 << 9);"
        "csrs sie, t0;");
}

void plic_init() {
    // TODO: Implement this function
    // (1) Set UART interrupt priority
    *(volatile int*)PLIC_PRIORITY(UART_IRQ) |= 1;
    // (2) Set UART interrupt enable for the boot hart
    *(volatile int*)(PLIC_ENABLE(boot_cpu_hartid)+UART_IRQ/32*4) |= 1<<(UART_IRQ%32);
    // (3) Set threshold for the boot hart
    *(volatile int*)PLIC_THRESHOLD(boot_cpu_hartid) &= 0;
    // (4) Enable external interrupts  
    enable_external_interrupt();
}

int plic_claim() {
    // TODO: Implement this function
    // for(int i=0;i<1024/32;i++){
    //     int irq = *(volatile int*)((unsigned long)PLIC_CLAIM(boot_cpu_hartid)+i);
    //     if(irq) {
    //         return i*32 + irq;
    //     }
    // }

    // return 0;
    return *(volatile int*)PLIC_CLAIM(boot_cpu_hartid);
}

void plic_complete(int irq) {
    // TODO: Implement this function
    // *(volatile unsigned int*)(PLIC_CLAIM(boot_cpu_hartid)+irq/32) |= irq%32;
    *(volatile unsigned int*)PLIC_CLAIM(boot_cpu_hartid) = irq;
}

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
    int irq = plic_claim();
    if (irq == UART_IRQ) {
        char c = *UART_RBR;
        // uart_putc(c == '\r' ? '\n' : c);
        uart_puts("scause: ");
        uart_hex(regs->scause);    
        uart_puts("\nspec: ");
        uart_hex(regs->sepc);
        uart_puts("\nstval: ");
        uart_hex(regs->stval);
        uart_puts("\nirq: ");
        uart_hex(irq);
        uart_puts("\n");
    }
    if (irq)
        plic_complete(irq);
}

void start_kernel() {
    uart_puts("\nStarting kernel ...\n");
    plic_init();
    uart_init();
    irq_enable();
    while (1)
        ;
}
