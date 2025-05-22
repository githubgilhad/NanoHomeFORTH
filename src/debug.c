/* vim: set ft=cpp noexpandtab fileencoding=utf-8 nomodified wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: tabstop=8 linebreak showbreak=»\   */
// ,,g = gcc, exactly one space after "set"
#include "debug.h"
bool nodebug=true;
bool noinfo=true;
bool notrace=true;
//bool nodebug=false;
//
extern void write_char(char c);
extern void write_charA(char c);
//

void error(const __memx char *c) { 	// {{{
	write_str(F(BG_RED CLR_BLUE STYLE_BOLD STR_2LESS )); //0xC2 0xAB);//'«'
	while (*c){write_char(*c++);};
	write_str(F(STR_2MORE CLR_RESET "\r\n"));// 0xC2 0xBB);//'»'
}	// }}}
void info(const __memx char *c) { 	// {{{
	if (noinfo) return;
	write_str(F(BG_BLUE STYLE_BOLD STR_2LESS )); //0xC2 0xAB);//'«'
	while (*c){write_char(*c++);};
	write_str(F(STR_2MORE CLR_RESET ));// 0xC2 0xBB);//'»'
//	write_str(F( "\r\n"));
}	// }}}
void trace(const __memx char *c) { 	// {{{
	if (notrace) return;
	write_str(F(BG_GREEN STYLE_BOLD STR_2LESS )); //0xC2 0xAB);//'«'
	while (*c){write_char(*c++);};
	write_str(F(STR_2MORE CLR_RESET ));// 0xC2 0xBB);//'»'
//	write_str(F( "\r\n"));
}	// }}}

// {{{ write_hexXX
const __memx char hex[] = "0123456789ABCDEF";
void write_hex8(uint8_t b) {
	write_char(hex[(b >> 4) & 0xF]);
	write_char(hex[b & 0xF]);
}

void write_hex16(uint16_t w) {
	write_hex8(w >> 8);
	write_hex8(w & 0xFF);
}

void write_hex24(uint32_t w) {
	write_hex8((w >> 16) & 0xFF);
	write_hex8((w >> 8) & 0xFF);
	write_hex8(w & 0xFF);
}
void write_hex32(uint32_t w) {
	write_hex8((w >> 24) & 0xFF);
	write_hex8((w >> 16) & 0xFF);
	write_hex8((w >> 8) & 0xFF);
	write_hex8(w & 0xFF);
}
// }}}
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define RAM_START     ((uint32_t)0x800100)
#define RAM_END       ((uint32_t)0x810000)
#define FLASH_START   ((uint32_t)0x000000)
#define FLASH_END     ((uint32_t)0x040000)

bool is_in_range(uint32_t addr) {	 // {{{
	return	((addr >= RAM_START)	&& (addr < RAM_END)) ||
		((addr >= FLASH_START)	&& (addr < FLASH_END));
}	// }}}

#elif defined(__PC__)
 #include <stdio.h>

bool is_in_range(uintptr_t addr /* void *addr*/) {
    FILE *fp = fopen("/proc/self/maps", "r");
    if (!fp) return false;

    uintptr_t target = (uintptr_t)addr;
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        long unsigned int start, end;
        char perms[5];

        if (sscanf(line, "%lx-%lx %4s", &start, &end, perms) == 3) {
            if ((target >= start && target < end) && perms[0] == 'r') {
                fclose(fp);
                return true;
            }
        }
    }

    fclose(fp);
    return false;
}
#endif
static inline uint8_t read_memx(uint32_t addr) {	 // {{{
	ptr24_u x; x.u32=addr;
	return *(volatile const __memx uint8_t*)x.p24;
}	// }}}
//void debug_dump(const __memx void * address, const __memx char* label) {	 
void debug_dump(uint32_t address, const __memx char* label) {	 // {{{
	if (nodebug) return;
//	uint32_t addr = p24u32(address);
	uint32_t addr = address;
	uint32_t base = addr & ~0x0F;
	uint32_t start = (base >= 16) ? base - 16 : base;

	write_str(F("{" CLR_YELLOW));
	write_str(label);
	write_str(F(CLR_RESET "\t [" CLR_GREEN));
	write_hex32(addr);
	write_str(F(CLR_RESET "] -> "));

	for (uint8_t i = 0; i < 3*16; ++i) {
		if( i==16 || i==32 ) write_str(F(" | "));
		uint32_t curr = start + i;
		if (curr == addr) write_str(F(BG_GREEN STR_2LESS));
		if (is_in_range(curr) ){ write_hex8(read_memx(curr));
		} else { write_str(F("  ")); };
		if (curr == addr) {
			write_str(F(STR_2MORE CLR_RESET));
		} else if (curr+1 != addr) {
			write_char(' ');
		};
	}

	write_str(F(" || '"));

	// ASCII
	char c;
	for (uint8_t i = 0; i < 3*16; ++i) {
		if( i==16 || i==32 ) write_str(F("' | '"));
		uint32_t curr = start + i;
		if (is_in_range(curr) ){ c = read_memx(curr);
		} else { c = ' '; };

		if (curr == addr) {
			write_str(F(BG_GREEN STR_2LESS));
			write_charA(c);
			write_str(F(STR_2MORE CLR_RESET));
		} else {
			write_charA(c);
		}
	}
	write_str(F("' }\r\n"));
}	// }}}
