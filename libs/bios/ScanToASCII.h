#ifndef SCANTOASCII_H
#define SCANTOASCII_H
#include <inttypes.h>
#include <avr/pgmspace.h>

extern const uint16_t ScanToASCII[3][128] PROGMEM ; // [x][..]: SHIFT/ALTGR keystate and PS/2 scancode, [..][y]: ASCII code)

#define xCTRL '^'+0x80		//	#define xCTRL '^'+0x80
#define xEsc '\e'		//	#define xEsc '['+0x80
#define xTab '\t'		//	#define xTab 'I'+0x80
#define xBack '\b'		//	#define xBack '!'+0x80
#define xEnter '\n'		//	#define xEnter 'M'+0x80
#define xCaps 'C'+0x80		//	#define xCaps 'C'+0x80
#define xLShft '<'+0x80		//	#define xLShft '<'+0x80
#define xRShft '>'+0x80		//	#define xRShft '>'+0x80
#define xAlt 'A'+0x80		//	#define xAlt 'A'+0x80

#define xWin 'W'+0x80		//	#define xWin 'W'+0x80
#define xWinR 'w'+0x80		//	#define xWinR 'w'+0x80
#define xMenu 'm'+0x80		//	#define xMenu 'm'+0x80
#define xNum 'N'+0x80		//	#define xNum 'N'+0x80
#define xStar '*'+0x80		//	#define xStar '*'+0x80
#define xMinus '-'+0x80		//	#define xMinus '-'+0x80
#define xPlus '+'+0x80		//	#define xPlus '+'+0x80
// #define xLom '/'+0x80		//	// #define xLom '/'+0x80
#define xDot '.'+0x80		//	#define xDot '.'+0x80
#define x0 '0'+0x80		//	#define x0 '0'+0x80
#define x1 '1'+0x80		//	#define x1 '1'+0x80
#define x2 '2'+0x80		//	#define x2 '2'+0x80
#define x3 '3'+0x80		//	#define x3 '3'+0x80
#define x4 '4'+0x80		//	#define x4 '4'+0x80
#define x5 '5'+0x80		//	#define x5 '5'+0x80
#define x6 '6'+0x80		//	#define x6 '6'+0x80
#define x7 '7'+0x80		//	#define x7 '7'+0x80
#define x8 '8'+0x80		//	#define x8 '8'+0x80
#define x9 '9'+0x80		//	#define x9 '9'+0x80

#define xF1 '1'+0x180		//	#define xF1 '1'+0x180
#define xF2 '2'+0x180		//	#define xF2 '2'+0x180
#define xF3 '3'+0x180		//	#define xF3 '3'+0x180
#define xF4 '4'+0x180		//	#define xF4 '4'+0x180
#define xF5 '5'+0x180		//	#define xF5 '5'+0x180
#define xF6 '6'+0x180		//	#define xF6 '6'+0x180
#define xF7 '7'+0x180		//	#define xF7 '7'+0x180
#define xF8 '8'+0x180		//	#define xF8 '8'+0x180
#define xF9 '9'+0x180		//	#define xF9 '9'+0x180
#define xF10 'A'+0x180		//	#define xF10 'A'+0x180
#define xF11 'B'+0x180		//	#define xF11 'B'+0x180
#define xF12 'C'+0x180		//	#define xF12 'C'+0x180

#endif
