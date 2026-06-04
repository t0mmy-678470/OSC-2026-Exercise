file kernel.elf
target remote :1234
add-symbol-file kernel.elf 0x80200000
b *0x80200000
c