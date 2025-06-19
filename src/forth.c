/* vim: set ft=cpp showbreak=»\  noexpandtab fileencoding=utf-8 nomodified wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd list: lcs=tab\:|- tabstop=8 linebreak  */
// ,,g = gcc, exactly one space after "set"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#ifdef __PC__
#include "itoa.h"
#endif
#include "flags.h"
#include "ptr24.h"
#include "io.h"
#include "debug.h"
#ifdef __PC__
	#include "memdump.h"
	#include <stdio.h>
	#define PEDANT
	#include <unistd.h>
#endif
extern uint8_t B1at(uint32_t p);			// asm.S	read 1 byte at address p (somewhere), return 1 byte
extern uint16_t B2at(uint32_t p);			// asm.S	read 2 bytes at address p (somewhere), return 2 bytes
extern uint32_t B3at(uint32_t p);			// asm.S	read 3 bytes at address p (somewhere), return 4 bytes (top cleared)
extern uint32_t B4at(uint32_t p);			// asm.S	read 4 bytes at address p (somewhere), return 4 bytes
extern const __memx void * B3PTR(uint32_t p);		// asm.S	typecast, get 3 bytes, return 4 bytes (top cleared)
extern uint32_t B3U32(const __memx void * p);		// asm.S	typecast, get 3 bytes, return 4 bytes (top cleared)
extern void jmp_indirect_24(uint32_t p);		// asm.S	call function, which byte_address is at address p (somewhere) (converts bytes to words)

/*
 * Decision:
 * 	cell = uint16_t
 * 	pointer = uint32_t, but only 3B used, 4.B always zero
 */

_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 32 bits");
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
_Static_assert(sizeof(const __memx void *) == 3, "const __memx void * must be 24 bits");
#elif defined(__PC__)
_Static_assert(sizeof(const __memx void *) == 4, "const __memx void * must be 32 bits");
void do_traceback();	// forward
extern char a_read_char();
char read_char() {
	char c=a_read_char();
	if (c==0x7E) do_traceback();
	return c;
}

#else
#error undefined
#endif

void write_num8(uint8_t n) {	// {{{
	if (n>99) {
		write_char('0'+ n/100);
		n=n % 100;
		};
	if (n>9) {
		write_char('0'+ n/10);
		n=n % 10;
		};
		write_char('0'+n);
}	// }}}
char buf[32];	// temporary buffer - stack eating structures cannot be in NEXT-chained functions, or stack will overflow !!!

/** {{{ Intro
 * Lets start programming FORTH
 * My idea is
 * 	* Indirect Threading
 * 	* long names for words
 * 		* well, possibly more than just 3 (of nano Forth)
 * 		* strictly less then 32 (so Pascal Length can be found backward)
 * 		* only ASCII chars allowed (from space=0x20 upt to tilda=0x7F)
 * 		*
 * 	* some FORTH words will be in FLASH
 * 		* so 3+ bytes pointers are needed
 * 		* 3 bytes pointers are problematic and __memx is tricky, so let use uint32_t instead and few asm utils
 * the header is divided into two parts, first is for dictionary, followed second, which is for runing words
 * C discourse
 * 	* first we need 3bytes pointer to something
 * 		* it will be memory, but maybe RAM, maybe FLASH
 * 		* it will be
 * 			* byte in memory - uint8_t (just the memory itself after all)
 * 			* 3bytes pointer in memory (usually pointer to some other word)
 * 			* const char (for easy string manipulation)
 * 			* 3B pointer to function in FLASH (well it could be 2B on 328, but not on 2560, and we will use it together with other 3B pointers, so make ++/-- easy)
 * 			* anything else, which can be (type casted) at will
 * */ // }}}
/*
typedef const __memx void(*CodeWord_t)();	// CodeWord_t is 2B pointer to function (in FLASH) (*CodeWord_t)() calls the function.
typedef const __memx CodeWord_t (*Data_t);	// Data_t is 3B pointer to CodeWord_t "somewhere"
typedef const __memx Data_t (* InstrPoint_t);	// InstrPoint_t is 3B pointer to Data_t "somewhere"
typedef void const __memx *PTR_t;	// universal 3B pointer to any data "somewhere"
typedef uint16_t CELL_t;	// cell on data stack 2B

typedef const __memx char *xpC;	// 3B pointer 1B target	pointer "somewhere" to char "somewhere"
typedef const __memx uint8_t *xpB;	// 3B pointer 1B target	pointer "somewhere" to byte "somewhere"
typedef const __memx uint32_t *xpD;


typedef struct head1_t {	// {{{
	const __memx struct head1_t *next;		// 3B: pointer to next header "somewhere"
	uint8_t fill; // to 4B pointer
	uint8_t flags;		// 1B:
	uint8_t len;		// 1B: up to 31 (=5bits)
	const char name[];	// len B:name of WORD
} head1_t;	// }}}
typedef const __memx head1_t	*xpHead1;	// 3B pointer to head1 "somewhere"
*/
/*
 * typedef struct head2 {	// {{{
 * 	CodeWord_t codepoint;	// 3B: pointer to function to interpret data
 * 	Data_t data[];	// 3B: pointer to 2B pointer "somewhere" to function to interpret data - pointer to head2 "somewhere"
 * 	} head2;	// }}}
 */

typedef const __memx void(*CodeWord_t)();	// CodeWord_t is 2B pointer to function (in FLASH) (*CodeWord_t)() calls the function.
typedef const __memx CodeWord_t (*Data_t);	// Data_t is 3B pointer to CodeWord_t "somewhere"
typedef const __memx char *xpC;	// 3B pointer 1B target	pointer "somewhere" to char "somewhere"
typedef struct head1_t {	// {{{
	const __memx struct head1_t *next;		// 3B: pointer to next header "somewhere"
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
	uint8_t fill; // to 4B pointer
#endif
	uint8_t flags;		// 1B:
	uint8_t len;		// 1B: up to 31 (=5bits)
	const char name[];	// len B:name of WORD
} head1_t;	// }}}
typedef const __memx head1_t	*xpHead1;	// 3B pointer to head1 "somewhere"


typedef uint32_t PTR_t; 	// universal "3B pointer" to any data "somewhere" - use B1at, B3at for dereferencing
typedef uint16_t CELL_t;	// cell on data stack 2B
typedef int16_t S_CELL_t;	// signed cell on data stack 2B
typedef uint32_t DOUBLE_t;	// 2 cell on data stack 4B
typedef int32_t S_DOUBLE_t;	// signed 2 cell on data stack 4B
typedef uint8_t  BYTE_t;	// something for pointers to points to

PTR_t		IP;	// pointer to element of data[], which should be next
PTR_t		DT;	// NEXT is **(IP++)() - so DT=*IP as internal step. DT is value of last data pointed by IP before NEXT (= address of codepoint to exec) (used by f_docol to know, from where it was called) -  f_docol= { Rpush(IP);IP=DT + x; NEXT} x=sizeof(codeword)
/*
 * STACKS:
 *   for now I will use array and index, as it is easy to check range and only pop/push should be affected
 *   also lets start with small value, so it can be tested for both under- and over- flow
 *   also let push grow up and pop go down and 0 is empty stack (so push(x){stck[stack++]=x;}
 */
CELL_t		stck[STACK_LEN];
uint16_t	stack=0;
PTR_t		Rstck[RSTACK_LEN];
uint16_t	Rstack=0;

/*
 * RAM:
 * now let it be just small array too
 * and test it until all works
 */
uint8_t RAM[RAM_LEN];
uint32_t HERE;
//const __memx uint8_t *HERE;

/*
 * LAST
 * pointer to begin of last header
 */
PTR_t LAST;

extern const __memx BYTE_t		top_head;	// pointer to last header in asm module
extern const __memx BYTE_t		w_lit_cw;
extern const __memx BYTE_t		w_quit_cw;
extern const __memx BYTE_t		w_exit_cw;
extern const __memx BYTE_t		w_double;
extern const __memx char w_test_data;
extern const __memx char w_test_cw;
extern const __memx char w_quit_data;
extern const __memx DOUBLE_t		val_of_w_exit_cw;
extern const __memx DOUBLE_t		val_of_f_docol;
extern const __memx char w_lit2_cw;
extern const __memx char w_firtsbuildinword_head;
extern const __memx char w_lastbuildinword_head;
extern const __memx char w_lastbuildinword_end;
#define NEXT
//#define NEXT f_next()
//void f_next() __attribute__((noreturn));
void f_next(){	// {{{
	INFO("f_next");
	DT=B3at(IP);
DEBUG_DUMP(IP,"IP old	");
//	ERROR("Press ANY key to continue");
//	wait_for_char();
	IP+=4;		// IP++ but 4 bytes everytime
DEBUG_DUMP(IP,"IP new	");
DEBUG_DUMP(DT,"DT new	");
DEBUG_DUMP(B3at(DT),"*DT	");
	jmp_indirect_24(DT);
}	// }}}
void forth_loop(uint32_t cw_addr) {	// {{{
	INFO("Start of loop");
	uint32_t fake[2];
	fake[0]=cw_addr;
	fake[1]=B3U32(NULL);
	IP=B3U32(&fake[0]);
	while ( (IP!=B3U32(NULL)) && ( (DT=B4at(IP)) !=B3U32(NULL)) ) {
		DT=B4at(IP);
		DEBUG_DUMP(IP,"IP:	");
		DEBUG_DUMP(DT,"DT:	");
		IP+=4;
		jmp_indirect_24(DT);
//		((void (*)(void))B4at(DT))();
//		((void (*)(void))B3PTR(B4at(DT)))();
	};
	INFO("End of loop");
}	// }}}

