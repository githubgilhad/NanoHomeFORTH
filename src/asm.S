/* vim: set ft=asm noexpandtab fileencoding=utf-8 nomodified wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: tabstop=8 linebreak showbreak=»\   */
// ,,g = gcc, exactly one space after "set"
#if defined(__PC__)
	.section .note.GNU-stack,"",@progbits
	.intel_syntax noprefix
#endif

#include "flags.h"
	.section .text

	.global here
here:
	.ascii "<###"
	.global RETX_0
RETX_0:
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
	ret
#elif defined(__PC__)
	mov eax, [esp + 4]	// načti argument (ukazatel na uint32_t)
	ret
#else
	#error "unsupported"
#endif
	.global B3PTR
	.type B3PTR, @function
	.global B3U32
	.type B3U32, @function
	.global B3U32noRAM
	.type B3U32noRAM, @function
B3PTR:
B3U32:
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
	clr	r25
	ret
#elif defined(__PC__)
	mov eax, [esp + 4]	// načti argument (ukazatel na uint32_t)
	ret
#else
	#error "unsupported"
#endif
B3U32noRAM:
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
	cbr	r24,0x80
	clr	r25
	ret
#elif defined(__PC__)
	mov eax, [esp + 4]	// načti argument (ukazatel na uint32_t)
	ret
#else
	#error "unsupported"
#endif
	// extern uint8_t B1at(ptr24_t p);				; {{{
	//	takes pointer somewhere	(24bit/32bit value)
	//	read 1 byte (as __xload_1 does) and return it as uint8_t
	//
	.global B1at
	.type B1at, @function

B1at:
#if defined(__AVR_ATmega328P__)
	// Adresa (__memx) je v r24:r22 (r22 = LSB)
	// Vyčteme tři bajty z adresy r22:r24
	movw r30, r22        //# ZL = r22, ZH = r23
	lpm	r25, Z
	sbrc	r24,7
	ld	r25, Z
	mov	r24,r25
	ret
#elif defined(__AVR_ATmega2560__)
	// Adresa (__memx) je v r24:r22 (r22 = LSB)
	// Vyčteme tři bajty z adresy r22:r24
	movw r30, r22        //# ZL = r22, ZH = r23
	sbrc	r24,7
	rjmp	1f
	in	r25,0x3b
	out	0x3b,r24
	elpm	r24, Z
	out	0x3b,r25
	ret
1:
	ld	r24, Z
	ret
#elif defined(__PC__)
	mov eax, [esp + 4]	// načti argument (ukazatel na uint32_t)
	mov eax, [eax]
	and eax, 0xFF
	ret
#else
	#error "B1at  - supported only for ATmega328P and ATmega2560"
#endif
	// }}}
	// extern B2 B2at(ptr24_t p);				; {{{
	//	takes pointer somewhere	(24bit/32bit value)
	//	read 2 bytes (as __xload_2 does) and return it as uint32_t (4.byte cleared)
	//
	.global B2at		//# ptr24_t p=&w_someword_cw; uint32_t cw= B3at(p);
	.type B2at, @function

B2at:
#if defined(__AVR_ATmega328P__)
	// Adresa (__memx) je v r24:r22 (r22 = LSB)
	// Vyčteme dva bajty z adresy r22:r24
	movw r30, r22        // ZL = r22, ZH = r23
	sbrc	r24,7
	rjmp	1f
	lpm	r24, Z+
	lpm	r25, Z+
	ret
1:
	ld	r24, Z+
	ld	r25, Z+
	ret
#elif defined(__AVR_ATmega2560__)
	// Adresa (__memx) je v r24:r22 (r22 = LSB)
	// Vyčteme dva bajty z adresy r22:r24
	movw r30, r22        //# ZL = r22, ZH = r23
	sbrc	r24,7
	rjmp	1f
	in	r23,0x3b
	out	0x3b,r24
	elpm	r24, Z+
	elpm	r25, Z+
	out	0x3b,r23
	ret
1:
	ld	r24, Z+
	ld	r25, Z+
	ret
#elif defined(__PC__)
	mov eax, [esp + 4]	// načti argument (ukazatel na uint32_t)
	mov eax, [eax]
	and eax, 0xFFFF
	ret
#else
	#error "B2at  - supported only for ATmega328P and ATmega2560"
#endif
	// }}}
	// extern B3 B3at(ptr24_t p);				; {{{
	//	takes pointer somewhere	(24bit/32bit value)
	//	read 3 bytes (as __xload_3 does) and return it as uint32_t (4.byte cleared)
	//
	.global B3at		// ptr24_t p=&w_someword_cw; uint32_t cw= B3at(p);
	.type B3at, @function

