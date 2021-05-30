

;SYNTAX CALL adresse_son (u8*) ,longueur (u16) ,volume (u8) ,vitesse (u8)
; volume 0 (min) to 15 (max)
; vitesse 2 to 15 (2=faster, 8=normal 15=very slow)
; SDCC doc about how to call assembly
;Default calling convention
;All parameters are passed on the stack, in right-to-left order (the last parameter is first put on the stack etc).
;Return values are passed in registers. 8-bit return values should be passed in L, 16-bit values in HL, 32-bit values in DEHL.

_LECTURE_SON::
LECTURE_SON:
	push IX 
	ld ix,#0
	add ix,sp
	ex   af, af'  ; CK Added else corrupt C stack
	exx
	push af
	push bc
	push iy	
	;adresse du son
	LD H,5(IX)
	LD L,4(IX)
	;longueur
	LD D,7(IX)
	LD E,6(IX)
	;volume
	LD A,8(IX)
	LD(voxvol+1),A
	;vitesse
	LD A,9(IX)
	LD(voxvit+1),A
	DI
loopvox:
	LD C,#0x08
	CALL Lecture
	INC HL
	DEC DE
	LD A,D
	OR E
	JR NZ,loopvox
	EI
	pop iy
	pop bc
	pop af
	exx
	ex af,af'		
	pop IX
	RET
;-------------------------------
Lecture:
	PUSH DE
	LD E,(HL)
vox3:
	SLA E
	LD A,#0
	JR NC,vox1
voxvol:
	LD A,#15		;volume
vox1:
	LD B,#0xF4
	LD D,#9
	OUT (C),D
	LD D,A
	LD B,#0xF6
	IN A,(C)
	OR #0xC0
	OUT (C),A
	AND #0x3F
	OUT (C),A
	LD B,#0xF4
	OUT (C),D
	LD B,#0xF6
	LD D,A
	OR #0x80
	OUT (C),A
	OUT (C),D
voxvit:
	LD A,#5		;vitesse
vox2:
	DEC A
	JR NZ,vox2
	DEC C
	JR NZ,vox3
	POP DE
	RET