// {{{ pop ...
CELL_t pop() {
	if (stack==0) {
		ERROR("pop - Stack underflow");
		#ifdef PEDANT
			do_traceback();
		#endif
		return 0;
		};
	if (! noinfo) write_hex16(stck[stack-1]);
	INFO("pop");
	return stck[--stack];
}
void push(CELL_t x) {
	if (! noinfo) write_hex16(x);
	INFO("push");
	if(stack>STACK_LEN-1) {
		ERROR("push - Stack owerlow");
		#ifdef PEDANT
			do_traceback();
		#endif
		return;
		};
	stck[stack++]=x;
}
CELL_t peek(){
	if (stack<1) {
		ERROR("peek - No Stack left");
		#ifdef PEDANT
			do_traceback();
		#endif
		return 0;
		};
	if (! noinfo) write_hex16(stck[stack-1]);
	INFO("peek");
	return stck[stack-1];
}
CELL_t peekX(uint8_t depth){
	if (stack<1+depth) {
		ERROR("peek - No Stack left");
		#ifdef PEDANT
			do_traceback();
		#endif
		return 0;
		};
	if (! noinfo) write_hex16(stck[stack-1-depth]);
	INFO("peek");
	return stck[stack-1-depth];
}
// }}}
// {{{ pop2 ...
DOUBLE_t pop2() {
	if (stack<2) {
		ERROR("pop2 - Stack underflow");
		#ifdef PEDANT
			do_traceback();
		#endif
		return 0;
		};
	if (! noinfo) write_hex32((stck[stack-2]*(1L<<16))+stck[stack-1]);
	INFO("pop2");
	DOUBLE_t r=stck[stack-2]*(1L<<16)+stck[stack-1];
	stack-=2;
	return r;
}
void push2(DOUBLE_t x) {
	if(stack>STACK_LEN-2) {
		ERROR("push2 - Stack owerlow");
		#ifdef PEDANT
			do_traceback();
		#endif
		return;
		};
	if (! noinfo) write_hex32(x);
	INFO("push2");
	stck[stack++]=x>>16;
	stck[stack++]=x&0xFFFF;
}
DOUBLE_t peek2(){
	if (stack<2) {
		ERROR("peek2 - No Stack left");
		#ifdef PEDANT
			do_traceback();
		#endif
		return 0;
		};
	if (! noinfo) write_hex32(stck[stack-2]*(1L<<16)+stck[stack-1]);
	INFO("peek2");
	return stck[stack-2]*(1L<<16)+stck[stack-1];
}
DOUBLE_t peek2X(uint8_t depth){
	if (stack<2+depth) {
		ERROR("peek2 - No Stack left");
		#ifdef PEDANT
			do_traceback();
		#endif
		return 0;
		};
	if (! noinfo) write_hex32(stck[stack-2-depth]*(1L<<16)+stck[stack-1-depth]);
	INFO("peek2");
	return stck[stack-2-depth]*(1L<<16)+stck[stack-1-depth];
}
// }}}
// {{{ Rpop ...
PTR_t Rpop() {
	if (Rstack==0) {
		ERROR("Rpop - Stack underflow");
		#ifdef PEDANT
			do_traceback();
		#endif
		return 0;
		};
	if (! noinfo) write_hex32(Rstck[Rstack-1]);
	INFO("Rpop");
	return Rstck[--Rstack];
}
void Rpush(PTR_t x) {
	if (! noinfo) write_hex32(x);
	INFO("Rpush");
	if(Rstack>RSTACK_LEN-1) {
		ERROR("Rpush - Stack owerlow");
		#ifdef PEDANT
			do_traceback();
		#endif
		return;
		};
	Rstck[Rstack++]=x;
}
PTR_t Rpeek(){
	if (Rstack==0) {
		ERROR("Rpeek - No Stack left");
		#ifdef PEDANT
			do_traceback();
		#endif
		return 0;
		};
	if (! noinfo) write_hex32(Rstck[Rstack-1]);
	INFO("Rpeek");
	return Rstck[Rstack-1];
}
// }}}
// {{{ some internal functions
uint8_t word_buf_len=0;
char word_buf[32];
void get_word(){	 // {{{ WAITS for word and puts it into word_buf_len + word_buf
	uint8_t i=0;
	char c=' ';
	while (true){
		while (strchr(" \t\r\n",c)) c=wait_for_char();			// skip spaces
		if (c=='\\') {
			while (!strchr("\r\n",c)) c=wait_for_char();
			continue;
			};	// skip \ comments to the end of line
		break;};	// finally word begins
	while ((!strchr(" \t\r\n",c)) && (i<sizeof(word_buf)-1)) {word_buf[i++]=c;c=wait_for_char();};
	word_buf[i]=0;
	word_buf_len=i;
}	// }}}
xpHead1 findHead(uint8_t len,const char *wordname, xpHead1 h) { 	// {{{
	if (len==0) return NULL;
	while (h) {	// internally it ends on .long 0
		if (h->flags & FLG_HIDDEN) { h=h->next;continue;};
		if (h->len != len) { h=h->next;continue;};
		const char *c=wordname;
		xpC hc=&(h->name[0]);
		int16_t l=len;
		while (l--) if(*c++!=*hc++) { break;};
		if (l==-1) return h;
		h=h->next;
	}
	return NULL;
}	// }}}
uint32_t get_codeword_addr(xpHead1 h){	 // {{{ // Data_t
	if (!(B3U32(h) & ~0x800000L)) return 0;
	if (!h) return 0;
	xpC c=&h->name[h->len];
	return B3U32(c);
}	// }}}
// }}}

void f_docol(); 	// FORWARD
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define VARfn(name)	void push_var_##name(){TRACE(#name);push(0x80);push((CELL_t)((uint16_t)(&name)));NEXT;}
#elif defined(__PC__)
#define VARfn(name)	void push_var_##name(){TRACE(#name);push2(((uint32_t)(&name)));NEXT;}
#else
#error undefined
#endif
#define VAR(name,value)	CELL_t name=(CELL_t)value;VARfn(name)
#define CONST(name,value)	void push_const_##name(){TRACE(#name);push(value); NEXT;}
#define CONST2(name,value)	void push_const_##name(){TRACE(#name);push2(value); NEXT;}

