; ------------------------------------------------
; Read function by Julien Nevo. (adapted for SDCC)
; Important : Motor on and off should be call manually. (no need to turn it off between two consecutive files)
; ------------------------------------------------

.equ DIRFSECT,0xC1
.equ NBMAXENT,4
.equ DIRTRACK, 0

_loadlevel::
LOADLEVEL:
	di
	ex   af, af'  ; CK Added else corrupt C stack
	exx
	push af
	push bc
	push ix
	push iy
    ld hl,#0xc9fb
    ld (0x38),hl
    ;call FDCON
    xor a
    ld b,#0
    call FDCVARS
    ld hl,#MYFILE
    ld de,#0x9C00 ;DE=Where to load it
    ld bc,#0x9A00 ;BC=#0x200 buffer (512bytes).    
	call LOADFILE
 	jr nc,ERROR
    ;call FDCOFF
	pop iy
	pop ix
	pop bc
	pop af
	exx
	ex af,af'	
    ret  
_levelname:: ; will be used to change level name
MYFILE:  ; MYFILE defb "TEXT    BIN" 84 69 88 84    DATA 68 65 84 65 (decimal) "LEVELXX BIN"
    .db 76 ; L
    .db 69 ; E
    .db 86 ; V
    .db 69 ; E
    .db 76 ; L
    .db 88 ; X
    .db 88 ; X
    .db 32
    .db 66 ; B
    .db 73 ; I
    .db 78 ; N
	
	;------------------------------------------------------	
ERROR: 
    ld bc,#0x7f10 ; CK : Screen border RED
    out (c),c
    ld c,#0x4c
    out (c),c
	call FDCOFF
	pop iy
	pop ix
	pop bc
	pop af
	exx
	ex af,af'	
    ret 	
	
	;------------------------------------------------------
_loadmusic::
LOADMUSIC:
	di
	ex   af, af'  ; CK Added else corrupt C stack
	exx
	push af
	push bc
	push ix
	push iy
    ld hl,#0xc9fb
    ld (0x38),hl
    ;call FDCON
    xor a
    ld b,#0
    call FDCVARS
    ld hl,#MYFILEMUSIC
    ld de,#0x0150 ;DE=Where to load it
    ld bc,#0x9A00 ;BC=#0x200 buffer (512bytes).
	call LOADFILE
 	jr nc,ERROR
    ;call FDCOFF
	pop iy
	pop ix
	pop bc
	pop af
	exx
	ex af,af'		
    ret 
_musicname:: ; will be used to change music name
MYFILEMUSIC:  ; MYFILE defb "TEXT    BIN" 84 69 88 84    DATA 68 65 84 65 (decimal) "LEVELXX BIN"
    .db 78 ; N
    .db 73 ; I
    .db 78 ; N
    .db 74 ; J
    .db 65 ; A
    .db 88 ; X
    .db 88 ; X
    .db 32
    .db 66 ; B
    .db 73 ; I
    .db 78 ; N	
	;------------------------------------------------------
_loadtitle::
LOADTITLE:
	di
	ex   af, af'  ; CK Added else corrupt C stack
	exx
	push af
	push bc
	push ix
	push iy
    ld hl,#0xc9fb
    ld (0x38),hl
    ;call FDCON
    xor a
    ld b,#0
    call FDCVARS
    ld hl,#MYFILETITLE
    ld de,#0xc000 ;DE=Where to load it
    ld bc,#0x9A00 ;BC=#0x200 buffer (512bytes).  
    call LOADFILE
	jr nc,ERROR
    ;call FDCOFF
	pop iy
	pop ix
	pop bc
	pop af
	exx
	ex af,af'	
    ret 
MYFILETITLE:  ; MYFILE defb "TEXT    BIN" 84 69 88 84    DATA 68 65 84 65 (decimal) "LEVELXX BIN"
    .db 84 ; T
    .db 73 ; I
    .db 84 ; T
    .db 76 ; L
    .db 69 ; E
    .db 32 ; 
    .db 32 ; 
    .db 32 ;
    .db 83 ; S
    .db 67 ; C
    .db 82 ; R		

	
FDCVARS: 
    ld (FDCDRIVE),a
    ld c,a
    ld a,b
    ld (FDCHEAD),a
    rla 
    rla 
    and #0b100
    or c
    ld (FDCIDDR),a
    ret 
