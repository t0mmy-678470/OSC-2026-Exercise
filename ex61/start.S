.section ".text.boot"
.global _start
_start:
    // TODO: Jump to high address
    la sp, _end
    call    setup_vm
    li a3, 0xffffffc000000000
    la a4, 1f
    add a4, a4, a3
    jr a4

.align 2
1:
    // TODO: load virtual address of sp
    add sp, sp, a3

    la      a3, __bss_start
    la      a4, __bss_stop
    ble     a4, a3, clear_bss_done
clear_bss:
    sd      zero, (a3)
    add     a3, a3, 8
    blt     a3, a4, clear_bss
clear_bss_done:
    call    drop_identity_map
    tail    start_kernel