extern char uart_getc(void);
extern void uart_putc(char c);
extern void uart_puts(const char* s);
extern void uart_hex(unsigned long h);
extern void* kmalloc(unsigned long size);
extern void* alloc_page();

#define STACK_SIZE 0x1000
#define LIST_ENTRY(ptr, type, memb)  ((type*)((unsigned long)ptr-(unsigned long)&(((type*)0)->memb)))

typedef struct list_head {
    struct list_head* prev;
    struct list_head* next;
}LIST_HEAD;

struct task_struct {
    struct thread_struct {
        unsigned long ra;
        unsigned long sp;
        unsigned long s[12];
    } thread;
    int pid;
    unsigned long kernel_sp;
    unsigned long user_sp;
    unsigned long stack;
    struct task_struct* prev;
    struct task_struct* next;
};

static inline void list_pop_front(LIST_HEAD* list_head) {
    list_head->next->next->prev = list_head;
    list_head->next = list_head->next->next;
}
static inline void list_push_back(LIST_HEAD* list_head, LIST_HEAD* new) {
    new->next = list_head;
    new->prev = list_head->prev;
    list_head->prev->next = new;
    list_head->prev = new;
}
static inline void list_push_front(LIST_HEAD* list_head, LIST_HEAD* new) {
    new->next = list_head->next;
    new->prev = list_head;
    list_head->next->prev = new;
    list_head->next = new;
}

static int nr_threads = 0;
static struct task_struct* run_queue;

static void enqueue(struct task_struct** queue, struct task_struct* task) {
    if (*queue == 0) {
        *queue = task;
        task->prev = task;
        task->next = task;
    } else {
        // struct task_struct* tail = (*queue)->next;
        // (*queue)->next = task;
        // task->next = tail;

        task->next = (*queue);
        task->prev = (*queue)->prev;
        (*queue)->prev->next = task;
        (*queue)->prev = task;
        (*queue) = task;
    }
}

struct task_struct* get_current() {
    register struct task_struct* current asm("tp");
    return current;
}

extern void switch_to(struct task_struct* prev, struct task_struct* next);

void schedule() {
    // TODO: Implement this function
    // caller-saved register 可以動到，但 callee-saved register 不行
    register struct task_struct* prev asm("tp");
    struct task_struct* next;
    if(prev == run_queue){
        run_queue = prev->next;
    }
    else{
        // remove itself
        prev->next->prev = prev->prev;
        prev->prev->next = prev->next;
        // push to last
        prev->next = run_queue;
        prev->prev = run_queue->prev;
        run_queue->prev->next = prev;
        run_queue->prev = prev;
    }
    
    next = run_queue;
    switch_to(prev, next);
}

void idle() {
    while (1) {
        // kill_zombies();
        schedule();
    }
}

void foo() {
    for (int i = 0; i < 5; i++) {
        uart_puts("Process ID: ");
        uart_hex(get_current()->pid);
        uart_puts(" ");
        uart_hex(i);
        uart_puts("\n");
        for (int i = 0; i < 1000000; i++)
            ;
        schedule();
    }
    while (1)
        ;
}

struct task_struct* kthread_create(void (*threadfn)()) {
    struct task_struct* task = kmalloc(sizeof(struct task_struct));
    task->pid = nr_threads++;
    task->stack = (unsigned long)alloc_page();
    task->thread.ra = (unsigned long)threadfn;
    task->thread.sp = task->stack + STACK_SIZE;
    enqueue(&run_queue, task);
    return task;
}

void start_kernel() {
    uart_puts("\nStarting kernel ...\n");
    /* Initialize the thread pointer */
    asm volatile("move tp, %0" : : "r"(kthread_create(idle)));
    for (int i = 0; i < 3; i++)
        kthread_create(foo);
    idle();
}

void do_trap() {
    uart_puts("Kernel panic - do_trap\n");
    unsigned long scause, sepc;
    asm volatile(
        "csrr %0, scause;"
        "csrr %1, sepc;"
        : "=r"(scause), "=r"(sepc)
    );
    uart_hex(scause);
    uart_puts("\n");
    uart_hex(sepc);
    uart_puts("\n");
    while (1)
        ;
}
