file kernel.elf
target remote :1234
b schedule
b enqueue
c
#b *(&uart_interrupt_handler+0xd000)
#add-symbol-file kernel.elf 0x8020d000
