#ifndef PTR24_H
#define PTR24_H
#define CM const __memx 
typedef const __memx void *cmvp;
typedef struct b3_t {	 // {{{ // === 24-bit pointer type ===
	uint8_t lo;
	uint8_t hi;
	uint8_t bank;
} b3_t; // }}}
typedef struct b4_t {	 // {{{ // === 32-bit pointer type ===
	uint8_t lo;
	uint8_t hi;
	uint8_t bank;
	uint8_t zero;
} b4_t; // }}}
typedef union  ptr24_u {	// {{{ b3 x uint32_t x const __memx void* union
	const __memx union ptr24_u* p24;
	b3_t b3;
	b4_t b4;
	uint32_t u32;
} ptr24_u; // }}}
static inline ptr24_u V(ptr24_u p){p.b4.zero=0;return p;};	// Verify, Value, sanitise
static inline ptr24_u P(ptr24_u p){	// {{{ Pointer (dereference a return 3Bytes from there)
	p.b4.zero=0;
	ptr24_u r = {.b3=*(CM b3_t *)p.p24};
	r.b4.zero=0;
	return r;
}	// }}}
#define F(x) PSTR(x)
static inline ptr24_u P24u(uint32_t u) { ptr24_u r; r.u32=u; return r;}
static inline ptr24_u P24p(CM void * p) { ptr24_u r; r.p24=(CM ptr24_u *)p;r.b4.zero=0; return r;}
static inline uint32_t p24u32(cmvp p) {ptr24_u r; r.p24=p; r.b4.zero=0;return r.u32;}
static inline cmvp u32p24(uint32_t u) {ptr24_u r; r.u32=u; r.b4.zero=0;return r.p24;}
#endif
