#define __SFR_OFFSET 0

#include "avr/io.h"
#include "buffer.h"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                      ;
;          RCAout.S               ;
;                                      ;
;    Created: 07/12/2020 09:09:33      ;
;    Author: ceptimus                  ;
;    Rewriten: 2025.03.01              ;
;    By Gilhad                         ;
;    Read PS/2 keys in free time       ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;	; SUPPRESS on PortD, pin 2 = D2
;	#define UNSUPPRESS CBI  DDRD, 2
;	#define SUPPRESS   SBI  DDRD, 2
#define UNSUPPRESS SBI PORTD, 2
#define SUPPRESS   CBI PORTD, 2
#define ECHO3 SBI PIND, 3

;    #define BUFFER_MASK 0x3F		; 64 byte buffer, I will use better algorythmus next time

#define D13  PINB, 5
#define D9  PINB, 1
#define A0  PINC, 0
#define A1  PINC, 1
#define Tx1 PIND, 1
#define Rx0 PIND, 0
#define D2  PIND, 2

#define CLOCK A0		; PS/2 clock
#define DATA A1			; PS/2 data
#define INSIDE D13		; PS/2 clock envelope (RC diode trick)

#define videosignal Tx1		; PIND, 1 (~1)
#define supress_pin D2		; PIND, 2 (~5)
#define sync_pin    D9		; PINB, 1 (~9)

.section .data
Data_register:	.byte	 0,0	; R18,R19	; for shifting data bits		;
Counter:	.byte	11	; R21		; we need 11 bits datagram		;
Status:		.byte	0	; R25		; - wait_fall+grab/wait_top/store	;
	; 0 - wait for fall and grab
	;	if grab, dec Counter, if Counter==0 set Status=2 else set Status=1
	;	if not INSIDE, reset all
	; 1 - wait for rise, if rise, set Status=0
	; 2 - save it
	;
	; interleaved PS/2 uses R18-R19, R21, R25 which LOOP	;## 9 cycles does not touch
	;			temporaly use R30-31(Z) and restore it
	;			temporaly use R24
	;

.section .text
; {{{ MACROs
;;; {{{ original modified loop
;;;	loop:                                                 cycles
;;;		NOP                                        // +1 =  1
;;;		NOP                                        // +1 =  2
;;;		NOP                                        // +1 =  3
;;;		NOP                                        // +1 =  4
;;;		LD R24, X+ ; get character from screen ram // +2 =  6
;;;		MOVW R30, R22 ; Z = fontSlice              // +1 =  7
;;;		ADD R30, R24                               // +1 =  8
;;;		LPM R20, Z ; fetch 8 pixels                // +3 = 11
;;;		STS UDR0, R20 ; send them to the USART     // +2 = 13
;;;		DEC R25                                    // +1 = 14
;;;		BRNE loop                                  // +2 = 16
;;; }}}
.macro LOOP	;## 9 cycles	; {{{
				; Uses R20, R22-R24, R26-27(X), R30-31(Z)
				;	* Need R22-R23,R26-R27(X)
				;	* Trashes R24,R20,R30-R31(Z)
				; Free to use R18-R19, R21, R25
;								   // 1..4
				LD R24, X+ ; get character from screen ram // +2 =  6		;# 2 cycles, 2 bytes
				MOVW R30, R22 ; Z = fontSlice              // +1 =  7		;# 1 cycles, 2 bytes
				ADD R30, R24 ;                             // +1 =  8		;# 1 cycles, 2 bytes
				LPM R20, Z ; fetch 8 pixels                // +3 = 11		;# 3 cycles, 2 bytes
				STS UDR0, R20 ; send them to the USART     // +2 = 13		;# 2 cycles, 4 bytes
;								   // 14..16
.endm	; }}}
; {{{					NOPx macros for waiting
.macro NOP2	;## 2 cycles
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
.endm
.macro NOP3	;## 3 cycles
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
.endm
.macro NOP4	;## 4 cycles
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
.endm
.macro NOP5	;## 5 cycles
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
.endm
.macro NOP7	;## 7 cycles
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
.endm
;								}}}
.macro LOOP7	;## 16 cycles
				NOP7	;## 7 cycles
				LOOP	;## 9 cycles
