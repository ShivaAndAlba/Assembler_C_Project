; this is a legal comment
	;this is an illegal comment
.entry LIST
.extern W
;main starts with lower case
main:
;lower case and illegal character
ma,n:
MAIN: 	add r3, LIST
LOOP: 	prn #48
        prn #49
;space between # and number
		prn # 49
;tab between # and number
		prn #   49
		lea W,r6
		inc r6
;too many operands
		inc r6,MAIN
;illegal command, does not exist
		kek #2,K
		mov r3,K
;too little operands
		mov r4
; too many operands
		sub r1,r4,r3
		bne END
		cmp K,#-6
		bne &END
		bne &W
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
