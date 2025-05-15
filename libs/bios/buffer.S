#define __SFR_OFFSET 0

#include "avr/io.h"
#define BUFFER_MASK 0x0F
	.section .bss
	.global BIOS_buffer
	.global BIOS_buffer_tail_ptr
	.global BIOS_buffer_head_ptr

BIOS_buffer:          .skip 16	; Allocate 16 bytes for the circular BIOS_buffer
BIOS_buffer_tail_ptr: .byte 0	; Allocate 1 byte for the tail pointer
BIOS_buffer_head_ptr: .byte 0	; Allocate 1 byte for the head pointer

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;#define SREG 0x3f
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.global BIOS_buffer_init	; void BIOS_buffer_init();
	.section .text
BIOS_buffer_init:
	ldi r18, 0			; init value
	sts BIOS_buffer_tail_ptr,r18	; zero tail
	sts BIOS_buffer_head_ptr,r18	; zero head
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.global BIOS_buffer_put_char	; void BIOS_buffer_put_char();
	.section .text
BIOS_buffer_put_char:
;sei                                                                                            ;
;jmp 0                                                                                          ;
	in r23,SREG                                                                             ;	;# 1 cycles, 2 bytes
	cli				; Disable interrupts                                    ;	;# 1 cycles, 2 bytes
                                                                                                ;
	mov r18,r24                                                                             ;	;# 1 cycles, 2 bytes
	lds r24, BIOS_buffer_tail_ptr	; Load tail pointer (BIOS_buffer read index)            ;	;# 2 cycles, 4 bytes
	lds r25, BIOS_buffer_head_ptr	; Load head pointer (BIOS_buffer write index)           ;	;# 2 cycles, 4 bytes
                                                                                                ;
	ldi ZL, lo8(BIOS_buffer)	; Load BIOS_buffer base address (low byte)              ;	;# 1 cycles, 2 bytes
	ldi ZH, hi8(BIOS_buffer)	; Load BIOS_buffer base address (high byte)             ;	;# 1 cycles, 2 bytes
	add ZL, r25			; Compute address: BIOS_buffer + head                   ;	;# 1 cycles, 2 bytes
	adc ZH, r1			; Carry addition if needed                              ;	;# 1 cycles, 2 bytes
                                                                                                ;
	st Z, r18			; store next char to BIOS_buffer                        ;	;# 2 cycles, 2 bytes
                                                                                                ;
	inc r25				; Increment head pointer                                ;	;# 1 cycles, 2 bytes
	andi r25, BUFFER_MASK		; Apply BUFFER_MASK (16-1 = 0x0F) for circular behavior ;	;# 1 cycles, 2 bytes
	cp r24, r25			; Compare tail and head                                 ;	;# 1 cycles, 2 bytes
	breq put_char_ret		; If equal, BIOS_buffer is full, return                 ;	;# 1-2 cycles, 2 bytes
	sts BIOS_buffer_head_ptr, r25	; Store updated head pointer                            ;	;# 2 cycles, 4 bytes
	                                                                                        ;
put_char_ret:                                                                                   ;
	out SREG,23			; Re-enable interrupts                                  ;	;# 1 cycles, 2 bytes
	ret                                                                                     ;	;# 4 cycles, 2 bytes
                                                                                                ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.global BIOS_buffer_get_char
	.section .text

BIOS_buffer_get_char:
	in r23,SREG
	lds r18, BIOS_buffer_tail_ptr	; Load tail pointer (BIOS_buffer read index)
	lds r25, BIOS_buffer_head_ptr	; Load head pointer (BIOS_buffer write index)

	cp r18, r25			; Compare tail and head
	breq buffer_empty		; If equal, BIOS_buffer is empty, return 0
	
	ldi ZL, lo8(BIOS_buffer)	; Load BIOS_buffer base address (low byte)
	ldi ZH, hi8(BIOS_buffer)	; Load BIOS_buffer base address (high byte)
	
;	cli				; Disable interrupts - not needed - will matter only if call from interrupt (and there is none) could colide with normal call
	
	add ZL, r18			; Compute address: BIOS_buffer + tail
	adc ZH, r1			; Carry addition if needed

	ld r24, Z			; Load next char from BIOS_buffer
	inc r18				; Increment tail pointer
	andi r18, BUFFER_MASK		; Apply BUFFER_MASK (64-1 = 0x3F) for circular behavior
	sts BIOS_buffer_tail_ptr, r18	; Store updated tail pointer
	jmp get_char_ret
buffer_empty:
	ldi r24, 0			; Return 0 if BIOS_buffer is empty
get_char_ret:
;	out SREG,23			; Re-enable interrupts
	ret				; Return 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.global ps2_read
ps2_read:
	; A0 (Clock) -> PINC0
	; A1 (Data)  -> PINC1
	; D13 (INSIDE) -> PINB5

	push r18
	push r24
	push r25

;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;#define PORTB

	#define A0 PINC, 0
	#define A1 PINC, 1
	#define D13 PINB, 5
	
	ldi r18, 11			; Counter for 11 bits
	clr r24				; Low byte of BIOS_buffer (holds bits 0-7) (RESULT)
	clr r25				; High byte of BIOS_buffer (holds bits 8-10)

wait_clock_low:
	sbis A0 			; Skip if Clock (D10) is low
	rjmp wait_clock_low		; Wait until clock goes low

read_bit:
	lsr r24				; Shift left (MSB into carry)
	ror r25				; Rotate carry into high byte
	sbic A1 			; Skip if Data (D11) is low
	ori r24, 0x80			; Set lowest bit

wait_clock_high:
	sbic A0 			; Skip if Clock (D10) is high
	rjmp wait_clock_high		; Wait until clock goes high

	dec r18				; Decrement bit counter
	brne wait_clock_low		; If not done, repeat

	lsl r25				; Shift out the first bit (Start bit)
	rol r24

	call BIOS_buffer_put_char	; r24 -> BIOS_buffer

	pop r25
	pop r24
	pop r18
	ret				; Return nothing


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;	Arduino Uno + PS/2 klávesnice 
;	Clock D10
;	Data D11
;	
;	Assembler implementace 64 byte ring bufferu
;	* void BIOS_buffer_put_char(byte); 
;	* char BIOS_buffer_get_char(); 
;	* BIOS_buffer_init();
;	
;	* ps2_read() - čeká na první znak na PS/2, vloží ho do bufferu a skončí - demo jak to číst, bude chtít přepracovat
;	
;	Po startu vypisuje na Serial trojice scankódů (obvykle stisk klávesy a její puštění (F0+kód))
