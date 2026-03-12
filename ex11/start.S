.section ".text.boot"
.global _start
_start:
    // TODO: Clear the .bss section
    la t0, __bss_start
    la t1, __bss_stop
    ble t1, t0, clear_bss_done

clear_bss:
    sd zero, (t0)
    add t0, t0, 8
    blt t0, t1, clear_bss
    // TODO: Set up the stack pointer

clear_bss_done:
    la sp, _end
    andi sp, 0xffffffffffffff00
    tail start_kernel