.endm
; }}}
.macro _4_L9_0	; {{{ 4 NOPs, 8 LOOPs and 7 NOPs total time consumation - reads PS/2 keys interleaved

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; {{{ intro + store - read registers, switch on action + start of store section
intro_lbl_\@:			; Load all our data to registers		;
				; 4 cycles to L(1)
	lds r18, Data_register	; r18 + r19 are data shifted in this order:	;		;# 2 cycles, 4 bytes
	lds r19, Data_register+1; Start Data[LSB] ... Data[MSB] Parity Stop	;		;# 2 cycles, 4 bytes
				; shifting it to right orders Data naturally	;
				LOOP	;## 9 cycles	; (1)
	lds r21, Counter	; How many bits to read yet			;		;# 2 cycles, 4 bytes
	lds r25, Status		; 0 wait for fall, 1 for rise, 2 store data	;		;# 2 cycles, 4 bytes

				; switch:
	cpi r25,0		; if Status is 0				;		;# 1 cycles, 2 bytes
	breq wait_fall_lbl_\@	; wait for fall and grab next bit		;		;# 1-2 cycles, 2 bytes
				NOP		;# 1 cycles, 1 bytes
				LOOP	;## 9 cycles	; (2)
	cpi r25,1		; if Status is 1				;		;# 1 cycles, 2 bytes
	breq wait_top_lbl_\@	; wait for rise					;		;# 1-2 cycles, 2 bytes
				; else Status is 2				;
			; L(2)+2
store_it_lbl_\@:		; Status = 2 - Store collected data and reset
				; trash r25 and r21 temporaly, as we know, what to reset them after this
				; we have 11 bits, need to save it
; shift back stop + parity bits
	lsl r19			; Shift out top bit to Carry			;		;# 1 cycles, 2 bytes
	rol r18			; Roll it in and shift out the last = Stop bit	;		;# 1 cycles, 2 bytes
	lsl r19			; and again					;		;# 1 cycles, 2 bytes
	rol r18			; discard Parity bit, now we have the Data in	;		;# 1 cycles, 2 bytes
				; and Start it in r19
				; r18 = data
				NOP		;# 1 cycles, 1 bytes
				LOOP	;## 9 cycles	; (3)
	rjmp store_it_2_lbl_\@	; too long for branches, reorganise ...		;		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; }}}
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; {{{ wait for fall (CLOCK was HIGH last time)
wait_fall_lbl_\@:	; L(1)+7
				LOOP	;## 9 cycles	; (2)
	sbic CLOCK		; if CLOCK is LOW, skip next instruction  (1/3) ;		;# 1-3 cycles, 2 bytes
	rjmp test_inside_lbl_\@	; check if we are still in datagram		;		;# 2 cycles, 2 bytes
				; else continue to grab_lbl_\@
grab_lbl_\@:		; L(2)+3
	lsr r18			; roll data right (to Carry)			;		;# 1 cycles, 2 bytes
	ror r19			; save what fall out				;		;# 1 cycles, 2 bytes
	sbic DATA		; if DATA is 1					;		;## 1-2 cycles, 2 bytes
	ori r18, 0x80		; insert it to top position			;		;# 1 cycles, 2 bytes
				LOOP	;## 9 cycles	; (3)
	ldi r25, 1		; next wait for rise of CLOCK			;		;# 1 cycles, 2 bytes
	dec r21			; one bit acquired, decrement Counter		;		;# 1 cycles, 2 bytes
	brne skip_over_1_\@	; if it was not last bit, continue		;		;# 1-2 cycles, 2 bytes
	ldi r25,2		; else next will be saving scan code (Status 2)	;		;# 1 cycles, 2 bytes
skip_over_1_\@:			;
	rjmp outro_L3_6_lbl_\@	; finish this path				;		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; }}}
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; {{{ wait for CLOCK rise back to HIGH
wait_top_lbl_\@:	; L(2)+3
	sbis CLOCK		; skip next if HIGH			  (1/3) ;		;# 1-3 cycles, 2 bytes
	rjmp outro_L2_6_lbl_\@	; still LOW, maybe next time ...		;		;# 2 cycles, 2 bytes
		; L(2)+6
	ldi r25, 0		; HIGH, next time wait for LOW (Status 0)	;		;# 1 cycles, 2 bytes
				LOOP	;## 9 cycles	; (3)
	rjmp outro_L3_2_lbl_\@	; finish this path				;		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; }}}
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; {{{ r18 is data, store it to buffer
store_it_2_lbl_\@:
				NOP3	;## 3 cycles
	lds r21, BIOS_buffer_head_ptr	; trash r21 = Counter, it will be reset later	;		;# 2 cycles, 4 bytes
				LOOP	;## 9 cycles	; (4)
	ldi ZL, lo8(BIOS_buffer)	; This must go uninterrupted, we need r30-r31  |;		;# 1 cycles, 2 bytes
	ldi ZH, hi8(BIOS_buffer)	; Head is address to use for new byte          |;		;# 1 cycles, 2 bytes
	add ZL, r21		; we store the data there and see, if there is |;		;# 1 cycles, 2 bytes
	adc ZH, r1		; space in buffer. It is safe, as there is at  |;		;# 1 cycles, 2 bytes
	st Z, r18		; least one unused position in buffer          |;		;# 2 cycles, 2 bytes
	inc r21			; Increment head pointer and wrap it around	;		;# 1 cycles, 2 bytes
				LOOP	;## 9 cycles	; (5)
	andi r21, BUFFER_MASK	; if necessary (64-1 = 0x3F) for circular behavior ;		;# 1 cycles, 2 bytes
	lds r24, BIOS_buffer_tail_ptr	; trash r24 for moment, LOOP will set it anyway |;		;# 2 cycles, 4 bytes
	cp r24, r21		; Compare head and tail                         |;		;# 1 cycles, 2 bytes
	breq put_char_ret_1_lbl_\@	; If equal, buffer is full, leave it so |;		;# 1-2 cycles, 2 bytes
	sts BIOS_buffer_head_ptr, r21	; Store updated head pointer and make it valid	;		;# 2 cycles, 4 bytes
				LOOP	;## 9 cycles	; (6) a)
	rjmp put_char_ret_2_lbl_\@	; still need to balance the times	;		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; }}}
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; {{{ test if we are inside the 11 bit datagram still
test_inside_lbl_\@:	; L(2)+3
	sbic INSIDE		; skip if we are no longer inside	  (1/3) ;		;# 1-3 cycles, 2 bytes
	rjmp outro_L2_6_lbl_\@	; we are inside, finish this path		;		;# 2 cycles, 2 bytes
	ldi r21, 11		; we are outside, scratch it all		;		;# 1 cycles, 2 bytes
					LOOP	;## 9 cycles	; (3)
	ldi r18, 0		; we need 11 bits of data, have nothing		;		;# 1 cycles, 2 bytes
	ldi r19, 0		; and we are already waiting for fall		;		;# 1 cycles, 2 bytes
	rjmp outro_L3_4_lbl_\@	;		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; }}}
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; {{{ balancing times for storing character
put_char_ret_1_lbl_\@:
				; buffer was full, we did not save the change	;
				NOP		;# 1 cycles, 1 bytes
				LOOP	;## 9 cycles	; (6) b)
				NOP		;# 1 cycles, 1 bytes
				NOP		;# 1 cycles, 1 bytes
put_char_ret_2_lbl_\@:
				; anyway we need reset for next scan code
	ldi r25, 1		; wait for rise of CLOCK first, then continue	;		;# 1 cycles, 2 bytes
	ldi r21, 11		; we want 11 bits				;		;# 1 cycles, 2 bytes
	ldi r18, 0		; and have no data so far			;		;# 1 cycles, 2 bytes
	ldi r19, 0		;						;		;# 1 cycles, 2 bytes
				NOP		;# 1 cycles, 1 bytes
				LOOP	;## 9 cycles	; (7)
	rjmp outro_L7_2_lbl_\@	; finish this path				;		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; }}}
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; {{{ finishing paths with some waiting and loops, then save actual status
outro_L2_6_lbl_\@:
				NOP		;# 1 cycles, 1 bytes
				LOOP	;## 9 cycles	; (3)
				NOP2	;## 2 cycles
outro_L3_2_lbl_\@:
				NOP2	;## 2 cycles		;# 1 cycles, 1 bytes
outro_L3_4_lbl_\@:
				NOP2	;## 2 cycles		;# 1 cycles, 1 bytes
outro_L3_6_lbl_\@:
				NOP		;# 1 cycles, 1 bytes
				LOOP	;## 9 cycles	; (4)
	sbi PCIFR,PCIF0		; clear pending PinChange  events		;		;# 2 cycles, 2 bytes
				; (do not clear it for save character path, which do not read pins)
				NOP5	;## 5 cycles
				LOOP	;## 9 cycles	; (5)
				NOP7	;## 7 cycles
				LOOP	;## 9 cycles	; (6)
				NOP7	;## 7 cycles
				LOOP	;## 9 cycles	; (7)
				NOP2	;## 2 cycles
outro_L7_2_lbl_\@:
				NOP2	;## 2 cycles
				NOP		;# 1 cycles, 1 bytes
	sts Data_register, r18		;		;# 2 cycles, 4 bytes
				LOOP	;## 9 cycles	; (8)
	sts Data_register+1, r19	;		;# 2 cycles, 4 bytes
	sts Counter, r21		;		;# 2 cycles, 4 bytes
	sts Status, r25			;		;# 2 cycles, 4 bytes
				NOP		;# 1 cycles, 1 bytes
				LOOP	;## 9 cycles
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; }}}
.endm
#define TRACE_STOP
TRACE_STOP
; }}}
; {{{
		; PS2_cont to manage it if CLOCK changes outside LOOPs
; }}}
.global PS2_cont
PS2_cont:
ECHO3
		; this function does the same as the main macro, but without waiting and LOOPs
		; it is intended to be called outside the RCAout function to continue the PS/2 work
