
; invalid registers
add 1, $2, $3
add $3	1, $2, $3
add $33, $2, $3
add $-1, $2, $3
jmp $register, $2, $3

; no such label
.entry nosuchlabel

; both extern and entry
.extern externentry
.entry externentry

; invalid commands params (missing params, access params)
add
add $1
add $1, $2
add $1, $2, $3, $4
jmp
stop $1
move $1,$2, $3

; invalid command parameter types
add text, $2 ,$3
move $1, text