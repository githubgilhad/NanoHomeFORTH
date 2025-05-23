/* vim: set ft=cpp noexpandtab fileencoding=utf-8 nomodified wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: tabstop=8 linebreak showbreak=»\   */
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
#else
#error undefined
#endif

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
typedef uint32_t DOUBLE_t;	// 2 cell on data stack 4B
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
		return;
		};
	stck[stack++]=x;
}
CELL_t peek(){
	if (stack<1) {
		ERROR("peek - No Stack left");
		return 0;
		};
	if (! noinfo) write_hex16(stck[stack-1]);
	INFO("peek");
	return stck[stack-1];
}
CELL_t peekX(uint8_t depth){
	if (stack<1+depth) {
		ERROR("peek - No Stack left");
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
		return 0;
		};
	if (! noinfo) write_hex32(stck[stack-2]*(1L<<16)+stck[stack-1]);
	INFO("peek2");
	return stck[stack-2]*(1L<<16)+stck[stack-1];
}
DOUBLE_t peek2X(uint8_t depth){
	if (stack<2+depth) {
		ERROR("peek2 - No Stack left");
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
		return;
		};
	Rstck[Rstack++]=x;
}
PTR_t Rpeek(){
	if (Rstack==0) {
		ERROR("Rpeek - No Stack left");
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
void f_or(){	// {{{
	TRACE("OR");
	push((pop()||pop())?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_and(){	// {{{
	TRACE("AND");
	push((pop()&&pop())?F_TRUE:F_FALSE);
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
void f_Store(){	// {{{ ! ( cell Daddr --  ) store cell at address(Double)
	TRACE("!");
	DOUBLE_t d=pop2();
	*(CELL_t*)B3PTR(d)=pop();
	NEXT;
}	// }}}
void f_StoreChar(){	// {{{ !C ( char Daddr -- ) store char at address(Double)
	TRACE("!C");
	DOUBLE_t d=pop2();
	uint8_t v=pop();
	*(uint8_t*)B3PTR(d)=v;
	NEXT;
}	// }}}
void f_StoreDouble(){	// {{{ !D ( D Daddr -- ) store Double at address(Double)
	TRACE("!D");
	DOUBLE_t d=pop2();
	*(uint32_t*)B3PTR(d)=pop2();
	NEXT;
}	// }}}
void f_At(){	// {{{ @ ( Daddr -- cell ) cell at address(Double)
	TRACE("@");
	push(B2at(pop2()));
	NEXT;
}	// }}}
void f_CharAt(){	// {{{ C@ ( Daddr -- char ) char at address(Double)
	TRACE("C@");
	push(B2at(pop2())&0xFF);
	NEXT;
}	// }}}
void f_DoubleAt(){	// {{{ D@ ( Daddr -- D ) Double at address(Double)
	TRACE("D@");
	push2(B4at(pop2()));
	NEXT;
}	// }}}
void f_ToR() {	// {{{ // ( u -- ; R: -- r ) Move to Rstack
	INFO("f_ToR");
	Rpush(pop());
	NEXT;
}	// }}}
void f_DoubleToR() {	// {{{ // ( D -- ; R: -- r ) Move Double to Rstack (still one position on R)
	INFO("f_DoubleToR");
	Rpush(pop2());
	NEXT;
}	// }}}
void f_FromR() {	// {{{ // ( -- u ; R: r -- ) Move from Rstack
	INFO("f_FromR");
	push(Rpop());
	NEXT;
}	// }}}
void f_DoubleFromR() {	// {{{ // ( -- D ; R: r -- ) Move Double from Rstack (still one position on R)
	INFO("f_DoubleFromR");
	push2(Rpop());
	NEXT;
}	// }}}
void f_CellAtR() {	// {{{ // ( -- u ; R: r -- r ) Peek from Rstack
	INFO("f_CellAtR");
	push(Rpeek());
	NEXT;
}	// }}}
void f_DoubleAtR() {	// {{{ // ( -- D ; R: r -- r ) Peek from Rstack
	INFO("f_DoubleAtR");
	push2(Rpeek());
	NEXT;
}	// }}}
void f_StackAddress() {	// {{{ // ( -- D ) Address, where Stack points
	INFO("f_StackAddress");
	push2(B3U32(&stck[stack]));
	NEXT;
}	// }}}
void f_SetStack() {	// {{{ // ( D -- ?? ) Set Stack Address
	INFO("f_SetStack");
	int32_t S=pop2()-B3U32(&stck[0]);
	if ((S<0) || (S>STACK_LEN*sizeof(stck[0]))) { ERROR("Out of stack area"); NEXT; return;};
	int32_t s=S/sizeof(stck[0]);
	if (s*sizeof(stck[0]) != S) { ERROR("Bad alligned stack"); NEXT; return;};
	stack=s;
	NEXT;
}	// }}}
void f_RStackAddress() {	// {{{ // ( -- D ) Address, where Rstack points
	INFO("f_RStackAddress");
	push2(B3U32(&Rstck[Rstack]));
	NEXT;
}	// }}}
void f_SetRStack() {	// {{{ // ( D -- R: ?? ) Set Rstack Address
	INFO("f_SetRStack");
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
	TRACE("cw2h");
	if (!cw) return 0;
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
uint8_t show_name(DOUBLE_t cw) {	// {{{ show name and address from codeword - return flags
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
		write_hex24(B3at(cw));
		write_str(F(" @ "));write_hex24(h);
		write_str(F(" (prev: ")); write_hex24(B3at(h));
		write_str(F(") "));
		};
	return flags;
}	// }}}
void show(DOUBLE_t cw) {	// {{{ ; ' WORD show - try to show definition of WORD
	TRACE("show");
	DOUBLE_t h=cw2h(cw);
	DOUBLE_t val;
	uint8_t flags;
	if (!h) {ERROR("Not a word");return;};
	show_name(cw);
	write_eoln();
	if (val_of_f_docol != B3at(cw)) return;	// neumim rozepsat
	do {
		cw+=4;
		val=B4at(cw);
		write_str(F("\t"));
//		write_str(F("\t["));
//		write_hex32(val);
//		write_str(F("] "));
		flags=show_name(val);
		if (flags & FLG_ARG) {
			cw+=4;
			val=B4at(cw);
			write_str(F("\r\n\t\t["));
			write_hex32(val);
			write_str(F("]"));
			val=0;
		};
		write_eoln();
	} while (val != val_of_w_exit_cw);

}	// }}}
void f_show() {	// {{{ ; ' WORD show - try to show definition of WORD
	TRACE("show");
	DOUBLE_t cw=pop2();
	show(cw);
	NEXT;
}	// }}}
uint8_t export_name(DOUBLE_t cw) {	// {{{ export name and address from codeword - return flags
	DOUBLE_t h=cw2h(cw);
	if (!h) {ERROR("Not a word");return 0;};
	uint8_t flags,len;
	flags=B1at(h+4);
	len=B1at(h+5);
//	if (flags & FLG_HIDDEN) write_str(F("HIDDEN "));
//	if (flags & FLG_IMMEDIATE) write_str(F("IMMEDIATE "));
//	if (flags & FLG_ARG) write_str(F("ARG "));
//	write_str(F("len: "));write_hex8(len);write_str(F(", name: "));
//	write_str(F(STR_2LESS));
	for (uint8_t i=0; i<len;i++) write_char(B1at(h+6+i));
//	write_str(F(STR_2MORE));
	return flags;
}	// }}}
void do_export(DOUBLE_t cw) {	// {{{ ; ' WORD export - try to export definition of WORD
	TRACE("export");
	DOUBLE_t h=cw2h(cw);
	DOUBLE_t val;
	uint8_t flags;
	if (!h) {ERROR("Not a word");return;};
	write_eoln();
	write_str(F(": "));
	flags = export_name(cw);
	if (flags & FLG_IMMEDIATE) write_str(F(" IMMEDIATE"));
	if (val_of_f_docol != B3at(cw)) {
		write_str(F(" NOT_DOCOL definition "));
		return;	// neumim rozepsat
		};
	do {
		cw+=4;
		val=B4at(cw);
		if (val == val_of_w_exit_cw) break;
		write_char(' ');
		flags=export_name(val);
		if (flags & FLG_ARG) {
			cw+=4;
			val=B4at(cw);
			write_str(F(" \\'0x"));
			write_hex32(val);
			val=0;
		};
	} while (val != val_of_w_exit_cw);
	write_str(F(" ;"));
	write_eoln();

}	// }}}
void f_export() {	// {{{ ; ' WORD export - try to export definition of WORD
	TRACE("export");
	DOUBLE_t cw=pop2();
	do_export(cw);
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
	INFO("f_docol");
//	track("f_docol ");
// ERROR("Press ANY key to continue");wait_for_char();
	Rpush(IP);
	IP=DT+4;	// README: DT points to 4B codeword, so next address is DT+4B and now it is on Data[0] in the target header
	DEBUG_DUMP(IP,"IP in f_docol	");
	DEBUG_DUMP(DT,"DT in f_docol	");
	NEXT;
}	// }}}
void f_exit(){	// {{{
	INFO("f_exit");
	IP=Rpop();
	NEXT;
}	// }}}
void f_lit(){	// {{{ README: LIT takes the next 4B pointer as 2B integer, ignores the top byte. This is done for taking the same 4B alingment in data
	INFO("f_lit");
	push(B2at(IP));
	IP+=4;
	NEXT;
}	// }}}
void f_lit2(){	// {{{ README: LIT takes the next 4B pointer as 4B integer. This is done for taking the same 4B alingment in data
	INFO("f_lit2");
	push2(B4at(IP));
	IP+=4;
	NEXT;
}	// }}}
void comma(uint32_t d) {	// {{{
	INFO("comma");
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
	char buf[32];	// stack eating structures cannot be in NEXT-chained functions, or stack will overflow !!!
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
	if (STATE == st_executing) { // st_executing
	INFO("st_executing");
		get_word();
		xpHead1 h=findHead(word_buf_len,&word_buf[0],B3PTR(LAST));
//		write_hex32(B3U32(h));
		push2(get_codeword_addr(h));
	} else {
	INFO("st_compiling");
		int32_t c=B4at(IP);
		IP+=4;
		push2(c);
	};
	NEXT;
}	// }}}
void f_immediate() {	// {{{ ; Addr_of_header IMMEDIATE make the word immediate
	TRACE("IMMEDIATE");
	uint32_t h=pop2();
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
	push((pop()>0)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_positive0() {	// {{{ ; true if positive or zero
	TRACE(">=0");
	push((pop()>=0)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_negative() {	// {{{ ; true if negative
	TRACE("<0");
	push((pop()<0)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_negative0() {	// {{{ ; true if negative or zero
	TRACE("<=0");
	push((pop()<=0)?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_notequal() {	// {{{ ; (c1 c2 -- flag ) true if notequal
	INFO("f_notequal");
	push((pop()==pop())?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_notequalD() {	// {{{ ; (d1 d2 -- flag ) true if notequal
	INFO("f_notequalD");
	push((pop2()==pop2())?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_equal() {	// {{{ ; (c1 c2 -- flag ) true if equal
	INFO("f_equal");
	push((pop()==pop())?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_equalD() {	// {{{ ; (d1 d2 -- flag ) true if equal
	INFO("f_equalD");
	push((pop2()==pop2())?F_TRUE:F_FALSE);
	NEXT;
}	// }}}
void f_interpret(){	 // {{{
	INFO("f_interpret");
//	write_str(F("\r\n"));
	DEBUG_DUMP(B3U32(&Rstck[Rstack]),("Rstack	"));
	DEBUG_DUMP(B3U32(&stck[stack]),("stack	"));
	DEBUG_DUMP((HERE),("HERE	"));
	if (stack>1) DEBUG_DUMP(peek2(),("*stack	"));
	for (int8_t p=0;p<stack;p++) {write_char('[');write_hex16(stck[p]);write_char(']');};
	write_str(F(PROMPT));
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
	INFO("f_doconst");
	push(B4at(DT+4));
	NEXT;
}	// }}}
void f_doconst2() {	// {{{
	INFO("f_doconst2");
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
	ERROR("f_create");
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
