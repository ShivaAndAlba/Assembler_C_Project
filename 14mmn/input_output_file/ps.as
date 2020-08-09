; this is a legal comment
	;this is an illegal comment
.entry LIST
.extern W
main:
ma,n:
MAIN: 	add r3, LIST
LOOP: 	pnr #48
		lea W,r6
		inc r6
		mov r3,K
		sub r1,r4
		bne END
		cmp K,#-6
		bne &END
		dec W
.entry MAIN
		jmp &LOOP
		add L3,L3
END: 	stop
STR1: 	.string "abcd"

; this is a ilegal string
STR2: 	.string "a1c\"

LIST1:	.data 6, -9

LIST2:	.data a,-&
		.data -100
K:		.data 31
.extern L3
