	MOV AL, 0H
	MOV BL, 5H
HERE:	ADD AL, BL
	DEC BL
	CMP BL, 01H
	JAE SKIP
SKIP:	LOOP HERE
END:	HLT