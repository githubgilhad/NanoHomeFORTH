/* vim: set noexpandtab fileencoding=utf-8 nomodified wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: tabstop=8 linebreak showbreak=»\  ft=cpp */
// ,,g = gcc, exactly one space after "set"

#include <Arduino.h>
#include "version.h"
#include "bios.h"
#include "MemoryFree.h"
#include "defines.h"
extern uint8_t  __data_load_end;
extern uint8_t  __DATA_REGION_LENGTH__;
//	 µFORTH
extern "C" {
	extern void my_setup();
	extern void my_loop();
	char read_char() {
		uint16_t c=bios.get_key();
		if (c== xF12) { if(bios.current_output==BIOS_VGA) { bios.set_output(BIOS_RCA); } else { bios.set_output(BIOS_VGA);} ; return 0; };
		return c;
	}

	void write_char(char c) {
		bios << c;
	}
	void write_charA(char c) {
		bios << (c < ' ' ? '.' : c);
	}
}
void setup(){
	bios.set_output(BIOS_RCA);              // Inicializace BIOS
	bios.set_output(BIOS_VGA);              // Inicializace BIOS
	bios.clear(' ',VGA_WHITE);
	bios.inverting=true;
	bios << YX(1,2) << COLOR(VGA_YELLOW) << F(" **** Nano Home Computer 2.7 **** ");
	bios << YX(2,2) << COLOR(VGA_GREEN) << F("Free: RAM: ") << freeMemory() << F(" B; FLASH: ") << uint32_t(HEX_MAXIMUM_SIZE - (uintptr_t)&__data_load_end) << F(" B");
	bios << YX(3,2) << COLOR(VGA_CYAN) << F("Used: FLASH ") <<  (uintptr_t)&__data_load_end << F(" / ") << uint32_t(HEX_MAXIMUM_SIZE) << F(" B");
	bios << YX(4,2) << F("Screen size: ") <<COLOR(VGA_GREEN) << ' ' << BIOS_COLS << 'x' << BIOS_ROWS;
	bios << YX(5,2) << F("Compiled: " _DATE_ " " __TIME__ " \r\n");
	bios <<(F(VERSION_STRING ));
	bios <<(F("  based on " VERSION_COMMIT " - " VERSION_MESSAGE  "\r\n"));
	bios <<(F("---- ==== #### FORTH #### ==== ---- \r\n"));
	bios <<(F("Hint: 0 nodebug 0 noinfo 0 notrace LAST D@ 20 + dump  \r\n"));
#if defined(__AVR_ATmega2560__)
	bios <<(F("Hint: hex ff DDRF !C aa PORTF !C ff DDRK !C aa PORTK !C : x ff  PINF !C ff  PINK !C ; x  \r\n"));
	bios <<(F(": count- 0 BEGIN DUP c2C PORTF !C PORTK !C 1- DUP ==0 UNTIL c2C PORTF !C PORTK !C ; \r\n"));
	bios <<(F(": count+ 0 BEGIN DUP c2C PORTF !C PORTK !C 1 + DUP ==0 UNTIL c2C PORTF !C PORTK !C ; \r\n"));
#endif
	bios <<(F("Test: : xx 0BRANCH [ 0 3 , ] 5 ; : xxx IF 1111 ELSE 2222 FI 3333 + ; \r\n")); 
	bios << F(
	STR_2LESS "<" // '«'
	STR_2MORE ">" // '»'
	STR_UP "^" // '^'
	STR_DOWN "v" // 'v'
	STR_LEFT "<-" // '«-'
	STR_RIGHT "->" // '-»'
	"\r\n"
	);
	while (!bios.get_key()){;};

	my_setup();
	bios <<(F("Setup done"));
}

void loop(){
	my_loop();
}