B3at:
#if defined(__AVR_ATmega328P__)
	// Adresa (__memx) je v r24:r22 (r22 = LSB)
	// Vyčteme tři bajty z adresy r22:r24
	movw r30, r22        // ZL = r22, ZH = r23
	sbrc	r24,7
	rjmp	1f
	lpm	r22, Z+
	lpm	r23, Z+
	lpm	r24, Z+
	clr	r25
	ret
1:
	ld	r22, Z+
	ld	r23, Z+
	ld	r24, Z+
	clr	r25
	ret
#elif defined(__AVR_ATmega2560__)
	// Adresa (__memx) je v r24:r22 (r22 = LSB)
	// Vyčteme tři bajty z adresy r22:r24
	movw r30, r22        // ZL = r22, ZH = r23
	sbrc	r24,7
	rjmp	1f
	in	r25,0x3b
	out	0x3b,r24
	elpm	r22, Z+
	elpm	r23, Z+
	elpm	r24, Z+
	out	0x3b,r25
	clr	r25
	ret
1:
	ld	r22, Z+
	ld	r23, Z+
	ld	r24, Z+
	clr	r25
	ret
#elif defined(__PC__)
	mov eax, [esp + 4]	// načti argument (ukazatel na uint32_t)
	mov eax, [eax]
	ret
#else
	#error "B3at  - supported only for ATmega328P and ATmega2560"
#endif
	// }}}
	// extern B4 B4at(ptr24_t p);				; {{{
	//	takes pointer somewhere	(24bit/32bit value)
	//	read 4 bytes (as __xload_3 does) and return it as uint32_t
	//
	.global B4at		// ptr24_t p=&w_someword_cw; uint32_t cw= B4at(p);
	.type B4at, @function

B4at:
#if defined(__AVR_ATmega328P__)
	// Adresa (__memx) je v r24:r22 (r22 = LSB)
	// Vyčteme čtyři bajty z adresy r22:r24
	movw r30, r22        // ZL = r22, ZH = r23
	sbrc	r24,7
	rjmp	1f
	lpm	r22, Z+
	lpm	r23, Z+
	lpm	r24, Z+
	lpm	r25, Z+
	ret
1:
	ld	r22, Z+
	ld	r23, Z+
	ld	r24, Z+
	ld	r25, Z+
	ret
#elif defined(__AVR_ATmega2560__)
	// Adresa (__memx) je v r24:r22 (r22 = LSB)
	// Vyčteme tři bajty z adresy r22:r24
	movw r30, r22        // ZL = r22, ZH = r23
	sbrc	r24,7
	rjmp	1f
	in	r21,0x3b
	out	0x3b,r24
	elpm	r22, Z+
	elpm	r23, Z+
	elpm	r24, Z+
	elpm	r25, Z+
	out	0x3b,r21
	ret
1:
	ld	r22, Z+
	ld	r23, Z+
	ld	r24, Z+
	ld	r25, Z+
	ret
#elif defined(__PC__)
	mov eax, [esp + 4]	// načti argument (ukazatel na uint32_t)
	mov eax, [eax]
	ret
#else
	#error "B4at  - supported only for ATmega328P and ATmega2560"
#endif
	// }}}
	// extern void jmp_indirect_24(ptr24_t p);				; {{{
	//	takes pointer to codeword somewhere	(24bit/32bit value)
	//	read the codeword
	//	divide by two (byte address -> word address)
	//	jumps there
	//
	.global jmp_indirect_24		// ptr24_t p=&w_someword_cw; jmp_indirect_24(p);
	.type jmp_indirect_24, @function

jmp_indirect_24:
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
	call B3at
	// Dělení 2 (>>1) pro přechod z byte addr na word addr
	lsr r24
	ror r23
	ror r22
#endif
#if defined(__AVR_ATmega328P__)
	// Pushujeme cílovou adresu na zásobník (2 bajty PC pro 328P)
	push r22
	push r23
	ret			// Skok na cílovou adresu
#elif defined(__AVR_ATmega2560__)
	// Pushujeme cílovou adresu na zásobník (3 bajty PC pro 2560)
	push r22
	push r23
	push r24
	ret			// Skok na cílovou adresu
#elif defined(__PC__)
	
	// eax = [esp + 4] = argument ptr
	mov eax, [esp + 4]	// načti argument (ukazatel na uint32_t)
	jmp [eax]
/*
	mov eax, [eax]		// načti hodnotu z toho místa = cílová adresa
	
	add esp, 8		// "ukliď" návratovou adresu ze zásobníku a argument
				// (funkce nikdy nevrací, tak ručně posuň ESP)
	jmp eax			// skok na novou adresu
*/

#else
	#error "jmp_indirect_24  - supported only for ATmega328P and ATmega2560"
#endif
	// }}}
.macro DEFWORD lbl, attr, name, codeword	// {{{
	.global \lbl
	.global \lbl\()_head
\lbl\():
\lbl\()_head:
	.long 1b-4
