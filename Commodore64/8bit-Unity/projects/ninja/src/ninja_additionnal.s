;---------------------------------------------
;void __fastcall__ waitRaster(unsigned char raster);
;---------------------------------------------
			.export _waitRaster
			.export _detectSID

_waitRaster:			
			cmp $d012
			bne _waitRaster
			rts
			
; ----------------------------------------------

; page zero registers. Free (Tangent) From $57 to $60
samples = $57  ; and $58
count =   $59  ; and $5A	
lenght =  $5B  ; and $5C
speed  =  $5D; one byte for speed	
			
			.export _playsample
			
_playsample:
	;; Save the registers, so we can use them
	;; in the IRQ routine.
	pha
	tya
	pha
	txa
	pha
	
                sei
                
                ; disable ROM and enable RAM
                ;lda #$35
                ;sta 1
                
                ; screen off
				lda $d011
				sta $55 ; Save vic state
                ldx #0
                stx $d011

                ; load sample start address
                ;lda #<audiodata
                ;sta samples
                ;lda #>audiodata
                ;sta samples+1
				lda speed
				sta speed1+1
				sta speed2+1
                
                ; clear count
                stx count
                stx count+1
loop:
                ; check for samples end
                lda count
                cmp lenght
                bne loop2
                lda count+1
                cmp lenght+1
                beq endofsample
                ;beq initLoop ; global loop (replay sample endlessly)
loop2:
                ; load next sample
                ;lda (samples),x
                lda (samples,x)
                pha

                ; play low nibble and delay
                and #15
                sta $D418
                ;ldy #20 ; Speed. Default 20
speed1:				
				ldy #30
delay1:          
                dey
                bne delay1

                ; play high nibble and delay
                pla
                ror
                ror
                ror
                ror
                and #15
                sta $d418
                ;ldy #18 ; Speed. Default 20.
speed2:
				ldy #30
delay2:
				dey
                bne delay2

                ; increment sample count
                inc count
                bne inc1
                inc count+1
inc1:
                ; increment samples pointer
                inc samples
                bne loop
                inc samples+1
                jmp loop
endofsample:
                ; screen on
				;lda $d011
				;ora #$10 ; set bit 4 to 1
				;sta $d011
				ldx $55
				stx $d011 ; restore vic state
				
				cli
				
				; Restore registers from stack
				pla
				tax
				pla
				tay
				pla				
                rts		

; ----------------------------------------
; Will set 1 in count ($59) if SID is old.
; else 0
_detectSID:    
			sei
			lda #11
			sta $d011

;Disable bad-lines (by blanking the screen).  This prevents badlines
;from interfering with the detection process.

			 ;sid setup here!
			 lda #$20
			 sta $d40e
			 sta $d40f

;Set Oscillator 3's Frequency Control to $2020.  I just randomly chose
;this value when experimenting, and it worked, so I kept it.  The trick
;here is to set a value fast enough that the oscillator will make a
;number of cycles (so we can get a good sample of the values coming
;out) but not so fast that it might miss any of the "bursts" I was
;mentioning earlier.

			 lda #%00110001
			 sta $d412

;Combine the triangle and sawtooth waveforms and start the ADSR cycle.

			 ldx #0
			 stx high

loopsid:     
			 lda $d41b
			 cmp high
			 bcc ahead
			 sta high
ahead:
			 dex
			 bne loopsid

;This loop takes 256 samples of Oscillator 3's output, saving the
;highest value in location high.

			 lda #%00110000
			 sta $d412

;Stop Oscillator 3.

			 cli
			 lda #27
			 sta $d011

;			Turn the screen back on.

         lda high
		 bmi SID8580
		 ;bpl SID6581 
		 lda #1 ; 1 = old sid
		 sta count		 
         rts
		 
SID8580:
		 lda #0 ; 0 = new SID
		 sta count		 
		 rts

high:     
		.byte 0