;Turn FDC on and wait a bit.	
_asm_FdcOn::
FDCON: 
    ld a,(FDCMOTOR)
    or a
    ret nz
    inc a
    ld (FDCMOTOR),a
    ld bc,#0xFA7E
    ld a,#1
    out (c),a
    ei ;Wait for motor to get full speed
    ld bc,#300
WAIT: 
    halt
    dec bc
    ld a,b
    or c
    jr nz,WAIT
    di
    ret
;Turn FDC off.	
_asm_FdcOff::
FDCOFF: 
    xor a
    ld (FDCMOTOR),a
    ld bc,#0xFA7E
    xor a
    out (c),a
    ret 
;Recalibrate current drive.
;RECALIBR: 
;    call RECALIB2
;    call RECALIB2
;    ret 
;RECALIB2: 
;    ld a,#0b00000111
;    call PUTFDC
;    ld a,(FDCIDDR)
;    call PUTFDC
;    jr WAITEND

;Load a file
;HL=Filename
;DE=Where to load it
;BC=#200 buffer
;RET=A=state. 0=ok  1=disc missing  2=read fail
;3=file not found	
LOADFILE: 
    ld (PTFILENM),hl
    ld (LOADWHER),de
    ld (ADBUFFER),bc
    call BUILDTAB
	jr c,LF_TABOK
	xor a
	ret
LF_TABOK:
	ld a,#128
	ld (SKIPBYTE+1),a	
	
;Reading the file		
    ld ix,#TABSECTS
LOADLP: 
    ld a,+0(ix)
    cp #0xff
    jr z,LOADFOK
    cp #0xfe
    jr z,LOADNEXT
    ld b,+1(ix)
    push ix
	ld hl,(LOADWHER)	
    call READSECT
    pop ix
    ret nc
    ld (LOADWHER),hl
    ld hl,(TOREAD)
    ld a,l
    or h
    jr z,LOADFOK
    xor a
    ld (SKIPBYTE+1),a
LOADNEXT: 
    inc ix
    inc ix
    jr LOADLP
LOADFOK: 
    scf
    ret 
;Wait for the end of the current instruction (using ST0).	
WAITEND: 
    ld a,#0b00001000
    call PUTFDC
    call GETFDC ;Get ST0
    ld (ST0),a
    call GETFDC
    xor a
    ld (ST1),a ;Reset ST1 and ST2
    ld (ST2),a
    ld a,(ST0)
    bit 5,a ;Instruction over ?
    jr z,WAITEND
    ret 
;Send data to FDC
;A=data	
PUTFDC: 
    ex af,af'
    ld bc,#0xFB7E
PUTFD2: 
    in a,(c)
    jp p,PUTFD2
    ex af,af'
    inc c
    out (c),a
    ret 
;Get data from FDC
;Ret = A=FDC data	
GETFDC: 
    ld bc,#0xFB7E
GETFD2: 
    in a,(c)
    jp p,GETFD2
    inc c
    in a,(c)
    ret 
;Track change
;a=nb piste	
GOTOPIST: 
    push af
    ld a,#0b00001111
    call PUTFDC
    ld a,(FDCIDDR)
    call PUTFDC
    pop af
    call PUTFDC
    jp WAITEND
     
;Read sector.
;A=track
;B=ID sector
;RET=A=state.Carry=1=ok A=0=ok  1=disc missing  2=read fail
;3=file not found
;HL=Where new data should be loaded (LOADWHER)	
READSECT: 
	ld (RSTarget+1),hl
    ld (RSPIST+1),a
    push bc
    call GOTOPIST
;INSTRUCTION : 9 octets
;1 COMMANDE  : &46 - %01000101
;2 DRIVE     : De 0 à 3 pour A à D
;3 Numéro de piste (Selon ID)
;4 Adresse de tête (Selon ID)
;5 Numéro du 1er secteur (Selon ID)
;6 Taille du secteur (Selon ID)
;7 Numéro du dernier secteur à lire
;8 GAP d'écriture (Ne pas confondre avec GAP format)
;9 Longueur secteur : En principe toujours &FF	
    ld a,#0b01000110 ; Command "Read sector"
    call PUTFDC
    ld a,(FDCIDDR) ;ID drive
    call PUTFDC
