.entry LOOP
.entry LENGTH
.extern L3
.extern W
MAIN: mov r5[r2],W
LOOP: jmp W
prn #-5
sub r1, r4
inc K
mov r7[r6],r3
bne L3
END: stop
LENGTH: .data 6,-9,15
K: .data 22
.string "abcdef"
