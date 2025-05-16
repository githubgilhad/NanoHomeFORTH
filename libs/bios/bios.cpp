#include <string.h>
#include "bios.h"
#include <Arduino.h>

//  {{{ divmod10_asm
//
// http://forum.arduino.cc/index.php?topic=167414.msg1293679#msg1293679
// http://forum.arduino.cc/index.php?topic=167414.msg1309482#msg1309482
//  equivelant code:
//    mod8 = in32 % 10;
//    in32 = in32 / 10;
//    tmp8 = 10;
#define divmod10_asm(in32, mod8, tmp8)		\
asm volatile (					\
      " ldi %2,51     \n\t"			\
      " mul %A0,%2    \n\t"			\
      " clr %A0       \n\t"			\
      " add r0,%2     \n\t"			\
      " adc %A0,r1    \n\t"			\
      " mov %1,r0     \n\t"			\
      " mul %B0,%2    \n\t"			\
      " clr %B0       \n\t"			\
      " add %A0,r0    \n\t"			\
      " adc %B0,r1    \n\t"			\
      " mul %C0,%2    \n\t"			\
      " clr %C0       \n\t"			\
      " add %B0,r0    \n\t"			\
      " adc %C0,r1    \n\t"			\
      " mul %D0,%2    \n\t"			\
      " clr %D0       \n\t"			\
      " add %C0,r0    \n\t"			\
      " adc %D0,r1    \n\t"			\
      " clr r1        \n\t"  			\
      " add %1,%A0    \n\t"			\
      " adc %A0,%B0   \n\t"			\
      " adc %B0,%C0   \n\t"			\
      " adc %C0,%D0   \n\t"			\
      " adc %D0,r1    \n\t"			\
      " add %1,%B0    \n\t"			\
      " adc %A0,%C0   \n\t"			\
      " adc %B0,%D0   \n\t"			\
      " adc %C0,r1    \n\t"			\
      " adc %D0,r1    \n\t"			\
      " add %1,%D0    \n\t"			\
      " adc %A0,r1    \n\t"			\
      " adc %B0,r1    \n\t"			\
      " adc %C0,r1    \n\t"			\
      " adc %D0,r1    \n\t"			\
      " lsr %D0       \n\t"			\
      " ror %C0       \n\t"			\
      " ror %B0       \n\t"			\
      " ror %A0       \n\t"			\
      " ror %1        \n\t"   			\
      " ldi %2,10     \n\t"			\
      " mul %1,%2     \n\t"			\
      " mov %1,r1     \n\t"			\
      " clr r1        \n\t"			\
      :"+r"(in32),"=d"(mod8),"=d"(tmp8) : : "r0")