1:
	.byte  \attr	// attributes
	.byte len\@ 	// name len
	name_start_\@:
	.ascii "\name"				// name without \0
	.equ len\@,(. - name_start_\@)
	
	.global \lbl\()_cw
\lbl\()_cw:
	.extern \codeword
	.long \codeword				// 3B address of function in FLASH
	.global \lbl\()_data
\lbl\()_data:
	// more payload may be outside macro
.endm	// }}}
.macro DEFVAR name	// {{{
	DEFWORD var_\name,0,"\name",push_var_\name
.endm	// }}}
.macro DEFCONST name	// {{{
	DEFWORD const_\name,0,"\name",push_const_\name
.endm	// }}}
.macro DEFCONST1 name, value	// {{{
	DEFWORD const_\name,0,"\name",f_doconst
	.long \value
.endm	// }}}
.macro DEFCONST2 name, value	// {{{
	DEFWORD const_\name,0,"\name",f_doconst2
	.long \value
.endm	// }}}
	//  :s@DEFWORD [^,]*,[^,]*,[^,]*,\s*\([^ 	;#/]*\)[^;#/]*\(.*\)@void \1() {	// {{{ \2\r	INFO("\1");\r	\r	NEXT;\r}	// }}}
	// ;;; trick for STOP value for search: 0.header with none data
	.long 0		// prev header none
1:
	.byte FLG_HIDDEN
	.byte 0 //len 0 + FLG_HIDDEN = impossible to find
	// zero bytes of name here
	.long 0		// emty codeword

	// ;;; word definitions

.macro PORT name,value	// {{{
DEFCONST2 PIN\name, 0x800000+\value
DEFCONST2 DDR\name, 0x800001+\value
DEFCONST2 PORT\name, 0x800002+\value
.endm	// }}}
#if defined(__AVR_ATmega328P__)
PORT B, 0x23
PORT C, 0x26
PORT D, 0x29
#elif defined(__AVR_ATmega2560__)
PORT A, 0x20
PORT B, 0x23
PORT C, 0x26
PORT D, 0x29
PORT E, 0x2C
PORT F, 0x2F
PORT G, 0x32
PORT H, 0x100
PORT J, 0x103
PORT K, 0x106
PORT L, 0x109
#endif
DEFVAR HERE
DEFVAR LAST
DEFVAR STATE
DEFVAR BASE
DEFCONST2 DOCOL,f_docol
DEFCONST2 RAM,RAM
DEFWORD w_lit,	FLG_ARG,	"LIT",	f_lit
DEFWORD w_lit2,	FLG_ARG,	"LIT2",	f_lit2
DEFWORD w_Store,	0,	"!",	f_Store
DEFWORD w_StoreChar,	0,	"!C",	f_StoreChar
DEFWORD w_StoreDouble,	0,	"!D",	f_StoreDouble
DEFWORD w_At,   	0,	"@",	f_At
DEFWORD w_CharAt,	0,	"C@",	f_CharAt
DEFWORD w_DoubleAt,	0,	"D@",	f_DoubleAt
DEFWORD w_dup,  	0,	"DUP",	f_dup
DEFWORD w_drop, 	0,	"DROP",	f_drop
DEFWORD w_swap, 	0,	"SWAP",	f_swap
DEFWORD w_or,   	0,	"OR",	f_or
DEFWORD w_and,  	0,	"AND",	f_and
DEFWORD w_plus, 	0,	"+",	f_plus
DEFWORD w_minus,	0,	"-",	f_minus
DEFWORD w_times,	0,	"*",	f_times
DEFWORD w_div,  	0,	"/",	f_div
DEFWORD w_div2, 	0,	"/2",	f_div2
DEFWORD w_div4, 	0,	"/4",	f_div4
DEFWORD w_dup_D,	0,	"DUP2",	f_dup_D
DEFWORD w_drop_D,	0,	"DROP2",	f_drop_D
DEFWORD w_swap_D,	0,	"SWAP2",	f_swap_D
DEFWORD w_plus_D,	0,	"+D",	f_plus_D
DEFWORD w_minus_D,	0,	"-D",	f_minus_D
DEFWORD w_times_D,	0,	"*D",	f_times_D
DEFWORD w_div_D,	0,	"/D",	f_div_D
DEFWORD w_div2_D,	0,	"/2D",	f_div2_D
DEFWORD w_div4_D,	0,	"/4D",	f_div4_D
DEFWORD w_c2C,  	0,	"c2C",	f_c2C	// cell -> 2 C
DEFWORD w_D4C,  	0,	"D4C",	f_D4C	// Double -> 4C
DEFWORD w_2Cc,  	0,	"2Cc",	f_2Cc	// 2 C -> cell
DEFWORD w_4CD,  	0,	"4CD",	f_4CD	// 4 C -> Double
DEFWORD w_1minus,	0,	"1-",	f_1minus	// decrement TOS by 1
DEFWORD w_4minus,	0,	"4-",	f_4minus	// decrement TOS by 4
DEFWORD w_exit, 	0,	"EXIT",	f_exit
DEFWORD w_return, 	0,	"RETURN",	f_exit	// synonymum EXIT, not detected by show (for exit in middle od WORD)
DEFWORD w_key,  	0,	"KEY",	f_key
DEFWORD w_emit, 	0,	"EMIT",	f_emit
DEFWORD w_word, 	0,	"WORD",	f_word
DEFWORD w_comma,	0,	",",	f_comma
DEFWORD w_number,	0,	"NUMBER",	f_number
DEFWORD w_dot,  	0,	".",	f_dot
DEFWORD w_branch,	FLG_ARG,	"BRANCH",	f_branch
DEFWORD w_0branch,	FLG_ARG,	"0BRANCH",	f_0branch	// branch if zero
DEFWORD w_interpret,	0,	"INTERPRET",	f_interpret
DEFWORD w_words,	0,	"WORDS",	f_words
# -------------------------------------------------------------------------------- tests
DEFWORD w_double, 0, "DOUBLE", f_docol
	.long w_dup_cw
	.long w_plus_cw
	.long w_exit_cw