RSPIST: 
    ld a,#0 ;track
    call PUTFDC
    xor a ;head
    call PUTFDC
    pop bc ;ID sect
    ld a,b
    push af
    call PUTFDC
    ld a,#2 ;size
    call PUTFDC
    pop af ;last sect to read
    call PUTFDC
    ld a,#0x52 ;GAP
    call PUTFDC
    ld a,#0xFF
    call PUTFDC
    ld bc,#0xFB7E ; Information état FDC
SKIPBYTE: 
    ld a,#0 ;If header, skip it
    or a
    jr z,RSAVLOOP
    ld e,a
    ld hl,#BUFHEAD
;Read bytes to the header buffer.	
RSSKIPLP: 
    in a,(c) ;FDC ready for transf ?
    jp p,RSSKIPLP
    and ##0b00100000 ;FDC performing ?
    jr z,RSFIN
    inc c
    in a,(c)
    ld (hl),a
    inc hl
    dec c
    dec e
    jr nz,RSSKIPLP
    ld hl,(BUFHEAD+64) ;get filesize
    ld (TOREAD),hl
	;Normal reading code.
RSAVLOOP: 
RSTarget:	
	ld hl,#0
    ld de,(TOREAD)
RSLOOP: 
    in a,(c) ;FDC ready for transf ?
    jp p,RSLOOP
    and #0b00100000 ;FDC performing ?
    jr z,RSFIN
    inc c
    in a,(c) ; get data from port FDC
    ld (hl),a ; copy to destination buffer
    inc hl
    dec c
    dec de
    ld a,e
    or d
    jr nz,RSLOOP
;Reading with saving. Done if end of file but sectors left.	
RSWASTE: 
    in a,(c) ;FDC ready for transf ?
    jp p,RSWASTE
    and #0b00100000 ;FDC performing ?
    jr z,RSFIN
    inc c
    in a,(c)
    dec c
    jr RSWASTE
;Reading instr result	
RSFIN: 
    ld (TOREAD),de
    call GETFDC
    ld (ST0),a
    call GETFDC
    ld (ST1),a
    call GETFDC
    ld (ST2),a
    call GETFDC
    call GETFDC
    call GETFDC
    call GETFDC
;Test errors.
;ret= Carry=1=ok a=0=ok  a=1=disc missing  2=read fail
;ute ST0, ST1, ST2	
    ld a,(ST0)
    bit 7,a
    jr nz,TESTEJEC ;no disc
    bit 3,a
    jr nz,TESTEJEC ;no disc
    bit 4,a
    jr nz,TESTFAIL ;Read fail
    ld a,(ST1)
    and #0b00110111
    jr nz,TESTFAIL
    ld a,(ST2)
    and #0b00110000
    jr nz,TESTFAIL
;No error.
    scf
    ret 
TESTEJEC: 
    ld a,#1
    or a
    ret 
TESTFAIL: 
    ld a,#2
    or a
    ret 
ERRFNF: 
    ld a,#3
    or a
    ret 
;Search a file in the AMSDOS directory, and
;build the sector table.
;RET=A=state. 0=ok  1=disc missing  2=read fail
;3=file not found	
BUILDTAB: 
    ;.db 0xed, 0xff ; breackpoint
    ld a,#DIRFSECT ;First sector of the directory #c1
    ld (BTSECT+1),a
    add a,#4
    ld (BTESECT+1),a
    xor a
    ld (FILFOUND),a
    ld (SKIPBYTE+1),a
    ld hl,#0xffff
    ld (TOREAD),hl
    ld hl,#TABSECTS
    ld de,#TABSECTS+1
    ld bc,#TABSECTF-TABSECTS-1 ; Size of buffer
    ld (hl),#0xfe
    ldir ; Repeats LDI (LD (DE),(HL), then increments DE, HL, and decrements BC) until BC=0. Note that if BC=0 before this instruction is called, it will loop around until BC=0 again.
BTLOOP: 
    ld a,#DIRTRACK ; track DIRTRACK = 0
BTSECT: 
    ld b,#0xc1 ; ID sector CK : the value will be automodified
	ld hl,(ADBUFFER)
    call READSECT ;HL=Where new data should be loaded (LOADWHER)
    ret nc
    ld hl,(ADBUFFER)
    ld (BTBUFF+1),hl