typedef enum { st_executing, st_compiling} st_STATE;
VARfn(LAST)	// LAST is in RAM, just points "somewhere"
st_STATE STATE=st_executing;
VARfn(STATE)
//uint8_t *HERE;
VARfn(HERE)
VAR(BASE,16)
CONST2(DOCOL,B3U32(f_docol))
CONST2(RAM,B3U32(RAM))
CONST2(RAM_END,B3U32(&RAM[RAM_LEN]))
CONST2(S0,B3U32(stck))
CONST2(S_END,B3U32(&stck[STACK_LEN]))
CONST2(R0,B3U32(Rstck))
CONST2(R_END,B3U32(&Rstck[RSTACK_LEN]))
void	print_stack() { 	// {{{ just simple debug print
	for (int8_t p=0;p<stack;p++) {write_char('[');write_hex16(stck[p]);write_char(']');};
}	// }}}
// {{{ dup, plus, ...
void f_dup(){	// {{{
	TRACE("DUP");
	push(peek());
	NEXT;
}	// }}}
void f_drop(){	// {{{
	TRACE("DROP");
	pop();
	NEXT;
}	// }}}
void f_swap() {	// {{{
	TRACE("SWAP");
	CELL_t a=pop();
	CELL_t b=pop();
	push(a);
	push(b);
	NEXT;
}	// }}}
void f_xor(){	// {{{
	TRACE("XOR");
	push(pop()^pop());
	NEXT;
}	// }}}
void f_or(){	// {{{
	TRACE("OR");
	push(pop()|pop());
	NEXT;
}	// }}}
void f_and(){	// {{{
	TRACE("AND");
	push(pop()&pop());
	NEXT;
}	// }}}
void f_Lor(){	// {{{
	TRACE("LOR");
	CELL_t a=pop();
	CELL_t b=pop();
	push((a||b)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_Land(){	// {{{
	TRACE("LAND");
	CELL_t a=pop();
	CELL_t b=pop();
	push((a&&b)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_plus(){	// {{{
	TRACE("+");
	push(pop()+pop());
	NEXT;
}	// }}}
void f_minus(){	// {{{
	TRACE("-");
	CELL_t c=pop();
	push(pop()-c);
	NEXT;
}	// }}}
void f_times() {	// {{{
	TRACE("*");
	push(pop()*pop());
	NEXT;
}	// }}}
void f_div() {	// {{{
	TRACE("/");
	CELL_t a=pop();
	push(pop()/a);
	NEXT;
}	// }}}
void f_div2() {	// {{{
	TRACE("/2");
	push(pop()/2);
	NEXT;
}	// }}}
void f_div4() {	// {{{
	TRACE("/4");
	push(pop()/4);
	NEXT;
}	// }}}
void f_dup_D() {	// {{{
	TRACE("DUP2");
	push2(peek2());
	NEXT;
}	// }}}
void f_dup_3() {	// {{{
	TRACE("DUP3");
	push2(peek2X(1));
	push(peekX(2));
	NEXT;
}	// }}}
void f_dup_4() {	// {{{
	TRACE("DUP4");
	push2(peek2X(2));
	push2(peek2X(2));
	NEXT;
}	// }}}
void f_rdrop() {	// {{{ // Drop from Rstack
	TRACE("RDROP");
	Rpop();
	NEXT;
}	// }}}
void f_drop_D() {	// {{{
	TRACE("DROP2");
	pop2();
	NEXT;
}	// }}}
void f_swap_D() {	// {{{
	TRACE("SWAP2");
	DOUBLE_t a=pop2();
	DOUBLE_t b=pop2();
	push2(a);
	push2(b);
	NEXT;
}	// }}}
void f_swap_12() {	// {{{ // ( c d -- d c )
	TRACE("SWAP12");
	DOUBLE_t d=pop2();
	CELL_t  c=pop();
	push2(d);
	push(c);
	NEXT;
}	// }}}
void f_swap_21() {	// {{{ // ( d c -- c d )
	TRACE("SWAP21");
	CELL_t  c=pop();
	DOUBLE_t d=pop2();
	push(c);
	push2(d);
	NEXT;
}	// }}}
void f_plus_D() {	// {{{
	TRACE("+D");
	push2(pop2()+pop2());
	NEXT;
}	// }}}
void f_minus_D() {	// {{{
	TRACE("-D");
	DOUBLE_t c=pop2();
	push2(pop2()-c);
	NEXT;
}	// }}}
void f_times_D() {	// {{{
	TRACE("*D");
	push2(pop2()*pop2());
	NEXT;
}	// }}}
void f_div_D() {	// {{{
	TRACE("/D");
	DOUBLE_t a=pop2();
	push2(pop2()/a);
	NEXT;
}	// }}}
void f_div2_D() {	// {{{
	TRACE("/2D");
	push2(pop2()/2);
	NEXT;
}	// }}}
void f_div4_D() {	// {{{
	TRACE("/4D");
	push2(pop2()/4);
	NEXT;
}	// }}}
void f_plus21() {	// {{{
	TRACE("+21");
	CELL_t  c=pop();
	DOUBLE_t d=pop2();
	push2(d+c);
	NEXT;
}	// }}}
void f_minus21() {	// {{{
	TRACE("-21");
	CELL_t  c=pop();
	DOUBLE_t d=pop2();
	push2(d-c);
	NEXT;
}	// }}}
void f_times21() {	// {{{
	TRACE("*21");
	CELL_t  c=pop();
	DOUBLE_t d=pop2();
	push2(d*c);
	NEXT;
}	// }}}
void f_div21() {	// {{{
	TRACE("/21");
	CELL_t  c=pop();
	DOUBLE_t d=pop2();
	push2(d/c);
	NEXT;
}	// }}}
void f_c2C() {	// {{{ ; cell -> 2 C
	TRACE("c2C");
	CELL_t c=pop();
	push((c>>8)&0xFF);
	push(c&0xFF);
	NEXT;
}	// }}}
void f_D4C() {	// {{{ ; Double -> 4C
	TRACE("D4C");
	DOUBLE_t d=pop2();
	push((d>>24)&0xFF);
	push((d>>16)&0xFF);
	push((d>>8)&0xFF);
	push(d&0xFF);
	NEXT;
}	// }}}
void f_2Cc() {	// {{{ ; 2 C -> cell
	TRACE("2Cc");
	CELL_t c=pop();
	push((pop()<<8)+c);
	NEXT;
}	// }}}
void f_4CD() {	// {{{ ; 4 C -> Double
	TRACE("4CD");
	DOUBLE_t d=pop();
	d+=pop()*0x100;
	d+=pop()*0x10000;
	d+=pop()*0x1000000;
	push2(d);
	
	NEXT;
}	// }}}
void f_1minus() {	// {{{ ; decrement TOS by 1
	TRACE("1-");
	stck[stack-1]--;
	NEXT;
}	// }}}
void f_4minus() {	// {{{ ; decrement TOS by 4
	TRACE("4-");
	stck[stack-1]-=4;
	NEXT;
}	// }}}
void f_1Dminus() {	// {{{ // decrement Double TOS by 1
	TRACE("1D-");
	push2(pop2()-1);
	NEXT;
}	// }}}
void f_4Dminus() {	// {{{ // decrement Double TOS by 4
	TRACE("4D-");
	push2(pop2()-4);
	NEXT;
}	// }}}
void f_1plus() {	// {{{ // increment TOS by 1
	TRACE("1+");
	push(pop()+1);
	NEXT;
}	// }}}
void f_4plus() {	// {{{ // increment TOS by 4
	TRACE("4+");
	push(pop()+4);
	NEXT;
}	// }}}
void f_1Dplus() {	// {{{ // increment Double TOS by 1
	TRACE("1D+");
	push2(pop2()+1);
	NEXT;
}	// }}}
void f_4Dplus() {	// {{{ // increment Double TOS by 4
	TRACE("4D+");
	push2(pop2()+4);
	NEXT;
}	// }}}
void f_Store(){	// {{{ ! ( cell Daddr --  ) store cell at address(Double)
	TRACE("!");
	DOUBLE_t d=pop2();
	#ifdef PEDANT
		if (! is_in_Wrange(d)) { do_traceback(); ERROR("Not in range");return;};
	#endif
	*(CELL_t*)B3PTR(d)=pop();
	NEXT;
}	// }}}
void f_StoreChar(){	// {{{ !C ( char Daddr -- ) store char at address(Double)
	TRACE("!C");
	DOUBLE_t d=pop2();
	uint8_t v=pop();
	#ifdef PEDANT
		if (! is_in_Wrange(d)) { do_traceback(); ERROR("Not in range");return;};
	#endif
	*(uint8_t*)B3PTR(d)=v;
	NEXT;
}	// }}}
void f_StoreDouble(){	// {{{ !D ( D Daddr -- ) store Double at address(Double)
	TRACE("!D");
	DOUBLE_t d=pop2();
	#ifdef PEDANT
		if (! is_in_Wrange(d)) { do_traceback(); ERROR("Not in range");return;};
	#endif
	*(uint32_t*)B3PTR(d)=pop2();
	NEXT;
}	// }}}
void f_At(){	// {{{ @ ( Daddr -- cell ) cell at address(Double)
	TRACE("@");
	DOUBLE_t d=pop2();
	#ifdef PEDANT
		if (! is_in_range(d)) { do_traceback(); ERROR("Not in range");return;};
	#endif
	push(B2at(d));
	NEXT;
}	// }}}
void f_CharAt(){	// {{{ C@ ( Daddr -- char ) char at address(Double)
	TRACE("C@");
	DOUBLE_t d=pop2();
	#ifdef PEDANT
		if (! is_in_range(d)) { do_traceback(); ERROR("Not in range");return;};
	#endif
	push(B2at(d)&0xFF);
	NEXT;
}	// }}}
void f_DoubleAt(){	// {{{ D@ ( Daddr -- D ) Double at address(Double)
	TRACE("D@");
	DOUBLE_t d=pop2();
	#ifdef PEDANT
		if (! is_in_range(d)) { do_traceback(); ERROR("Not in range");return;};
	#endif
	push2(B4at(d));
	NEXT;
}	// }}}
void f_ToR() {	// {{{ // ( u -- ; R: -- r ) Move to Rstack
	TRACE(">R");
	Rpush(pop());
	NEXT;
}	// }}}
void f_DoubleToR() {	// {{{ // ( D -- ; R: -- r ) Move Double to Rstack (still one position on R)
	TRACE("D>R");
	Rpush(pop2());
	NEXT;
}	// }}}
void f_FromR() {	// {{{ // ( -- u ; R: r -- ) Move from Rstack
	TRACE("R>");
	push(Rpop());
	NEXT;
}	// }}}
void f_DoubleFromR() {	// {{{ // ( -- D ; R: r -- ) Move Double from Rstack (still one position on R)
	TRACE("R>D");
	push2(Rpop());
	NEXT;
}	// }}}
void f_CellAtR() {	// {{{ // ( -- u ; R: r -- r ) Peek from Rstack
	TRACE("@R");
	push(Rpeek());
	NEXT;
}	// }}}
void f_DoubleAtR() {	// {{{ // ( -- D ; R: r -- r ) Peek from Rstack
	TRACE("D@R");
	push2(Rpeek());
	NEXT;
}	// }}}
void f_StackAddress() {	// {{{ // ( -- D ) Address, where Stack points
	TRACE("S?");
	push2(B3U32(&stck[stack]));
	NEXT;
}	// }}}
void f_SetStack() {	// {{{ // ( D -- ?? ) Set Stack Address
	TRACE("S!");
	int32_t S=pop2()-B3U32(&stck[0]);
	if ((S<0) || (S>STACK_LEN*sizeof(stck[0]))) { ERROR("Out of stack area"); NEXT; return;};
	int32_t s=S/sizeof(stck[0]);
	if (s*sizeof(stck[0]) != S) { ERROR("Bad alligned stack"); NEXT; return;};
	stack=s;
	NEXT;
}	// }}}
void f_RStackAddress() {	// {{{ // ( -- D ) Address, where Rstack points
	TRACE("R?");
	push2(B3U32(&Rstck[Rstack]));
	NEXT;
}	// }}}
void f_SetRStack() {	// {{{ // ( D -- R: ?? ) Set Rstack Address
	TRACE("R!");
	int32_t S=pop2()-B3U32(&Rstck[0]);
	if ((S<0) || (S>RSTACK_LEN*sizeof(Rstck[0]))) { ERROR("Out of Rstack area"); NEXT; return;};
	int32_t s=S/sizeof(Rstck[0]);
	if (s*sizeof(Rstck[0]) != S) { ERROR("Bad alligned Rstack"); NEXT; return;};
	Rstack=s;
	NEXT;
}	// }}}
void f_hex(){	// {{{
	TRACE("hex");
	BASE=16;
	NEXT;
}	// }}}
void f_dec(){	// {{{
	TRACE("dec");
	BASE=10;
	NEXT;
}	// }}}
void f_bin(){	// {{{
	TRACE("bin");
	BASE=2;
	NEXT;
}	// }}}
// }}}
DOUBLE_t cw2h(DOUBLE_t cw) {	// {{{ codeword address to head address
//	TRACE("cw2h");
	if ( ! (
		( (B3U32(&w_firtsbuildinword_head) <= cw) && ( cw < B3U32(&w_lastbuildinword_end)) ) ||
		( (B3U32(&RAM[0]) <= cw) && ( cw < HERE ) )
		) ) { return 0; };
	if (!cw) return 0;
	if (!is_in_range(cw)) return 0;
	uint8_t i =0;
	cw--;
	while ((i<33 ) && (i!=B1at(cw))) {i++;cw--;};
	if (i<33) return cw-5;
	return 0;
}	// }}}
void f_cw2h() {	// {{{ ; ( cw -- h ) convert codeword address to head address
	TRACE("f_cw2h");
	push2(cw2h(pop2()));
	NEXT;
}	// }}}
DOUBLE_t h2cw(DOUBLE_t h) {	// {{{ convert head address to codeword address
	h+=5;
	h+=1+B1at(h);
	return h;
}	// }}}
void f_h2cw() {	// {{{ // ( h -- cw ) convert head address to codeword address
	TRACE("h2cw");
	push2(h2cw(pop2()));
	NEXT;
}	// }}}
void f_h2da() {	// {{{ // ( h -- da ) convert head address to data address
	TRACE("h2da");
	push2(h2cw(pop2())+4);
	NEXT;
}	// }}}
bool get_bounds_of_word(DOUBLE_t addr,DOUBLE_t *start, DOUBLE_t *stop) {	// {{{ // Find word at addr and returns start&stop, or false
	*start=LAST;
	*stop=HERE;
// printf("get_bounds_of_word(0x%08x, 0x%p -> 0x%08x, 0x%p -> 0x%08x)\n",addr, start, *start,stop,*stop);
	while (*start) {
// printf("get_bounds_of_word loop( 0x%p -> 0x%08x, 0x%p -> 0x%08x)\n", start, *start,stop,*stop);
		if ( (*start<=addr) && (addr<*stop)) {
			uint8_t len=B1at(*start+5);
//			printf("len: %d\n",len);
			if (len>31) return false;  // misformed word
			(*stop)-=4;
			return true;
		};
		*stop=*start;
		*start=B4at(*start);
	};
//	printf("end\n");
	return false;
}	// }}}
uint8_t name_to_buf(DOUBLE_t cw) {	// {{{ fill name into global buf codeword - return flags
	DOUBLE_t h=cw2h(cw);
	if (!h) {ERROR("Not a word");strcpy_P(buf,F(" Not a word "));return 0;};
	uint8_t flags,len;
	flags=B1at(h+4);
	len=B1at(h+5);
	uint8_t i=0;
	for (; i<len;i++) buf[i]=B1at(h+6+i);
	buf[i]=0;
	return flags;
}	// }}}
#if defined(__PC__)
void f_memdump() {	// {{{ // ( daddr len -- ) dump data to file
	INFO("memdump");
	DOUBLE_t addr2=pop2();
	DOUBLE_t addr=pop2();
	const char  *fname = "._dump.bin";
	memdump(addr,addr2-addr,fname);
	NEXT;
}	// }}}
void file_do_export(FILE *f, DOUBLE_t h, DOUBLE_t top) {	// {{{
	DOUBLE_t cw=h2cw(h);
	DOUBLE_t val;
	uint8_t flags;
	if (!h) {fprintf(f," Not a word ");return;};
	fprintf(f, ": ");
	flags = name_to_buf(cw);
	fprintf(f,"%s",buf);
	if (val_of_f_docol != B3at(cw)) {
		fprintf(f," NOT_DOCOL definition ");
		return;	// neumim rozepsat
		};
	do {
		cw+=4;
		val=B4at(cw);
//		if (val == val_of_w_exit_cw) break;
		fprintf(f," ");
		flags = name_to_buf(val);
		fprintf(f,"%s",buf);
		if (flags & FLG_ARG) {
			cw+=4;
			val=B4at(cw);
			fprintf(f," ");
			DOUBLE_t h1, h2;
			if ( get_bounds_of_word(val, &h1, &h2)) { // it MAY be word
				if (h2cw(h1)==val) {	// it points to cw of existing word
					name_to_buf(val);
					fprintf(f,"%s ( 0x%08x )",buf,val);
				} else {
					fprintf(f,"\\'0x%08x ", val);
				};
			} else {
				fprintf(f,"\\'0x%08x ", val);
			};
			val=0;
		}
		else if ((flags & FLG_PSTRING) && (B4at(cw+4)<128)) { // too long strings probabely are not arguments
			cw+=4;
			val=B4at(cw);
			fprintf(f," \\'0x%08x", val);
			cw+=4;
			fprintf(f," \\\"");
			while (val--) fprintf(f,"%c",B1at(cw++));
			fprintf(f,"\"" );
			cw-=4;	// we are already at next pos, but loop will add 4 anyway
			val=0;
		};
	} while (cw < top);
}	// }}}
void traceback_word(FILE *f, uint16_t depth, DOUBLE_t addr) {	// {{{ // Find word and dump actual position in it
	for (int8_t d=0;d<=depth;d++) fprintf(f,"\t");
	fprintf(f,"0x%08x .. ",addr);
	fflush(f);
	DOUBLE_t start,stop;
	if (get_bounds_of_word(addr,&start,&stop)) {
			uint8_t len=B1at(start+5);
			for (int8_t i=0;i<len; i++) fprintf(f,"%c",B1at(start+6+i));
			fprintf(f," : at data+%d :", (addr-start-10-len));
			fflush(f);
			file_do_export(f, start, addr);
			fprintf(f,"\n");
	} else {
		fprintf(f,"NOT A WORD, maybe value? %d\n", addr);
	};
}	// }}}
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
void do_traceback(){	// {{{ dump traceback data to file
	FILE *f = fopen("._traceback.log","a");
	if (!f) {
		perror("fopen");
		return;
	};
	fprintf(f,"\n\n\n<==== traceback ====>\n");
	fprintf(f,"IP=0x%08x\nStack (depth: %d) :",IP,stack);
	for (int8_t p=0;p<stack;p++) fprintf(f,"[%04x]",stck[p]);
	fprintf(f,"\t\t\t and maybe ");
	for (int8_t p=0;p<min(STACK_LEN-stack,8);p++) fprintf(f,"[%04x]",stck[stack+p]);
	fprintf(f,"\nRStack (depth: %d) :",Rstack);
	for (int8_t p=0;p<Rstack;p++) fprintf(f,"[%08x]",Rstck[p]);
	fprintf(f,"\t\t\t and maybe ");
	for (int8_t p=0;p<min(RSTACK_LEN-Rstack,8);p++) fprintf(f,"[%04x]",Rstck[Rstack+p]);
	fprintf(f,"\nRAM: 0x%08x, HERE: 0x%08x, LAST:0x%08x\n",B3U32(RAM),HERE,LAST);
	fprintf(f,"\n	<==== analyze ====>\n");
	fflush(f);
	for (int8_t p=0;p<Rstack;p++) traceback_word(f,p,Rstck[p]-4);
	traceback_word(f,Rstack+1,IP-4);
	fprintf(f,"\n	<==== analyze end ====>\n\n\n");
	fclose(f);
	return;
}	// }}}
void f_traceback() {	// {{{ // ( -- ) dump traceback data to file
	INFO("traceback");
	do_traceback();
	NEXT;
}	// }}}
#endif
uint8_t show_name(DOUBLE_t cw, DOUBLE_t orig) {	// {{{ show name and address from codeword - return flags
	DEBUG_DUMP(cw,"cw\t");
	DOUBLE_t h=cw2h(cw);
	if (!h) {ERROR("Not a word");return 0;};
	uint8_t flags,len;
	flags=B1at(h+4);
	len=B1at(h+5);
	if (flags & FLG_HIDDEN) write_str(F("HIDDEN "));
//	if (flags & FLG_IMMEDIATE) write_str(F("IMMEDIATE "));
//	if (flags & FLG_ARG) write_str(F("ARG "));
//	write_str(F("len: "));write_hex8(len);write_str(F(", name: "));
	write_str(F(STR_2LESS));
	for (uint8_t i=0; i<len;i++) write_char(B1at(h+6+i));
	write_str(F(STR_2MORE));
	if (!noinfo) {
		write_str(F("\t\t ["));write_hex32(orig);
		write_str(F("] -> "));write_hex32(B3at(cw));
		write_str(F(" ( head: "));write_hex32(h);
//		write_str(F("; prev: ")); write_hex24(B3at(h));
		write_str(F(") "));
		};
	return flags;
}	// }}}
void show(DOUBLE_t cw) {	// {{{ ; ' WORD show - try to show definition of WORD
	TRACE("show");
	DOUBLE_t h=cw2h(cw);
	DOUBLE_t val;
	write_eoln();
	uint8_t flags;
	if (!h) {ERROR("Not a word");return;};
//	show_name(cw, 0);
	flags=name_to_buf(cw);
	write_str(F(STR_2LESS));
	write_str(buf);
	write_str(F(STR_2MORE));
	if (flags & FLG_HIDDEN) write_str(F(" HIDDEN "));
	if (flags & FLG_IMMEDIATE) write_str(F(" IMMEDIATE "));
	write_eoln();
	if (val_of_f_docol != B3at(cw)) return;	// neumim rozepsat
	DOUBLE_t start,stop;
	if (! get_bounds_of_word(cw,&start,&stop)) {
		write_str(F("Failed\n"));
		return;
	};
	do {
		cw+=4;
	DEBUG_DUMP(cw,"code\t");
		val=B4at(cw);
		write_str(F("\t"));
//		write_str(F("\t["));
//		write_hex32(val);
//		write_str(F("] "));
//		flags=show_name(val,cw);
		flags=name_to_buf(val);
		write_str(buf);
		if (!noinfo) {
			write_str(F("\t\t ("));write_hex32(cw);write_str(F(") "));
		};
		if (flags & FLG_ARG) {
			cw+=4;
			val=B4at(cw);
			write_str(F("\r\n\t\t["));
			DOUBLE_t h1, h2;
			if ( get_bounds_of_word(val, &h1, &h2)) { // it MAY be word
				if (h2cw(h1)==val) {	// it points to cw of existing word
					name_to_buf(val);
					write_str(buf);
					write_str(F(" ("));
					write_hex32(val);
					write_str(F(")"));
				} else {
					write_hex32(val);
				};
			} else {
				write_hex32(val);
			};
			write_str(F("]"));
			val=0;
		}
		else if ((flags & FLG_PSTRING) && (B4at(cw+4)<128)) { // too long strings probabely are not arguments
			cw+=4;
			val=B4at(cw);
			cw+=4;
			write_str(F("\r\n\t\t["));
			write_hex32(val);
			write_str(F(" '"));
			while (val--) write_charA(B1at(cw++));
			write_str(F("']"));
			cw-=4;	// we are already at next pos, but loop will add 4 anyway
			val=0;
		};
		write_eoln();
	} while (cw<stop);

}	// }}}
void f_show() {	// {{{ ; ' WORD show - try to show definition of WORD
	TRACE("show");
	DOUBLE_t cw=pop2();
	show(cw);
	NEXT;
}	// }}}
void do_export(DOUBLE_t addr) {	// {{{ ; ' WORD export - try to export definition of WORD
	TRACE("do_export");
	DOUBLE_t start,stop;
	if (! get_bounds_of_word(addr,&start,&stop)) {
// printf("\ndo_export get_bounds_of_word(0x%08x, 0x%08x, 0x%08x)\n",addr, start, stop);
		ERROR("Not a word");return;
	};
//	DOUBLE_t cw=h2cw(start);
	DOUBLE_t cw=h2cw(start);
	DOUBLE_t val;
	uint8_t flags;
	if (!start) {ERROR("Not a word");return;};
	write_eoln();
	write_str(F(": "));
	flags = name_to_buf(cw);
	write_str(buf);
	if (flags & FLG_IMMEDIATE) write_str(F(" IMMEDIATE"));
	if (val_of_f_docol != B3at(cw)) {
		write_str(F(" NOT_DOCOL definition "));
		return;	// neumim rozepsat
		};
	do {
		cw+=4;
		val=B4at(cw);
		if ((val == val_of_w_exit_cw) && (cw>=stop)) break;
		write_char(' ');
		flags=name_to_buf(val);
		write_str(buf);
		if (flags & FLG_ARG) {
			cw+=4;
			val=B4at(cw);
			write_char(' ');
			DOUBLE_t h1, h2;
			if ( get_bounds_of_word(val, &h1, &h2)) { // it MAY be word
				if (h2cw(h1)==val) {	// it points to cw of existing word
					name_to_buf(val);
					write_str(buf);
					write_str(F(" ( \\'0x"));
					write_hex32(val);
					write_str(F(" )"));
				} else {
					write_str(F("\\'0x"));
					write_hex32(val);
				};
			} else {
				write_str(F("\\'0x"));
				write_hex32(val);
			};
			val=0;
		}
		else if ((flags & FLG_PSTRING) && (B4at(cw+4)<128)) { // too long strings probabely are not arguments
			cw+=4;
			val=B4at(cw);
			write_str(F(" \\'0x"));
			write_hex32(val);
			cw+=4;
			write_str(F(" \\\""));
			while (val--) write_charA(B1at(cw++));
			write_str(F("\""));
			cw-=4;	// we are already at next pos, but loop will add 4 anyway
			val=0;
		};
	} while (cw < stop);
//	} while (val != val_of_w_exit_cw);
	write_str(F(" ;"));
	write_eoln();

}	// }}}
void f_export() {	// {{{ ; ' WORD export - try to export definition of WORD
	TRACE("export");
	DOUBLE_t cw=pop2();
	do_export(cw);
	NEXT;
}	// }}}
void f_export_all() {	// {{{ // (  -- ) try to export definitions of all words for forth2inc.py
	INFO("export_all");
	DOUBLE_t h=LAST;
	while (h>=B3U32(RAM)) {
		do_export(h);
		h=B4at(h);
	};
	NEXT;
}	// }}}
void f_stack() {	// {{{ // ( -- ) nonestructiove print of stack (for debug mainly)
	INFO(".s");
	write_eoln();
	print_stack();
	NEXT;
}	// }}}
void f_key(){	 // {{{ WAITS for char and puts it on stack
	push(wait_for_char());
	NEXT;
}	// }}}
void f_emit() { 	 // {{{ ; ( c -- ) emits character
	TRACE("EMIT");
	CELL_t c=pop();
	write_char(c & 0xFF);
	NEXT;
}	// }}}
void f_word() {	 // {{{ Put address and size of buff to stack
	TRACE("WORD");
	get_word();
	push2(B3U32(&word_buf));
	push(word_buf_len);
	NEXT;
}	// }}}
void f_docol() {	// {{{
	TRACE("docol");
// ERROR("Press ANY key to continue");wait_for_char();
	Rpush(IP);
	IP=DT+4;	// README: DT points to 4B codeword, so next address is DT+4B and now it is on Data[0] in the target header
	if (! notrace) {
		write_eoln();
		for (uint8_t i=0; i<Rstack; ++i) write_char('\t');
		uint32_t p=cw2h(DT);
		if (p!=0) {
			p+=5;
			uint8_t i,l=B1at(p++);
			buf[l+2]=0;
			i=0;
			buf[i++]='[';
			while (l!=0) {buf[i++]=B1at(p++);l--;};
			buf[i++]=']';
			buf[i++]=0;
			trace(buf);
			};
		};
	DEBUG_DUMP(IP,"IP in f_docol	");
	DEBUG_DUMP(DT,"DT in f_docol	");
	NEXT;
}	// }}}
void f_exit(){	// {{{
	TRACE("EXIT");
	IP=Rpop();
	if (! notrace) {
		write_eoln();
		for (uint8_t i=0; i<Rstack; ++i) write_char('\t');
		};
	NEXT;
}	// }}}
void f_lit(){	// {{{ README: LIT takes the next 4B pointer as 2B integer, ignores the top byte. This is done for taking the same 4B alingment in data
	TRACE("LIT");
	push(B2at(IP));
	IP+=4;
	NEXT;
}	// }}}
void f_lit2(){	// {{{ README: LIT takes the next 4B pointer as 4B integer. This is done for taking the same 4B alingment in data
	TRACE("LIT2");
	push2(B4at(IP));
	IP+=4;
	NEXT;
}	// }}}
void comma(uint32_t d) {	// {{{
	TRACE(",");
	*(uint32_t*)B3PTR(HERE)=d;
	HERE+=4;
}	// }}}
/*
void comma(Data_t d) {	// {{{
	INFO("comma");
	*(Data_t*)HERE=d;
	HERE+=4;
}	// }}}
*/
void f_comma() {	// {{{ take 3B address (2 CELLs) from datastack and put it to HERE
	TRACE(",");
	CELL_t c=pop();
	*(CELL_t *)B3PTR(HERE)=c;
	HERE+=2;
	c=pop();
	*(CELL_t *)B3PTR(HERE)=c;
	HERE+=2;
	NEXT;
}	// }}}
void f_dot() { 	 // {{{
	TRACE(".");
	CELL_t c=pop();
	itoa(c, buf, BASE);
	write_str(&buf[0]);
	NEXT;
}	// }}}

void f_number() {	// {{{ (Daddr n -- val rest ) rest= #neprevedenych znaku
	TRACE("NUMBER");
	CELL_t i=pop();
	char *buf=(char *)B3PTR(pop2());
	char *end;CELL_t c=strtoul(buf,&end,BASE);
	push(c);push(i-(end-buf));
	NEXT;
}	// }}}
void f_branch(){ 	 // {{{ BRANCH .long offset ; offset -4 loop; offset +4 nop;
	TRACE("BRANCH");
	int32_t c=B4at(IP);
	IP+=c;
	NEXT;
}	// }}}
void f_tick() {	// {{{ ; push CW_address of next word to stack (and skip it)
	TRACE("'");
	push2(B4at(IP));
	IP+=4;

	/*
	if (STATE == st_executing) { // st_executing
	INFO("st_executing");
		get_word();
		xpHead1 h=findHead(word_buf_len,&word_buf[0],B3PTR(LAST));
//		write_hex32(B3U32(h));
		push2(get_codeword_addr(h));
	} else {
	INFO("st_compiling");
		*(uint32_t*)B3PTR(HERE)=B3U32(&w_lit2_cw);
		HERE+=4;
//		int32_t c=B4at(IP);
//		IP+=4;
//		push2(c);
	};
	*/
	NEXT;
}	// }}}
void f_immediate() {	// {{{ ;  IMMEDIATE make the last word immediate
	TRACE("IMMEDIATE");
	uint32_t h=LAST;
	*(uint8_t *)B3PTR(h+4) |= FLG_IMMEDIATE;
	NEXT;
}	// }}}
void f_0branch(){ 	 // {{{	; branch if zero; 0BRANCH .long offset ; offset -4 loop; offset +4 nop;
	TRACE("0BRANCH");
	int32_t c=B4at(IP);
	if ( pop()) c=4;
	IP+=c;
	NEXT;
}	// }}}
void f_zero() {	// {{{ ; true if zero
	TRACE("==0");
	push((pop()==0)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_notzero() {	// {{{ ; true if not zero
	TRACE("!=0");
	push((pop()!=0)?F_FALSE:F_TRUE);
	NEXT;
}	// }}}
void f_positive() {	// {{{ ; true if positive
	TRACE(">0");
	push(((S_CELL_t)pop()>0)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_positive0() {	// {{{ ; true if positive or zero
	TRACE(">=0");
	push(((S_CELL_t)pop()>=0)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_negative() {	// {{{ ; true if negative
	TRACE("<0");
	push(((S_CELL_t)pop()<0)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_negative0() {	// {{{ ; true if negative or zero
	TRACE("<=0");
	push(((S_CELL_t)pop()<=0)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_notequal() {	// {{{ ; (c1 c2 -- flag ) true if notequal
	TRACE("<>");
	push((pop()!=pop())?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_notequalD() {	// {{{ ; (d1 d2 -- flag ) true if notequal
	TRACE("<>D");
	push((pop2()!=pop2())?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_equal() {	// {{{ ; (c1 c2 -- flag ) true if equal
	TRACE("=");
	push((pop()==pop())?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_equalD() {	// {{{ ; (d1 d2 -- flag ) true if equal
	TRACE("=D");
	push((pop2()==pop2())?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_greater() {	// {{{ // (c1 c2 -- flag ) true if greater
	TRACE(">");
	CELL_t c1,c2;
	c2=pop();
	c1=pop();
	push((c1>c2)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_greaterequal() {	// {{{ // (c1 c2 -- flag ) true if greaterequal
	TRACE(">=");
	CELL_t c1,c2;
	c2=pop();
	c1=pop();
	push((c1>=c2)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_less() {	// {{{ // (c1 c2 -- flag ) true if less
	TRACE("<");
	CELL_t c1,c2;
	c2=pop();
	c1=pop();
	push((c1<c2)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_lessequal() {	// {{{ // (c1 c2 -- flag ) true if lessequal
	TRACE("<=");
	CELL_t c1,c2;
	c2=pop();
	c1=pop();
	push((c1<=c2)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_greaterD() {	// {{{ // (c1 c2 -- flag ) true if greater
	TRACE(">D");
	DOUBLE_t d1,d2;
	d2=pop2();
	d1=pop2();
	push((d1>d2)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_greaterequalD() {	// {{{ // (c1 c2 -- flag ) true if greaterequal
	TRACE(">=D");
	DOUBLE_t d1,d2;
	d2=pop2();
	d1=pop2();
	push((d1>=d2)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_lessD() {	// {{{ // (c1 c2 -- flag ) true if less
	TRACE("<D");
	DOUBLE_t d1,d2;
	d2=pop2();
	d1=pop2();
	push((d1<d2)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_lessequalD() {	// {{{ // (d1 d2 -- flag ) true if lessequal
	TRACE("<=D");
	DOUBLE_t d1,d2;
	d2=pop2();
	d1=pop2();
	push((d1<=d2)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_notnull() {	// {{{ // (daddr -- flag ) true if daddr is not NULL
	TRACE("NOTNULL");
	DOUBLE_t d=pop2();
	push((d & ~0x800000)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_isnull() {	// {{{ // (daddr -- flag ) true if daddr is NULL
	INFO("ISNULL");
	DOUBLE_t d=pop2();
	push((d & ~0x800000)?F_FALSE:F_TRUE);
	NEXT;
}	// }}}
void f_DIVMOD() {	// {{{ // (c1 c2 -- c1/c2 c1%c2 )
	TRACE("/MOD");
	CELL_t c1,c2;
	c2=pop();
	c1=pop();
	push(c1%c2);
	push(c1/c2);
	NEXT;
}	// }}}
void f_CHAR() {	// {{{ // ( -- C) read one char
	TRACE("CHAR");
	get_word();
	push(word_buf[0]);
	NEXT;
}	// }}}
	void f_OVER() {	// {{{ // ( c1 c2 -- c1 c2 c1 )
	TRACE("OVER");
	push(peekX(1));
	NEXT;
}	// }}}
	void f_OVER2() {	// {{{ // ( d1 d2 -- d1 d2 d1 )
	TRACE("OVER2");
	push2(peek2X(2));
	NEXT;
}	// }}}
	void f_OVER12() {	// {{{ // ( c1 d2 -- c1 d2 c1 )
	TRACE("OVER12");
	push(peekX(2));
	NEXT;
}	// }}}
	void f_OVER21() {	// {{{ // ( d1 c2 -- d1 c2 d1 )
	TRACE("OVER21");
	push(peekX(2));
	push(peekX(2));
	NEXT;
}	// }}}
	void f_ROT() {	// {{{ // ( c1 c2 c3 --  c2 c3 c1 )
	TRACE("ROT");
	CELL_t c1,c2,c3;
	c3=pop();
	c2=pop();
	c1=pop();
	push(c2);
	push(c3);
	push(c1);
	NEXT;
}	// }}}
	void f_NROT() {	// {{{ // ( c1 c2 c3 -- c3 c1 c2 )
	TRACE("NROT");
	CELL_t c1,c2,c3;
	c3=pop();
	c2=pop();
	c1=pop();
	push(c3);
	push(c1);
	push(c2);
	NEXT;
}	// }}}
	void f_ROT4() {	// {{{ // ( c1 c2 c3 c4 --  c2 c3 c4 c1 )
	TRACE("ROT4");
	CELL_t c1,c2,c3,c4;
	c4=pop();
	c3=pop();
	c2=pop();
	c1=pop();
	push(c2);
	push(c3);
	push(c4);
	push(c1);
	NEXT;
}	// }}}
	void f_NROT4() {	// {{{ // ( c1 c2 c3 c4 -- c4 c1 c2 c3 )
	TRACE("NROT4");
	CELL_t c1,c2,c3,c4;
	c4=pop();
	c3=pop();
	c2=pop();
	c1=pop();
	push(c4);
	push(c1);
	push(c2);
	push(c3);
	NEXT;
}	// }}}
	void f_QDUP() {	// {{{ // duplicate top of stack if non-zero
	TRACE("?DUP");
	CELL_t c=peek();
	if (c) push(c);
	NEXT;
}	// }}}
	void f_QDUPD() {	// {{{ // duplicate Double top of stack if non-zero
	TRACE("?DUP2");
	DOUBLE_t d=peek2();
	if (d) push2(d);
	NEXT;
}	// }}}
	void f_INVERT() {	// {{{ // this is the FORTH bitwise "NOT" function (cf. NEGATE and NOT)
	TRACE("INVERT");
	push(~pop());
	NEXT;
}	// }}}
	void f_ADDSTORE() {	// {{{ // ( c Daddr -- ) [Daddr] += c
	TRACE("+!");
	DOUBLE_t d=pop2();
	CELL_t c=pop();
	*(CELL_t *)B3PTR(d)+=c;
	NEXT;
}	// }}}
	void f_SUBSTORE() {	// {{{ // ( c Daddr -- ) [Daddr] -= c
	TRACE("-!");
	DOUBLE_t d=pop2();
	CELL_t c=pop();
	*(CELL_t *)B3PTR(d)-=c;
	NEXT;
}	// }}}
	void f_ADDSTORED() {	// {{{ // ( d Daddr -- ) [Daddr] += d Double
	INFO("+!D");
	DOUBLE_t d=pop2();
	DOUBLE_t c=pop();
	*(DOUBLE_t *)B3PTR(d)+=c;
	NEXT;
}	// }}}
	void f_SUBSTORED() {	// {{{ // ( d Daddr -- ) [Daddr] -= d Double
	INFO("-!D");
	DOUBLE_t d=pop2();
	DOUBLE_t c=pop();
	*(DOUBLE_t *)B3PTR(d)-=c;
	NEXT;
}	// }}}
	void f_CMOVE() {	// {{{ // ( saddr daddr len -- ) CharMove (len) from saddr to daddr
	TRACE("CMOVE");
	CELL_t l=pop();
	DOUBLE_t d=pop2();
	DOUBLE_t s=pop2();
	for(CELL_t i=0;i<l;i++) *(char*)B3PTR(d+i)=B1at(s+i);
	NEXT;
}	// }}}
	void f_LITSTRING() {	// {{{ // ( -- daddr len ) push daddr and len of string on the stach - similar to LIT
	TRACE("LITSTRING");
	CELL_t len=B2at(IP);
	IP+=4;
	push2(IP);
	push(len);
	IP+=len;
	NEXT;
}	// }}}
	void f_TELL() {	// {{{ // ( daddr len -- ) prints out string
	TRACE("TELL");
	CELL_t len = pop();
	DOUBLE_t addr=pop2();
	for (uint32_t i=0; i<len; ++i) { write_char(B1at(addr+i));};
	NEXT;
}	// }}}
	void f_EXECUTE() {	// {{{ // ( xt -- ) execute xt (&cw)
	INFO("EXECUTE");
	IP=pop2();
	NEXT;
}	// }}}
void f_ISINSTR() {	// {{{ // ( char addr len  -- flag ) if char is in string
	INFO("ISINSTR");
	CELL_t l=pop();
	DOUBLE_t addr=pop2();
	CELL_t c=pop();
	uint32_t i=0;
	for (; i<l; ++i,++addr) if (c==B1at(addr)) break;
	if (i<l) {
		push(F_TRUE);
	} else {
		push(F_FALSE);
	};
	NEXT;
}	// }}}
void f_POS() {	// {{{ // ( char addr len  -- (pos) flag ) return flag and if found also zero based position
	INFO("POS");
	CELL_t l=pop();
	DOUBLE_t addr=pop2();
	CELL_t c=pop();
	uint32_t i=0;
	for (; i<l; ++i,++addr) if (c==B1at(addr)) break;
	if (i<l) {
		push(i); push(F_TRUE);
	} else {
		push(F_FALSE);
	};
	NEXT;
}	// }}}
void f_RANDOM() {	// {{{ // ( max -- rnd ) rnd 0..(max -1)
	INFO("RANDOM");
	push(rand() % pop())
	NEXT;
}	// }}}
void f_getword() {	// {{{ // ( addr -- h end ) return range of word, in which the addr is
	INFO("getword");
	DOUBLE_t addr=pop2();
	DOUBLE_t start,stop;
	if (! get_bounds_of_word(addr,&start,&stop)) {
		push2(0); push2(0);
	} else {
		push2(start); push2(stop);
	};
	NEXT;
}	// }}}
void f_firtsbuildinword() {	// {{{ // ( -- h ) firts build in word - put its header addr on stack
	INFO("firtsbuildinword");
	push2(B3U32(&w_firtsbuildinword_head));
	NEXT;
}	// }}}
void f_lastbuildinword() {	// {{{ // ( -- h ) last build in word - put its header addr on stack
	INFO("lastbuildinword");
	push2(B3U32(&w_lastbuildinword_head));
	NEXT;
}	// }}}
#if defined(__PORTABLE_GRAPHIC__)
#if OUTPUT_TARGET == OUTPUT_TARGET_vram
extern void BIOS_clear(char c, int col);
extern void BIOS_set_cursor(uint8_t row, uint8_t col);
extern uint16_t BIOS_get_key();
extern void BIOS_wait(unsigned int dt);
extern uint8_t vram[BIOS_ROWS][BIOS_COLS];
void f_VRAM_yx() {	// {{{ // ( y x -- daddr ) y row, x column, daddr addr in VRAM
	INFO("VRAM_yx");
	CELL_t x=pop();
	CELL_t y=pop();
	push2(B3U32(&vram[y][x]));
	NEXT;
}	// }}}
void f_fetchVRAM_yx() {	// {{{ // ( y x -- c ) y row, x column, c character in VRAM
	INFO("VRAM_yx@");
	CELL_t x=pop();
	CELL_t y=pop();
	if( (x<MAX_COLS) && (y<MAX_ROWS)) {
		push(vram[y][x]);
	} else {
		push(0);
		ERROR("VRAM_yx@ out");
		write_hex16(x);
		write_char('x');
		write_hex16(y);
	};
	NEXT;
}	// }}}
void f_storeVRAM_yx() {	// {{{ // ( y x c -- ) y row, x column, c character in VRAM
	INFO("VRAM_yx!");
	CELL_t c=pop();
	CELL_t x=pop();
	CELL_t y=pop();
	if( (x<MAX_COLS) && (y<MAX_ROWS)) {
		vram[y][x]=c;
	} else {
		ERROR("VRAM_yx! out");
		write_hex16(x);
		write_char('x');
		write_hex16(y);
		write_char('=');
		write_hex8(c);
	};
	
	NEXT;
}	// }}}

void f_CLS() {	// {{{
	INFO("CLS");
	BIOS_clear(' ', 0b1111);
	NEXT;
}	// }}}
void f_CUR_yx() {	// {{{ // ( y x -- ) move cursor to  y row, x column
	INFO("CUR_yx");
	CELL_t x=pop();
	CELL_t y=pop();
	BIOS_set_cursor(y,x);
	NEXT;
}	// }}}
void f_KEYpress() {	// {{{ // ( -- c ) ascii of pressed key or 0
	INFO("KEYpress");
	push(BIOS_get_key());
	NEXT;
}	// }}}
void f_WAIT() {	// {{{ // ( c -- ) bios.wait(c)
	INFO("WAIT");
	BIOS_wait(pop());
	NEXT;
}	// }}}
#elif OUTPUT_TARGET == OUTPUT_TARGET_terminal

uint8_t vram[MAX_ROWS][MAX_COLS];
void f_CLS() {	// {{{
	INFO("CLS");
	memset(&vram,' ',MAX_ROWS * MAX_COLS);
	write_str(F("\e[2J\e[1;1H"));
	NEXT;
}	// }}}
void f_fetchVRAM_yx() {	// {{{ // ( y x -- c ) y row, x column, c character in VRAM
	INFO("VRAM_yx@");
	CELL_t x=pop();
	CELL_t y=pop();
	if( (x<MAX_COLS) && (y<MAX_ROWS)) {
		push(vram[y][x]);
	} else {
		push(0);
		ERROR("VRAM_yx@ out");
		write_hex16(x);
		write_char('x');
		write_hex16(y);
	};
	NEXT;
}	// }}}
void f_storeVRAM_yx() {	// {{{ // ( y x c -- ) y row, x column, c character in VRAM
	INFO("VRAM_yx!");
	CELL_t c=pop();
	CELL_t x=pop();
	CELL_t y=pop();
	if( (x<MAX_COLS) && (y<MAX_ROWS)) {
		vram[y][x]=c;
		write_str(F("\e["));
		write_num8(y+1);
		write_char(';');
		write_num8(x+1);
		write_char('H');
		write_char(c);
	} else {
		ERROR("VRAM_yx! out");
		write_hex16(x);
		write_char('x');
		write_hex16(y);
		write_char('=');
		write_hex8(c);
	};
	
	NEXT;
}	// }}}
void f_CUR_yx() {	// {{{ // ( y x -- ) move cursor to  y row, x column
	INFO("CUR_yx");
	CELL_t x=pop();
	CELL_t y=pop();
		write_str(F("\e["));
		write_num8(y+1);
		write_char(';');
		write_num8(x+1);
		write_char('H');
	NEXT;
}	// }}}
void f_KEYpress() {	// {{{ // ( -- c ) ascii of pressed key or 0
	INFO("KEYpress");
	push(read_char());
	NEXT;
}	// }}}
void f_WAIT() {	// {{{ // ( c -- ) bios.wait(c)
	INFO("WAIT");
	usleep(20000*pop());
	NEXT;
}	// }}}
#endif
#endif
void f_interpret(){	 // {{{
	TRACE("INTERPRET");
//	write_str(F("\r\n"));
	DEBUG_DUMP(B3U32(&Rstck[Rstack]),("Rstack	"));
	DEBUG_DUMP(B3U32(&stck[stack]),("stack	"));
	DEBUG_DUMP((HERE),("HERE	"));
	if (stack>1) DEBUG_DUMP(peek2(),("*stack	"));
	print_stack();
	if (STATE==st_executing) {
		write_str(F(PROMPT));
	} else {
		write_str(F(PROMPTcomp));
	};
	get_word();
	INFO(" got: ");info(&word_buf[0]);
	xpHead1 h=findHead(word_buf_len,&word_buf[0],B3PTR(LAST));
//	write_str(" head found? ");
//	DEBUG_DUMP((cmvp)h,"head?");
	if (h!=NULL) { // WORD
//		write_str("yes");
		if ((STATE==st_executing) || (h->flags & FLG_IMMEDIATE)) {
			DEBUG_DUMP(get_codeword_addr(h),"jump to	");
			DT=get_codeword_addr(h);
			INFO("jmp_indirect_24");
			jmp_indirect_24(get_codeword_addr(h));
			return; 	// NEXT is in called function
//			((void (*)(void))B4at(get_codeword_addr(h)))();
		} else {
			comma(get_codeword_addr(h));
		};
	} else {
		// Number ?
//		write_str("no");
		char *end;CELL_t c=strtoul(&word_buf[0],&end,BASE);
		if((word_buf_len-(end-&word_buf[0])) ==0) {
			// it is number (c)
			if (STATE==st_executing) {
				push(c);
			} else {
				comma(B3U32(&w_lit_cw));
//				comma((Data_t)(char *)c);	// README: overtype from 2B data - 2B pointer - 3B pointer works
				comma(c);	// README: overtype from 2B data - 2B pointer - 3B pointer works
			};
		} else {
			// it is not
			ERROR("What?");error(&word_buf[0]);
		}
	};
	INFO("end of f_interpret");
	NEXT;
	
	// XXX
}	// }}}
void f_debug(void) {	// {{{ // === f_debug: return from FOTH or what ===
	ERROR("f_debug");
	// f_next();	// No, simply no, return back to caler ...
}	// }}}
void f_doconst() {	// {{{
	TRACE("DOCONST");
	push(B4at(DT+4));
	NEXT;
}	// }}}
void f_doconst2() {	// {{{
	TRACE("DOCONST2");
	push2(B4at(DT+4));
	NEXT;
}	// }}}
void print_words(void) {	// {{{ // === print all wocabulary
	INFO("print_words");
	xpHead1 h=B3PTR(LAST);
	while (h) {
		if (h->flags & FLG_HIDDEN) write_str(F(CLR_GREY));
		if (h->flags & FLG_IMMEDIATE) write_str(F(BG_RED));
		for (uint8_t i = 0; i < h->len; ++i) write_char(h->name[i]);
		if (h->flags & FLG_IMMEDIATE) write_str(F(CLR_RESET));
		if (h->flags & FLG_HIDDEN) write_str(F(CLR_RESET));
		write_char(' ');
		h= h->next;
		};
	write_eoln();
}	// }}}
void f_words(void) {	// {{{ print all words
	TRACE("WORDS");
	print_words();
	NEXT;
}	// }}}
void f_dump() {	// {{{ ; ( Addr -- ) dump
	TRACE("dump");
	uint32_t addr=pop2();
	bool d=nodebug;
	nodebug=false;
	write_eoln();
	debug_dump(addr,F("dump	"));
	nodebug=d;
	NEXT;
}	// }}}
void f_nodebug() {	// {{{ ; nodebug
	TRACE("nodebug");
	nodebug=(0!=pop());
	NEXT;
}	// }}}
void f_noinfo() {	// {{{ ; noinfo
	TRACE("noinfo");
	noinfo=(0!=pop());
	NEXT;
}	// }}}
void f_notrace() {	// {{{ ; notrace
	TRACE("notrace");
	notrace=(0!=pop());
	NEXT;
}	// }}}
// {{{ more primitives
void f_create(void) {	// {{{ create header of new word
	INFO("CREATE");
	uint32_t temp_h=HERE;
	*(uint32_t*)B3PTR(HERE)=LAST; HERE+=4;		// 4B next ptr
	*(uint8_t*)B3PTR(HERE) =0;HERE++;			// 1B attr
	uint8_t len=pop();
	*(uint8_t*)B3PTR(HERE) =len;HERE++;				// 1B len "words"
//	strncpy_PF((char*)B3PTR(HERE),B3PTR(pop2()),len); HERE+=len;// len Bytes (+\0, but we overwrite it next step)
	DEBUG_DUMP(peek2(),"from buff	");
	DEBUG_DUMP(HERE,"HERE	");
	uint32_t from=pop2();
	// strncpy_PF((char*)B3PTR(HERE),pop2(),len); HERE+=len;// len Bytes (+\0, but we overwrite it next step)
	for (uint8_t i=0; i<len;i++){
		*(uint8_t*)B3PTR(HERE++) =*(uint8_t*)B3PTR(from++);
	};
	DEBUG_DUMP(HERE,"HERE	");
	LAST=temp_h;
	NEXT;
}	// }}}
void f_right_brac() {	// {{{ ; ] goes to compile mode
	TRACE("]");
	STATE=st_compiling;
	NEXT;
}	// }}}
void f_left_brac() {	// {{{ [ goes to immediate mode
	TRACE("[");
	STATE=st_executing;
	NEXT;
}	// }}}
void f_hidden() {	// {{{ ; Addr_of_header HIDDEN hide/unhide the word
	TRACE("HIDDEN");
	uint32_t addr=pop2() + 4;	// flags
	*(uint8_t *)B3PTR(addr) = B1at(addr) ^ FLG_HIDDEN;
	NEXT;
}	// }}}
void f_find() {	// {{{ ; WORD FIND return Addr_of_header (or 0 0 )
	TRACE("FIND");
	uint8_t len=pop();
	uint32_t addr=pop2();
	xpHead1 h=findHead(len,B3PTR(addr),B3PTR(LAST));
	push2(B3U32(h));
	NEXT;
}	// }}}
// }}}
const __flash char f_words_name[]="WORDS2";
void my_setup(){	// {{{
	notrace=false;
	noinfo=false;
	nodebug=false;
	notrace=true;
	noinfo=true;
	nodebug=true;
	HERE=B3U32(&RAM[0]);
	RAM[0]='>';
	RAM[1]='>';
	RAM[2]='>';
	LAST=B3U32(&top_head);
	ERROR("Test");
	uint32_t temp_h=HERE;
	*(uint32_t*)B3PTR(HERE)=LAST; HERE+=4;		// 3B next ptr
	*(uint8_t*)B3PTR(HERE) =0;HERE++;			// 1B attr
	uint8_t len=strlen_P(f_words_name);
	*(uint8_t*)B3PTR(HERE) =len;HERE++;				// 1B len "words"
	strcpy_P((char*)B3PTR(HERE),f_words_name); HERE+=len;// len Bytes (+\0, but we overwrite it next step)
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
	uint16_t cw=(uint16_t)&f_words;
	*(uint32_t*)B3PTR(HERE)=cw * 2; HERE+=4;	// codeword
#elif defined(__PC__)
	uint32_t cw=(uint32_t)&f_words;
	*(uint32_t*)B3PTR(HERE)=cw; HERE+=4;	// codeword
#else
#error undefined
#endif
	LAST=temp_h;
// --------------------------------------------------------------------------------
	DEBUG_DUMP(B3U32(&RAM[0]),"RAM	");
	DEBUG_DUMPp(&HERE,"HERE	");
	DEBUG_DUMP(HERE,"*HERE	");
	DEBUG_DUMPp(&LAST,"LAST	");
	DEBUG_DUMP(LAST,"*LAST	");
// --------------------------------------------------------------------------------
	ERROR("my_setup");
	IP = B3U32(&w_test_data);
	DEBUG_DUMP(IP,"IP\t");
	DEBUG_DUMP(val_of_f_docol,"val_of_f_docol");
	DEBUG_DUMP(val_of_w_exit_cw,"val_of_w_exit_cw");
	push(0x21);
	print_words();
//	NEXT;
	forth_loop(B3U32(&w_test_cw));
	write_hex16(pop());
// --------------------------------------------------------------------------------
	ERROR("Full run");
	IP = B3U32(&w_quit_data);
	DEBUG_DUMP(IP,"IP\t");
	DEBUG_DUMP(B3U32(&f_docol),"&f_docol");
	Rpush(IP);
	IP=B3U32(&f_docol);
	DEBUG_DUMP(IP,"B3U32(&f_docol)");
	IP=Rpop();
	
	forth_loop(B3U32(&w_quit_cw));
//	NEXT;
// --------------------------------------------------------------------------------
	ERROR("the end");
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
	while(1){;};
#elif defined(__PC__)
#else
#error undefined
#endif

};	// }}}
void my_loop(){	// {{{
};// }}}