DEFWORD w_trouble, FLG_HIDDEN + FLG_IMMEDIATE, "TROUBLE", f_docol
	.long w_exit_cw
DEFWORD w_debug, 0, "DEBUG", f_debug	// primitivum, kde se opet zmocnim vlady
DEFWORD w_test, 0, "TEST", f_docol
	.long w_double_cw
	.long w_debug_cw
	.long w_exit_cw
DEFWORD w_dump, 	0,	"dump",	f_dump
DEFWORD w_nodebug,	0,	"nodebug",	f_nodebug
DEFWORD w_noinfo,	0,	"noinfo",	f_noinfo
DEFWORD w_notrace,	0,	"notrace",	f_notrace
DEFWORD w_hex,  	0,	"hex",	f_hex
DEFWORD w_dec,  	0,	"dec",	f_dec
DEFWORD w_bin,  	0,	"bin",	f_bin
DEFWORD w_cw2h, 	0,	"cw2h",	f_cw2h		// ( cw -- h ) convert codeword address to head address
DEFWORD w_show, 	0,	"show",	f_show		// ( cw -- ) show  try to show definition of WORD addressed by codeword
DEFCONST1 ff,	0xFF
DEFCONST1 aa,	0xAA
# -------------------------------------------------------------------------------- more primitives
DEFWORD w_create,	0,	"CREATE",	f_create		// create head from WORD result
DEFWORD w_left_brac,	FLG_IMMEDIATE,	"[",	f_left_brac		// [ goes to immediate mode
DEFWORD w_right_brac,	0,	"]",	f_right_brac			// ] goes to compile mode
DEFWORD w_find, 	0,	"FIND",	f_find				// WORD FIND return Addr_of_header (or 0 0 )
DEFWORD w_hidden,	0,	"HIDDEN",	f_hidden		// Addr_of_header HIDE hide/unhide the word
DEFWORD w_tick, 	0,	"'",	f_tick				// push CW_address of next word to stack (and skip it)
DEFWORD w_immediate,	0,	"IMMEDIATE",	f_immediate		// Addr_of_header IMMEDIATE make the word immediate
DEFWORD w_zero, 	0,	"==0",	f_zero				// true if zero
DEFWORD w_notzero,  	0,	"!=0",	f_notzero			// true if not zero
DEFWORD w_positive,	0,	">0",	f_positive			// true if positive
DEFWORD w_positive0,	0,	">=0",	f_positive0			// true if positive or zero
DEFWORD w_negative,	0,	"<0",	f_negative			// true if negative
DEFWORD w_negative0,	0,	"<=0",	f_negative0			// true if negative or zero
DEFWORD w_notequal, 	0,	"!=",	f_notequal			// (c1 c2 -- flag ) true if not equal
DEFWORD w_notequalD, 	0,	"!=D",	f_notequalD			// (d1 d2 -- flag ) true if not equal
DEFWORD w_equal, 	0,	"==",	f_equal				// (c1 c2 -- flag ) true if equal
DEFWORD w_equalD, 	0,	"==D",	f_equalD			// (d1 d2 -- flag ) true if equal
# -------------------------------------------------------------------------------- more FORTH words

#include "words.inc"


	.global val_of_w_exit_cw, val_of_f_docol
val_of_w_exit_cw:
	.long w_exit_cw
val_of_f_docol:
	.long f_docol
	.global top_head
#	.EQU	top_head,1b-3
	.EQU	top_head,1b-4

.balign 2
	.ascii "###>"
	.global end
end:
//.balign 65536, 'F'
