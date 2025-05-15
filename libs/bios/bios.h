/* vim: set noexpandtab fileencoding=utf-8 nomodified nowrap textwidth=270 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: */
// ,g = gcc, exactly one space after "set"
//
#ifndef BIOS_H
#define BIOS_H

#define BIOS_ROWS		25					// number of rows of VGA text output
#define BIOS_COLS		37					// number of columns of VGA text output (more than 37 means noise from PS/2 input)
#define RCA_PIXELS_PER_CHARACTER 9

// time into scanline when pixel pumper awakes (but it takes time to get going because of the interrupt handler etc.
// Units: us * 16
#define RCA_LEFT_EDGE 116
// scan line at which pixel pumper is enabled. 30 minimum to be on-screen on 7-inch monitor
// #define TOP_EDGE 32
// #define RCA_TOP_EDGE 40
// Now, with less rows to meet VGA we can center vertically this :)
#define RCA_TOP_EDGE 56
// and when it's stopped. 303 maximum to be on-screen on 7-inch monitor
#define RCA_BOTTOM_EDGE (RCA_TOP_EDGE + BIOS_ROWS * RCA_PIXELS_PER_CHARACTER - 1)

#ifdef __cplusplus

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <WString.h>
#include "ScanToASCII.h"
#include "buffer.h"
#include "charset.h"
// #include "VGA.h"
//#include "bios_test.h"

enum BIOS_Color {
	VGA_BLACK 	 = 0b0000,
	VGA_DKGRAY 	 = 0b0001,
	VGA_DKRED 	 = 0b0010,
	VGA_DKGREEN 	 = 0b0100,
	VGA_DKYELLOW 	 = 0b0110,
	VGA_DKBLUE 	 = 0b1000,
	VGA_DKMAGENTA 	 = 0b1010,
	VGA_DKCYAN 	 = 0b1100,
	VGA_RED 	 = 0b0011,
	VGA_GREEN 	 = 0b0101,
	VGA_YELLOW 	 = 0b0111,
	VGA_BLUE 	 = 0b1001,
	VGA_MAGENTA 	 = 0b1011,
	VGA_CYAN 	 = 0b1101,
	VGA_GRAY 	 = 0b1110,
	VGA_WHITE 	 = 0b1111,
	VGA_none 	 = 0xFF,
	};

enum BIOS_output {
	BIOS_none,
	BIOS_VGA,
	BIOS_RCA
	};

class XY {	// {{{ up to 37x25
	public:
		uint8_t x, y;
		XY(uint8_t x, uint8_t y) : x(x), y(y) {};
};	// }}}
class YX {	// {{{ up to 37x25
	public:
		uint8_t x, y;
		YX(uint8_t y, uint8_t x) : x(x), y(y) {};
		void setYX(uint8_t y, uint8_t x) { this->x=x; this->y=y;};
};	// }}}
class COLOR {	// {{{ up to 37x25
	public:
		BIOS_Color col;
		COLOR(BIOS_Color col) : col(col) {};
};	// }}}

enum PS2Status {
	none 	 = 0,
	all 	 = 0xFF,
	SHIFT_BIT 	 = 0b00001,
	CTRL_BIT 	 = 0b00010,
	ALT_BIT 	 = 0b00100,
	CAPSLOCK_BIT 	 = 0b01000,
	NUMLOCK_BIT 	 = 0b10000,
};
extern const uint8_t charset[9][256] PROGMEM;					// VGA just 8 top lines, RCA all 9, MUST BE 256 B ALIGNED
class BIOS {
public:

	static volatile uint16_t frames ;												// counting the displayed frames (60Hz)
	static volatile int vline asm("vline");			// current vertical position of pixel video output
	static /* const */ char hexa_digits[16];				// 
	public:
		static YX cursor;			//  {0,0};
	public:
		BIOS();
		~BIOS();
		BIOS& operator<<(char c);
		BIOS& operator<<(const char* str);
		BIOS& operator<<(const __FlashStringHelper* str);
		BIOS& operator<<(uint16_t  num);
		BIOS& operator<<(uint32_t  num);
		BIOS& operator<<(int  num);
		BIOS& operator<<(XY pos);
		BIOS& operator<<(YX pos);
		BIOS& operator<<(COLOR col);

		static void set_output(BIOS_output output=BIOS_VGA);
		static bool inverting; // = false;
		static void invert();						// inverts character at actual cursor
		// print interprets \r\n\t chars, write doesnot
		static void printNum(int32_t i, uint8_t size=0, char fill=' ');
		static void printHex(uint32_t i, uint8_t size=0, char fill=' ');
		static void printBin(uint32_t i, uint8_t size=0, char fill=' ');
		//
		static void write(char c);
		static void write(char *c);
		static void write(const char *c);
		//
		static void scroll();	// cursor not scrolled
		static void clear(char c = ' ', BIOS_Color col = VGA_WHITE);	// cursor to 0,0
		//
		static void set_cursor(uint8_t row, uint8_t col);
		//
		static void set_rowcolor(uint8_t row, BIOS_Color color);
		static void set_color(BIOS_Color color);	// current row
		//
		static uint16_t get_key();
		static uint8_t get_scancode();
		static void wait(unsigned int dt);		// wait(0) does not wait at all
	public:
		// rather do not use
		static PS2Status ps2status;		// none;
		static const uint16_t ScanToASCII[3][128] PROGMEM;				// [x][..]: SHIFT/ALTGR keystate and PS/2 scancode, [..][y]: ASCII code)
		static const uint8_t (*chardef)[9][256] asm("chardef");					// pointer to actual charset ( default: chardef=&charset;)
		static uint8_t vram[BIOS_ROWS][BIOS_COLS] asm("vram"); 		// array of character video RAM data
		static uint8_t cram[BIOS_ROWS] asm("cram"); 				// array of color video RAM data
		static void VGA_begin();
		static void VGA_end();
		static void RCA_begin();
		static void RCA_end();
		static BIOS_output current_output;	// BIOS_none

}; 
extern BIOS bios;
// extern int main(void);
#else
	// assembler here
#endif
#endif
