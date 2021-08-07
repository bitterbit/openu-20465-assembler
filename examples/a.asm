MAIN:   add $9,$5,$3
LOOP:   ori $2,-5,$9 
        la val1
        jmp Next
Next:   move $20,$4
        bgt $4,$2,END
        la K
        sw $0,4, $10
        bne $31,$9,LOOP 
        call val1
        jmp $4
END:    stop 
STR:    .asciz "aBcd"
LIST:   .db 6,-9
        .dh 27056
.entry  K 
K:     .dw -12,31
.extern val1