; {{{
intro_lbl_:				; Load all our data				;
	lds r18, Data_register								;		;# 2 cycles, 4 bytes
	lds r19, Data_register+1							;		;# 2 cycles, 4 bytes
	lds r21, Counter								;		;# 2 cycles, 4 bytes
	lds r25, Status		; = 8 cycles so far					;		;# 2 cycles, 4 bytes

;switch:
	cpi r25,0		; are we waiting for clock fall?			;		;# 1 cycles, 2 bytes
	breq wait_fall_lbl_		;						;		;# 1-2 cycles, 2 bytes
	cpi r25,1		; or for clock go up ?					;		;# 1 cycles, 2 bytes
	breq wait_top_lbl_		;						;		;# 1-2 cycles, 2 bytes
	; else it is store_it_lbl_	;
	; L(2)+2
store_it_lbl_:			; Status = 2
				; trash r25 and r21 temporaly, as we know, what to reset them after this
				; we have 11 bits, need to save it
	lsl r19			; Shift out the 8. bit					;		;# 1 cycles, 2 bytes
	rol r18			; Roll it in and shift out the first bit (Start bit)	;		;# 1 cycles, 2 bytes
	lsl r19			;							;		;# 1 cycles, 2 bytes
	rol r18			;							;		;# 1 cycles, 2 bytes
; shift back stop + parity bits
				; r18 = data
	rjmp store_it_2_lbl_		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
wait_fall_lbl_:	; L(1)+7
	sbic CLOCK		;		;# 1-3 cycles, 2 bytes
	rjmp test_inside_lbl_		;		;# 2 cycles, 2 bytes
	; else fall to grab_lbl_

grab_lbl_:		; L(2)+2
	lsr r18			;		;# 1 cycles, 2 bytes
	ror r19			;		;# 1 cycles, 2 bytes
	sbic DATA		;		;## 1-2 cycles, 2 bytes
	ori r18, 0x80		;		;# 1 cycles, 2 bytes
	ldi r25, 1		;		;# 1 cycles, 2 bytes
	dec r21			;		;# 1 cycles, 2 bytes
	brne skip_over_1_		;		;# 1-2 cycles, 2 bytes
	ldi r25,2		;st1->2	;		;# 1 cycles, 2 bytes
skip_over_1_:
	rjmp outro_L3_6_lbl_		;		;# 2 cycles, 2 bytes

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
wait_top_lbl_:	; L(2)+3
	sbis CLOCK			;		;# 1-3 cycles, 2 bytes
	rjmp outro_L2_6_lbl_		;		;# 2 cycles, 2 bytes
		; L(2)+6
	ldi r25, 0			;		;# 1 cycles, 2 bytes
	rjmp outro_L3_2_lbl_		;		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
store_it_2_lbl_:
	lds r21, BIOS_buffer_head_ptr	; Load head pointer (buffer write index)                ;		;# 2 cycles, 4 bytes
	ldi ZL, lo8(BIOS_buffer)	; Load buffer base address (low byte)                  |;		;# 1 cycles, 2 bytes
	ldi ZH, hi8(BIOS_buffer)	; Load buffer base address (high byte)                 |;		;# 1 cycles, 2 bytes
	add ZL, r21		; Compute address: buffer + head                       |;		;# 1 cycles, 2 bytes
	adc ZH, r1		; Carry addition if needed                             |;		;# 1 cycles, 2 bytes
	st Z, r18		; store next char to buffer                            |;		;# 2 cycles, 2 bytes
	inc r21			; Increment head pointer                                ;		;# 1 cycles, 2 bytes
	andi r21, BUFFER_MASK	; Apply BUFFER_MASK (64-1 = 0x3F) for circular behavior ;		;# 1 cycles, 2 bytes
	lds r24, BIOS_buffer_tail_ptr	; Load tail pointer (buffer read index)                |;		;# 2 cycles, 4 bytes
	cp r24, r21		; Compare head and tail                                |;		;# 1 cycles, 2 bytes
	breq put_char_ret_1_lbl_	; If equal, buffer is full, return                     |;		;# 1-2 cycles, 2 bytes
	sts BIOS_buffer_head_ptr, r21	; Store updated head pointer                            ;		;# 2 cycles, 4 bytes
	rjmp put_char_ret_2_lbl_		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
test_inside_lbl_:	; L(2)+3
	sbic INSIDE			;		;# 1-3 cycles, 2 bytes
	rjmp outro_L2_6_lbl_		;		;# 2 cycles, 2 bytes
	ldi r21, 11			;		;# 1 cycles, 2 bytes
	ldi r18, 0			;		;# 1 cycles, 2 bytes
	ldi r19, 0			;		;# 1 cycles, 2 bytes
	rjmp outro_L3_4_lbl_		;		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
put_char_ret_1_lbl_:
put_char_ret_2_lbl_:
	ldi r25, 1		;# 1 cycles, 2 bytes
	ldi r21, 11		;# 1 cycles, 2 bytes
	ldi r18, 0		;# 1 cycles, 2 bytes
	ldi r19, 0		;# 1 cycles, 2 bytes
	rjmp outro_L7_2_lbl_		;# 2 cycles, 2 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
outro_L2_6_lbl_:
outro_L3_2_lbl_:
outro_L3_4_lbl_:
outro_L3_6_lbl_:
	sbi PCIFR,PCIF0		; clear pending PinChange  events		;		;# 2 cycles, 2 bytes
outro_L7_2_lbl_:
	sts Data_register, r18		;		;# 2 cycles, 4 bytes
	sts Data_register+1, r19	;		;# 2 cycles, 4 bytes
	sts Counter, r21		;		;# 2 cycles, 4 bytes
	sts Status, r25			;		;# 2 cycles, 4 bytes
; }}}
ECHO3
	ret		;# 4 cycles, 2 bytes


