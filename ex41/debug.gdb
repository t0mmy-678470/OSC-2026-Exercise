file kernel.elf
target remote :1234
b *0x8020020c
c
b *$sepc
