; Line too long
this assembly line is too long to be an assembly line, and hence, it shuold never be supported

; duplicate labels
duplicate: add $1, $2, $3
duplicate: add $1, $2, $3

; out of range numbers
.dw 2147483648
.dw -2147483649
.dh 32768
.dh -32769
.db 128
.db -129

; Invalid data directives
.asciz 3
.asciz "test
.asciz test2
.db , 200
.dh 200, 
.dw 1000,,2000
 
; Unknown commands
.blup 5
invalid_commmand $20

; Invalid labels
THISLABELISVERYYYLONGGANDISLONGERTHAN32: add $1, $2, $3
1cantstartwithnumber: add $1, $2, $3
bne:
lets_check_underscores

; invalid extern and entry commands
.extern extern1, extern2
.entry entry1, entry2