.global RCAout ; (*pScreenRam, *fontSlice, tcnt, minTCNT)
;                       R25  R24     R23 R22  R21 R20 R19 R18
; uses R18-R27
; interleaved PS/2 uses R10-...   (from LOOP	;## 9 cycles R18-R19, R21, R25)
RCAout:
	UNSUPPRESS
nop
	; interrupt may jitter because of instructions taking 1, 2, 3 clock cycles
	; (and maybe more if there are other interrupts)
	; this hack synchronizes the CPU with the tcnt value
	; only checking the low byte here because interrupt should always occur earlier than 240 counts (15 us)
	inc R18		;# 1 cycles, 2 bytes
	CP R20, R18 ; compare tcnt with minTCNT+1		;# 1 cycles, 2 bytes
	BRLO jitterFix1 ; extra cycle if tcnt was lower		;# 1-2 cycles, 2 bytes
jitterFix1:
	inc R18		;# 1 cycles, 2 bytes
	CP R20, R18 ; compare tcnt with minTCNT+2		;# 1 cycles, 2 bytes
	BRLO jitterFix2		;# 1-2 cycles, 2 bytes
jitterFix2:
	INC R18		;# 1 cycles, 2 bytes
	CP R20, R18 ; compare tcnt with minTCNT+3		;# 1 cycles, 2 bytes
	BRLO jitterFix3		;# 1-2 cycles, 2 bytes
jitterFix3:
	INC R18		;# 1 cycles, 2 bytes
	CP R20, R18 ; compare tcnt with minTCNT+4		;# 1 cycles, 2 bytes
	BRLO jitterFix4		;# 1-2 cycles, 2 bytes
jitterFix4:
	MOVW R26, R24 ; X = pScreenRam		;# 1 cycles, 2 bytes
	; when USART is awoken, it insists on clocking out at least one high pixel, no matter what we do.
	; so to suppress that, we use an IO pin, active low, to ground the video signal after the resistor.
	; once the USART has got going, we switch the IO pin back to input mode so it stops grounding the video signal.
	; interestingly, when the USART is put to sleep at the end of the row, then its output goes low.
	; this is true even when the last pixel sent was high.
	SUPPRESS
	NOP		; a	;# 1 cycles, 1 bytes
	NOP		; b	;# 1 cycles, 1 bytes
	NOP		; c	;# 1 cycles, 1 bytes
	NOP		; d	;# 1 cycles, 1 bytes
	NOP		; e	;# 1 cycles, 1 bytes
	NOP		; f	;# 1 cycles, 1 bytes
	NOP		; g	;# 1 cycles, 1 bytes
	LOOP	;## 9 cycles ; (0)
	LDI R24, 0x08		;# 1 cycles, 2 bytes
	STS UCSR0B, R24 ; enable USART transmit		;# 2 cycles, 4 bytes
	UNSUPPRESS
	NOP		;# 1 cycles, 1 bytes
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_4_L9_0
;			NOP
;			NOP
;			NOP
;			NOP
;			LOOP	;## 9 cycles ; (1)
;			LOOP7	;## 16 cycles ; (2)
;			LOOP7	;## 16 cycles ; (3)
;			LOOP7	;## 16 cycles ; (4)
;			LOOP7	;## 16 cycles ; (5)
;			LOOP7	;## 16 cycles ; (6)
;			LOOP7	;## 16 cycles ; (7)
;			LOOP7	;## 16 cycles ; (8)
;			LOOP7	;## 16 cycles ; (9)
			LOOP7	;## 16 cycles ; (10)
			NOP3	;## 3 cycles
_4_L9_0
;			LOOP	;## 9 cycles ; (11)
;			LOOP7	;## 16 cycles ; (12)
;			LOOP7	;## 16 cycles ; (13)
;			LOOP7	;## 16 cycles ; (14)
;			LOOP7	;## 16 cycles ; (15)
;			LOOP7	;## 16 cycles ; (16)
;			LOOP7	;## 16 cycles ; (17)
;			LOOP7	;## 16 cycles ; (18)
;			LOOP7	;## 16 cycles ; (19)
			LOOP7	;## 16 cycles ; (20)
			NOP3	;## 3 cycles
_4_L9_0
;			LOOP7	;## 16 cycles ; (21)
;			LOOP7	;## 16 cycles ; (22)
;			LOOP7	;## 16 cycles ; (23)
;			LOOP7	;## 16 cycles ; (24)
;			LOOP7	;## 16 cycles ; (25)
;			LOOP7	;## 16 cycles ; (26)
;			LOOP7	;## 16 cycles ; (27)
;			LOOP7	;## 16 cycles ; (28)
;			LOOP7	;## 16 cycles ; (29)
			LOOP7	;## 16 cycles ; (30)
			NOP3	;## 3 cycles
;_4_L9_0
			LOOP7	;## 16 cycles ; (31)
			LOOP7	;## 16 cycles ; (32)
			LOOP7	;## 16 cycles ; (33)
			LOOP7	;## 16 cycles ; (34)
			LOOP7	;## 16 cycles ; (35)
			LOOP7	;## 16 cycles ; (36)
;			LOOP7	;## 16 cycles ; (37)
;			LOOP7	;## 16 cycles ; (38)
;			LOOP7	;## 16 cycles ; (39)
;			LOOP7	;## 16 cycles ; (40)

;			LOOP7	;## 16 cycles ; (41)
;			LOOP7	;## 16 cycles ; (42)
;			LOOP7	;## 16 cycles ; (43)
;			LOOP7	;## 16 cycles ; (44)
;			LOOP7	;## 16 cycles ; (45)
			NOP3	;## 3 cycles
;
			STS UCSR0B, R1 ; disable USART transmit		;# 2 cycles, 4 bytes
NOP7	;## 7 cycles
SUPPRESS
			RET		;# 4 cycles, 2 bytes