size_t Print__printNumberDec(unsigned long n, uint8_t sign)
{
	uint8_t digit, buf[11], *p;
	uint8_t tmp8;
	p = buf + (sizeof(buf));
	do {
		divmod10_asm(n, digit, tmp8);
		*--p = digit + '0';
	} while (n);
	if (sign) *--p = '-';
//	return write(p, sizeof(buf) - (p - buf));
	return 0; // fake
}
// }}}
// Initialize static members
BIOS_output BIOS::current_output = BIOS_output::BIOS_none;
YX BIOS::cursor = {0, 0};
PS2Status BIOS::ps2status = PS2Status::none;
extern const uint8_t charset[9][256];
const uint8_t (*BIOS::chardef)[9][256] = &charset;						// pointer to actual charset ( default: chardef=&charset;)
uint8_t BIOS::vram[BIOS_ROWS][BIOS_COLS] asm("vram"); 						// array of character video RAM data
uint8_t BIOS::cram[BIOS_ROWS] asm("cram"); 							// array of color video RAM data
bool BIOS::inverting = false;									// inverting character on cursor automatically?
/* const */ char BIOS::hexa_digits[16]={'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

BIOS::BIOS() {											// {{{
	charset_hook();
	BIOS::chardef=&charset;
	BIOS::current_output = BIOS_output::BIOS_none;
	BIOS::set_output(BIOS_VGA);
	BIOS::clear();
	clear();
}	// }}}

BIOS::~BIOS() {											// {{{
	BIOS::set_output(BIOS_none);
}	// }}}
BIOS& BIOS::operator<< (char c) {	// {{{
	invert();
	switch (c) {
	case '\t':
		cursor.x = (cursor.x+8) & 0xF8;
		if (cursor.x >= BIOS_COLS) {
			cursor.x=0;
			if (++cursor.y >= BIOS_ROWS) scroll();
		};
		break;
	case '\n':
		if (++cursor.y >= BIOS_ROWS) scroll();
		break;
	case '\r':
		cursor.x=0;
		break;
	default:
		vram[cursor.y][cursor.x++]=c;
		if (cursor.x >= BIOS_COLS) {
			cursor.x = 0;
			if (++cursor.y >= BIOS_ROWS) scroll();
		};
		break;
	};
	invert();
	return *this;
}	// }}}
BIOS& BIOS::operator<< (const char* str) {	// {{{ Operátor pro zápis řetězce (včetně podpory pro __flash řetězce)
	while (*str) { *this << *str++; };
	return *this;
}	// }}}
BIOS& BIOS::operator<< (const __FlashStringHelper* str) {	// {{{ Operátor pro zápis řetězce z __flash paměti
	const char* p = reinterpret_cast<const char*>(str);
	char c;
	while ((c = pgm_read_byte_near(p++))) {
		*this << c;
	}
	return *this;
}	// }}}
BIOS& BIOS::operator<< (int num) {	// {{{
	char buffer[10];
	itoa(num, buffer, 10);
	*this << buffer;
	return *this;
}	// }}}
BIOS& BIOS::operator<< (uint16_t num) {	// {{{
	char buffer[10];
	ultoa(num, buffer, 10);
	*this << buffer;
	return *this;
}	// }}}
BIOS& BIOS::operator<< (uint32_t num) {	// {{{
	char buffer[14];
	ultoa(num, buffer, 10);
	*this << buffer;
	return *this;
}	// }}}
BIOS& BIOS::operator<< (YX pos) {	// {{{
	set_cursor(pos.y,pos.x);
	return *this;
}	// }}}
BIOS& BIOS::operator<< (XY pos) {	// {{{
	set_cursor(pos.y,pos.x);
	return *this;
}	// }}}
BIOS& BIOS::operator<< (COLOR col) {	// {{{
	cram[cursor.y] = col.col;
	return *this;
}	// }}}

void BIOS::set_output(BIOS_output output){							// {{{
//	if (current_output==output) return;	// no change, no init				// let reinit happened
	if (current_output==BIOS_VGA) VGA_end();
	if (current_output==BIOS_RCA) RCA_end();
	current_output=output;
//	for (int i =0; i<=13;++i) pinMode(i, INPUT);
//	for (int i =0; i<=13;++i) pinMode(i, INPUT);
//	pinMode(0,OUTPUT); digitalWrite(0,0);
//	pinMode(2,OUTPUT); digitalWrite(2,0);
	if (current_output==BIOS_VGA) VGA_begin();
	if (current_output==BIOS_RCA) RCA_begin();
}	// }}}
void BIOS::invert() {										// {{{ inverts character at actual cursor
	if (inverting) vram[cursor.y][cursor.x] ^= 0x80;
}	// }}}
void BIOS::printNum(int32_t i, uint8_t size, char fill) {				// {{{
	invert();
	uint8_t *first=&vram[cursor.y][cursor.x];
	uint8_t *last;
	bool neg=(i<0);
	if (neg) i= -i;
	do {
		vram[cursor.y][cursor.x++]= hexa_digits[i%10];
		i/=10;
		if (cursor.x >= BIOS_COLS) {
			cursor.x=0;
			if (++cursor.y >= BIOS_ROWS) { scroll(); first-=BIOS_COLS;};
		};
	} while(i);
	if (neg) vram[cursor.y][cursor.x++]='-';
	if (cursor.x >= BIOS_COLS) {
		cursor.x=0;
		if (++cursor.y >= BIOS_ROWS) { scroll(); first-=BIOS_COLS;};
	};
	// TODO size, fill
	last=&vram[cursor.y][cursor.x];
	while (size >last - first){vram[cursor.y][cursor.x++]=fill;last=&vram[cursor.y][cursor.x];}; --last;
	while (last>first) { uint8_t x=*last;*last--=*first;*first++=x;};
	invert();
	// TODO lepsi verzi s divmod10_asm
}	// }}}
void BIOS::printHex(uint32_t i, uint8_t size, char fill) {				// {{{
	invert();
	uint8_t buf[8], *p,sz;
	p = buf + (sizeof(buf));
	do {
//		*--p=((i & 0x0F) >9 ? 'A' - 10:'0') + (i & 0x0F);
		*--p=hexa_digits[i & 0x0F];
		i >>=4;
	} while (i);
	sz = sizeof(buf) - (p - buf);
	if (sz > size) size=sz;
	if (size+cursor.x>=BIOS_COLS && cursor.y >= BIOS_ROWS-1) {
		scroll();
		--cursor.y;
	};
	uint8_t *f=&vram[cursor.y][cursor.x];
	if (size+cursor.x>=BIOS_COLS) cursor.y++;
	cursor.x = (cursor.x + size) % BIOS_COLS;
	while (sz < size-- ) *f++=fill;
	while (sz--) *f++=*p++;
	invert();
}	// }}}
void BIOS::printBin(uint32_t i, uint8_t size, char fill) {				// {{{
	invert();
	uint8_t buf[32], *p,sz;
	p = buf + (sizeof(buf));
	do {
		*--p = '0' + (i & 1);
		i >>=1;
	} while (i);
	sz = sizeof(buf) - (p - buf);
	if (sz > size) size=sz;
	if (size+cursor.x>=BIOS_COLS && cursor.y >= BIOS_ROWS-1) {
		scroll();
		--cursor.y;
	};
	uint8_t *f=&vram[cursor.y][cursor.x];
	if (size+cursor.x>=BIOS_COLS) cursor.y++;
	cursor.x = (cursor.x + size) % BIOS_COLS;
	while (sz < size-- ) *f++=fill;
	while (sz--) *f++=*p++;
	invert();
}	// }}}
void BIOS::write(char c) {									// {{{
	vram[cursor.y][cursor.x++] = c;
	if (cursor.x >= BIOS_COLS) {
		cursor.x=0;
		if (++cursor.y >= BIOS_ROWS) scroll();
	};
	invert();
}	// }}}
void BIOS::write(const char *c) {								// {{{
	while (*c) write(*c++);
}	// }}}
void BIOS::scroll() { 										// {{{ cursor not scrolled
	memmove(&vram[0][0], &vram[1][0], sizeof(vram[0][0]) * (BIOS_ROWS-1)*BIOS_COLS);	// scroll video RAM
	memset(&vram[BIOS_ROWS-1][0], ' ', sizeof(vram[0][0]) * BIOS_COLS);
	
	memmove(&cram[0], &cram[1], sizeof(cram[0]) * (BIOS_ROWS-1));				// scroll color RAM
	cram[BIOS_ROWS-1] = cram[BIOS_ROWS-2];
	
	if (cursor.y >= BIOS_ROWS) cursor.y = BIOS_ROWS-1;
}	// }}}
void BIOS::clear(char c, BIOS_Color col) { 							// {{{ cursor to 0,0
	memset((void*)&vram[0][0], c, BIOS_ROWS*BIOS_COLS);
	memset((void*)&cram[0], col, BIOS_ROWS);
	set_cursor(0,0);
}	// }}}

void BIOS::set_cursor(uint8_t row, uint8_t col) {						// {{{
	cursor.x = col < BIOS_COLS? col: BIOS_COLS-1;
	cursor.y = row < BIOS_ROWS? row: BIOS_ROWS-1;
}	// }}}
void BIOS::set_rowcolor(uint8_t row, BIOS_Color color) {					// {{{
	if (row<BIOS_ROWS)
		cram[row] = color;
}	// }}}
void BIOS::set_color(BIOS_Color color) {							// {{{
	cram[cursor.y] = color;
}	// }}}

uint16_t BIOS::get_key() {										// {{{
//
	static bool altgr = false;	// state of some important keys of the PS2 keyboard
	static bool shift = false;
	static bool caps = false;
	static bool nums = false;
	static bool relea = false;	// indicating that the next key was released
//	static uint8_t last_char=' ';
	while( uint8_t scan = BIOS_buffer_get_char())	// are any unread input in the ring buffer?
	{
		uint8_t s=0; if (shift || caps) s = 1; else if (altgr) s = 2;	// select bank of lookup according to states of special keys
		uint16_t key=pgm_read_word_near(&BIOS::ScanToASCII[s][scan & 0x7F]);
		if (scan==0x83) key=xF7;
//		{char c; c=scan >> 4; BIOS::vram[0][0]=(c>9?'A'-10+c:'0'+c); c=scan & 0x0F; BIOS::vram[0][1]=((c>9)?'A'-10+c:'0'+c);};
		switch (scan)
			{
			case 0xf0: relea = true; break;	// key release indicator	
			case 0xe0: break;	// ignore prefix of special keys
			case 0x11: altgr = !relea; relea = false; break;	// ALT, ALTGR
			case 0x12: case 0x59: shift = !relea; relea = false; break;	// treat LSHIFT and RSHIFT the same
//			case 0x76: if (!relea) clear(last_char);break;	//Escape
			case 0x77: if (!relea) nums = !nums; break;
			case 0x58: if (!relea) caps = !caps; break;
			default:	// any other key
				if (relea == true) relea = false;	// key released => don't emit anything
				else											// key pressed => emit ASCII code according to lookup table
				{
//					last_char=key;
					if (nums) switch (key){
						case x1: key='1'; break;
						case x2: key='2'; break;
						case x3: key='3'; break;
						case x4: key='4'; break;
						case x5: key='5'; break;
						case x6: key='6'; break;
						case x7: key='7'; break;
						case x8: key='8'; break;
						case x9: key='9'; break;
						case x0: key='0'; break;
						case xDot: key='.'; break;
						case xStar: key='*'; break;
						case xMinus: key='-'; break;
						case xPlus: key='+'; break;
						};
					return key;
				}
				break;
			}
		}
	return 0;
}	// }}}
uint8_t BIOS::get_scancode() {									// {{{
	return BIOS_buffer_get_char();
return 0;
}	// }}}
extern "C" {
	void VGA_hook();
};
void BIOS::VGA_begin(){										// {{{
	current_output = BIOS_VGA;
	VGA_hook();
	for (int i =0; i<=13;++i) pinMode(i, INPUT);
		noInterrupts();	// disable interrupts before messing around with timer registers

		DDRC	= 0b00111100;	// PORTC is always input
		PORTC = 0b00111111;	// enable 20k pull-up resistors for CLK and DAT of PS/2
		DDRD	= 0b11111111;	// PORTD is always output
		PORTD = 0b00000000;
		DDRB	= 0b00111100;	// B0: CLKO, B2: /VSYNC (timer1), B3: /PE by hand, B4: /HSYNC by hand in ISR
		PORTB = 0b00011100;	// B0: CLKO, B2: /VSYNC=1, B3: /PE=1, B4: /HSYNC=1
			
		GTCCR = 0b10000011;	// set TSM, PSRSYNC und PSRASY to correlate all 3 timers

		// *****************************
		// ***** Timer0: VGA HSYNC *****
		// *****************************
		TCNT0	= 4;	// aligns VSYNC and HSYNC pulses
		TCCR0A = (1 << WGM01) | (0 << WGM00);	// mode 2: Clear Timer on Compare Match (CTC)
		TCCR0B = (0 << WGM02) | (0 << CS02) | (1 << CS01) | (0 << CS00); // x8 prescaler -> 0.5µs
		OCR0A	= 63;	// 60cols (24MHz): 95, 40cols (16MHz): 63, compare match register A (TOP) -> 32µs
		TIMSK0 = (1 << OCIE0A);	// Output Compare Match A Interrupt Enable (not working: TOIE1 with ISR TIMER0_TOIE1_vect because it is already defined by timing functions)

		// *****************************
		// ***** Timer1: VGA VSYNC *****
		// *****************************
		TCNT1	= 0;
		TCCR1A = (1 << COM1B1) | (1 << COM1B0) | (1 << WGM11) | (1 << WGM10); // mode 15 (Fast PWM), set OC1B on Compare Match, clear OC1B at BOTTOM, controlling OC1B pin 10
		TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (0 << CS11) | (1 << CS10); // x1024 prescaler -> 64µs
		OCR1A	= 259;	// 60cols (24MHz): 389, 40cols (16MHz): 259, compare match register A (TOP) -> 16.64ms
		OCR1B	= 0;	// compare match register B -> 64µs
		TIMSK1 = (1 << TOIE1);	// enable timer overflow interrupt setting vlines = 0

		// ************************************************
		// ***** Timer2: only used for jitter control *****
		// ************************************************
		TCNT2	= 0;
		TCCR2A = (0<<COM2A1) | (0<<COM2A0) | (1<<WGM21) | (1<<WGM20); // mode 7: Fast PWM, COM2A0=0: normal port HIGH, COM2A0=1: Toggle OC2A pin 11 on Compare Match
		TCCR2B = (1<<WGM22) | (0<<CS22) | (0<<CS21) | (1<<CS20) ;	// set x0 prescaler -> 62.5ns;
		OCR2A	= 7;			// compare match register A (TOP) -> 250ns
		TIMSK2 = 0;			// no interrupts here

		GTCCR = 0;	// clear TSM => all timers start synchronously
		UCSR0B = 0;	// brute-force the USART off just in case...

		interrupts();

}	// }}}
	volatile uint16_t BIOS::frames = 0;		// VGA counting the displayed frames (60Hz)
	volatile int BIOS::vline asm("vline") = 0;	// VGA current vertical position of pixel video output
	volatile uint16_t scanline = 0;			// RCA counts 0 - 311 (312 scan lines per frame)
// Function to enable/disable A0 interrupt
void disableA0Interrupt() {
	PCMSK1 &= ~(1 << PCINT8);  // Disable interrupt for A0
}
void enableA0Interrupt() {
	PCIFR  |= bit (PCIF1);   // clear any outstanding interrupts
	PCMSK1 |= (1 << PCINT8);   // Enable interrupt for A0
}
extern "C" {
	void RCAout(uint8_t *pScreenRam, const uint8_t *fontSlice, uint16_t tcnt, uint16_t minTCNT);
	void PS2_cont();
}
ISR(TIMER1_OVF_vect) {
	if ( BIOS::current_output == BIOS_VGA) {	// {{{ timer1 overflow interrupt resets vline counter at HSYNC
		BIOS::vline = 0;
		BIOS::frames++;
	} 						// }}}
	else if ( BIOS::current_output == BIOS_RCA) {	// {{{ TIMER1_OVF vector occurs at the start of each scan line's sync pulse
		if (++scanline == 312) {
			OCR1A = 948; 		// scan lines 0 - 7 have wide 59.3us sync pulses
			scanline = 0;
			BIOS::frames++;
		} else if (scanline == 8) {
			OCR1A = 74;		// swap to short 4.7us sync pulses for scan lines 8 - 311
						// enabling the interrupt generates an immediate 'stored up' interrupt
						// so enable it one scan line early, test and return within interrupt handler to ignore 1st one
		} else if (scanline == RCA_TOP_EDGE) {	// scan line 51 is first 'text safe' scan line - will already have been incremented to 52 here
			TIMSK1 |= _BV(OCIE1B);
		} else if (scanline == RCA_TOP_EDGE +1) {
			disableA0Interrupt();
			PS2_cont();
		} else if (scanline > RCA_TOP_EDGE) {
			PS2_cont();
		};
	};	// }}}
}	//
volatile uint16_t minTCNT = 0xFFFF;
volatile uint16_t maxTCNT = 0;


// Interrupt Service Routine for Pin Change Interrupt 1 (A0-A5)
ISR(PCINT1_vect) {
// asm volatile("SBI %[addr], 3 \n\t" : : [addr] "I" (_SFR_IO_ADDR(PIND)) );
	PS2_cont();	// pokracovaci funkce od RCAout vlozeneho kodu. Chceme ji volat kdykoli se zmeni A0 a klidne i casteji
//	++BIOS::vram[0][10];
// asm volatile("SBI %[addr], 3 \n\t" : : [addr] "I" (_SFR_IO_ADDR(PIND)) );
}
ISR(TIMER1_COMPB_vect) {		// {{{ occurs at start of 'text safe' area of scan lines 51 - 280
	static uint8_t *pScreenRam;
	static const uint8_t *fontSlice;
	static uint8_t slice;
	
	uint16_t tcnt = TCNT1;		// capture timer to allow jitter correction
	
	if (scanline == RCA_TOP_EDGE) {	// on stored-up 'false trigger' scanline, initialize the pointers
		slice = 0;
		pScreenRam = &BIOS::vram[0][0];	// point to first character (top left) in screenRam
//		fontSlice = &charset[0][0];	// point to slice before first (top) slice of font pixels (top pixel of each 10 is just RVS cap)
		fontSlice = &((*BIOS::chardef)[0][0]);
	} else {
		RCAout(pScreenRam, fontSlice, tcnt, minTCNT);
		if (tcnt > maxTCNT) maxTCNT = tcnt;
		if (tcnt < minTCNT) minTCNT = tcnt;

		if (scanline == RCA_BOTTOM_EDGE) {
			TIMSK1 &= ~_BV(OCIE1B);	// we don't want any more COMPB interrupts this frame
			enableA0Interrupt();
		} else if (++slice == RCA_PIXELS_PER_CHARACTER) {
			slice = 0;
//			fontSlice = &charset[0][0];
//			fontSlice = chardef;
		fontSlice = &((*BIOS::chardef)[0][0]);
			pScreenRam += BIOS_COLS;
		} else {
			fontSlice += 256;
		}
	}
}	// }}}

void BIOS::VGA_end(){										// {{{
	current_output = BIOS_none;
	noInterrupts();	// disable interrupts before messing around with timer registers
	TIMSK0=0;
	TIMSK1=0;
	TIMSK2=0;
	interrupts();
}	// }}}




#define PIN_SUPPRESS 2
#define PIN_SUPPRESS3 3 // test
#define PIN_SYNC 9
#define PIN_PS2CLOCK A0	// 10
#define PIN_PS2DATA A1	// 11
#define PIN_PS2INSIDE 13
void BIOS::RCA_begin() {				// {{{
	current_output = BIOS_RCA;
///////////////////////////////// better safe than sorry ///////////////////////
//		EIMSK &= ~(1 << INT1);
	pinMode(PIN_SUPPRESS3, OUTPUT);
	pinMode(PIN_SUPPRESS, OUTPUT);
	pinMode(PIN_SYNC, OUTPUT);
///////////////////////////////// better safe than sorry ///////////////////////
	BIOS_buffer_init();
	pinMode(PIN_PS2CLOCK, INPUT);
	pinMode(PIN_PS2DATA, INPUT);
	pinMode(PIN_PS2INSIDE, INPUT);
	
	
////////////////////////////////////////////////////////////////////////////////
	
	scanline=0;
	cli();  // Disable interrupts while configuring
	// Enable Pin Change Interrupt for CLOCK A0 (which is on PCINT8 group - Port C)
	PCMSK1 |= (1 << PCINT8); // Enable PCINT8 (A0 is PC0) want pin A0
	PCIFR  |= (1 << PCIF1);   // clear any outstanding interrupts
	PCICR  |= (1 << PCIE1);   // Enable Pin Change Interrupt for PCINT[14:8] (Port C)
	sei();  // Enable interrupts

	
//	pinMode(PIN_SUPPRESS, INPUT);
	digitalWrite(PIN_SUPPRESS, HIGH);	//test
	
//	pinMode(PIN_SUPPRESS, INPUT);
	digitalWrite(PIN_SUPPRESS, LOW);	// prime pixel suppressor - when Pin is switched to output, it will force BLACK
					// configure USART as master SPI mode 0, MSB first, 8MHz
	UCSR0A = _BV(U2X0);		// double speed
	UCSR0B = _BV(TXEN0);
	UCSR0C = _BV(UMSEL01) | _BV(UMSEL00);
	UBRR0L = UBRR0H = 0x00;		// fastest possible baud
	
					// output pin for sync pulses - low 4.7 us pulses at start of visible scan lines;  longer low pulses for vertical blank
	digitalWrite(PIN_SYNC, HIGH);
	pinMode(PIN_SYNC, OUTPUT);
					// configure timer/counter 1 to output scanline sync pulses on Pin9 (OC1A)
					// use mode 7 (fast PWM 10-bit count to TOP=1023) at 16MHz fclk - one cycle per 64us scanline
	cli(); 				// not necessary
	TCCR1A =	_BV(COM1A1) | _BV(COM1A0) | _BV(WGM11) | _BV(WGM10);	// set OC1A output on compare match, (mode 3 so far)
	TCCR1B = _BV(WGM12) | _BV(CS10);	// now mode 7 at clk/1 (16MHz)
	OCR1A = 948; 			// 59.3us wide sync pulse for first 8 scan lines
	OCR1B = RCA_LEFT_EDGE;
	TIMSK1 = _BV(TOIE1); 		// _BV(OCIE1A);
	TCNT1 = 0x0000;
	sei();				// necessary
	TIMSK0 &= ~_BV(TOIE0);		// disable timer0 - stops millis() working but necessary to stop timer 0 interrupts spoiling display timing

/*
	// PIN_SUPPRESS - explicitnejsi nastaveni
	// ; SUPPRESS on PortD, pin 2 = D2
	DDRD &= ~(1<<PIN_SUPPRESS);	// nastavim INPUT (nahodou D2=2)
	PORTD &= ~(1<<PIN_SUPPRESS);	// nastavim LOW = no PULLUP
	DDRD |= (1<<PIN_SUPPRESS);	// nastavim OUTPUT (nahodou D2=2)
	PORTD &= ~(1<<PIN_SUPPRESS);	// nastavim LOW
	DDRD &= ~(1<<PIN_SUPPRESS);	// nastavim INPUT (nahodou D2=2)
*/
}	// }}}
void BIOS::RCA_end(){				// {{{
	// TODO
	current_output = BIOS_none;
	PCMSK1 &= ~(1 << PCINT8);	// Disable PCINT8 (A0 is PC0)
	PCICR  &= ~(1 << PCIE1);	// Disable Pin Change Interrupt for PCINT[14:8] (Port C)
	
	TIMSK1 &= ~_BV(TOIE1); 		// _BV(OCIE1A);
	TIMSK0 &= ~_BV(TOIE0);		// disable timer0 - stops millis() working but necessary to stop timer 0 interrupts spoiling display timing
	PCMSK1 &= ~(1 << PCINT8);	// Disable interrupt for A0
	
	TIMSK0=0;
	TIMSK1=0;
	TIMSK2=0;
	UCSR0A = 0;
	UCSR0B = 0;
	UCSR0C = 0;
}	// }}}

void BIOS::wait(unsigned int dt) { unsigned int t = BIOS::frames; while(BIOS::frames - t < dt); }


/*
int main() __attribute__((weak));
int main()							// enforce main() loop w/o serial handler
	{
	BIOS::set_output();
	
		memset((void*)BIOS::vram, 32, BIOS_ROWS*BIOS_COLS);			// clear the video RAM
		memset((void*)BIOS::cram, 15, BIOS_ROWS);			// set color RAM to white
		BIOS::text(F("**** Nano Home Computer 2.3 ****"), 1,3, VGA_YELLOW);
		BIOS::text(F("26KB of FLASH and 1KB of SRAM"), 2,3, VGA_CYAN);
		BIOS::set_cursor(3,3);
		BIOS::set_color(VGA_GREEN);
		BIOS::writeNum(BIOS_COLS);
		BIOS::write('x');
		BIOS::writeNum(BIOS_ROWS);
		
		for(uint8_t a=1;a<BIOS_ROWS-1;a++) {
			BIOS::vram[a][0]='|';
			BIOS::vram[a][BIOS_COLS-1]='|';
		};
		for(uint8_t a=1;a<BIOS_COLS-1;a++) {
			BIOS::vram[0][a]='-';
			BIOS::vram[BIOS_ROWS-1][a]='-';
		};
		BIOS::vram[0][0]='+';
		BIOS::vram[0][BIOS_COLS-1]='+';
		BIOS::vram[BIOS_ROWS-1][0]='+';
		BIOS::vram[BIOS_ROWS-1][BIOS_COLS-1]='+';
	
	setup();
	while(true) loop();
	}
*/
BIOS bios=BIOS();

