#if 0
uint8_t RotateDigit(uint8_t d)
{
    return (d & 0xc0) | ((d & 0x07) << 3) | ((d & 0x38) >> 3);
}
#else
uint8_t RotateDigit(uint8_t d) __naked
{
    (void)d;    // avoid "unused parameter" warning
    __asm
    push ar7
    // preserve bits 6-7
    mov a,#0xc0
    anl a,dpl
    mov r7,a
    // promote bits 0-2 three places
    mov a,#0x07
    anl a,dpl
    swap a
    rr a
    orl a,r7
    mov r7,a
    // demote bits 3-5 three places
    mov a,#0x38
    anl a,dpl
    rl a
    swap a
    orl a,r7
    mov dpl,a
    pop ar7
    ret
    __endasm;
}
#endif