;Search in the loaded sector the right entry(ies)	
    ld b,#16
BTENTLP: 
    push bc
    ld hl,(PTFILENM)
BTBUFF: 
    ld de,#0
    ld a,(de)
    cp #0xe5 ;ignore deleted files
    jr z,BTNEXT
    inc de
    call CMP
    call c,BTFOUND
BTNEXT: 
    ld hl,(BTBUFF+1)
    ld de,#32
    add hl,de
    ld (BTBUFF+1),hl
    pop bc
    djnz BTENTLP
;Next sector	
    ld a,(BTSECT+1)
    inc a
    ld (BTSECT+1),a
BTESECT:  
    cp #0xc5 ; CK : the track value will be automodified
    jr nz,BTLOOP
    ld a,(FILFOUND)
    or a
    jr z,BTNOFND
;No error.
    scf
    ret 
BTNOFND: 
    ld a,#3 ;File not found
	or a            ;Shoudln't be needed.
    ret 
;Right entry found.	
BTFOUND: 
    ld a,#1
    ld (FILFOUND),a
    ld ix,(BTBUFF+1)
    ld a,+12(ix)
    ld l,a
    ld h,#0
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    add hl,hl
    ld de,#TABSECTS
    add hl,de
    ex de,hl
    .db 253 ; ??  defb #fd : ld l,e
    .db 107
    .db 253 ; ?? defb #fd : ld h,d
    .db 98
    ld de,#16
    add ix,de
;ix=source
;iy=dest	
    jr CONV

;Compare two strings. 7th bit to 0.
;DE=     buffer   HL=filename
;RET=    Carry=OK	
CMP: 
    ld b,#11
CM2: 
    ld a,(de)
    res 7,a
    cp (hl)
    jr nz,CMPNOT
    inc hl
    inc de
    djnz CM2
    scf
    ret 
	
CMPNOT: 
    or a
    ret 
	
;Convert block table to table of tracks+sects
;IX=src  IY=dest	
CONV: 
    ld b,#16 ;Reading 16 blocks max
CVNEXT: 
    ld a,+0(ix) ;read block number
    or a
    ret z
    inc ix
    push bc
    ld l,a
    ld h,#0
    add hl,hl
    call DIV9
    ld +0(iy),c ;get track
;  sect1	
    ld hl,(RESTE)
    ld a,l
    add a,#DIRFSECT ; DIRFSECT equ #c1
    ld +1(iy),a
; sect2	
    inc a
    cp #0xCA
    jr nz,CVP2
    ld a,#0xC1
    inc c
CVP2: 
    ld +2(iy),c
    ld +3(iy),a
 	ld de,#NBMAXENT ;ld de,#NBMAXENT ;  ld de,4
    add iy,de
    pop bc
    djnz CVNEXT
    ret 
	
DIV9: 
    ;ld de,#NBMAXENT+5 ;Not optimised. Who cares ?
	ld de,#NBMAXENT + #5
    ld c,#0
DIV91: 
    or a
    sbc hl,de
    jr c,DIV92
    inc c
    jr DIV91
DIV92: 
    add hl,de
    ld (RESTE),hl
    ret 
	
FDCMOTOR:  ;Drive motor (0-1)
    .db 0 
FDCDRIVE:  ;Drive used (0-3)
    .db 0
FDCHEAD:   ;Head used (0-1)
    .db 0
FDCIDDR:   ;Drive ID.
    .db 0
ST0: 
    .db 0
ST1: 
    .db 0
ST2: 
    .db 0
FILFOUND: 
    .db 0
RESTE: 
    .dw 0
LOADWHER: ;Where to load the file
    .dw 0
ADBUFFER: ;#200 buffer
    .dw 0
PTFILENM: ;Point on the filename
    .dw 0
TOREAD: ;Size (decrease)
    .dw 0
;Table where is build the sector table.
;One directory entry can contain 16 blocks, hence '16'
;One block=2 sectors, each one defined by a track+sect ID. Hence the '*4'.
TABSECTS: ;16*4*4, #0xfe
    .db #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe
	.db #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe
	.db #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe
	.db #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe
	.db #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe
	.db #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe
	.db #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe
	.db #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe
	.db #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe, #0xfe
 TABSECTF: 
    .dw 0xFFFF
BUFHEAD: ;Header put here
    .blkb 128