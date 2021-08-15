.entry entrylabel
.extern externlabel

; use extern labels
call externlabel
jmp externlabel

; endless loop
loop: addi $1, 2, $3
jmp loop
call loop

; create some data
number: .db 5
bignumber: .dh 500
negativebignumber: .dh -2000
hugenumber: .dw 50000
string: .asciz "No lang like C lang"
numberlist: .db 1,2,3,4
.dw 1000,2000,3000
entrylabel: .asciz "wooohooo...."

; R commands 
add $1, $2, $3
sub $1, $2, $3
and $1, $2, $3
or $1, $2, $3
nor $1, $2, $3
move $1, $2
mvhi $1, $2
mvlo $1, $2

; I commands
addi $1, 2, $3
subi $1, 2, $3
andi $1, 2, $3
ori $1, 2, $3
nori $1, 2, $3

; branching!
bne $31,$32,loop
beq $31,$32,loop
blt $31,$32,loop
bgt $31,$32,loop

; Load and store
lb $1, 2, $3
lh $1, 2, $3
lw $1, 2, $3
sb $1, 2, $3
sh $1, 2, $3
sw $1, 2, $3

; J commands (only la and stop weren't used till now)
la string

stop