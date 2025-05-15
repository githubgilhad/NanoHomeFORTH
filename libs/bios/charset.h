#ifndef CHARSET_H
#define CHARSET_H

	#ifdef __cplusplus
		#include <inttypes.h>
		#include <avr/pgmspace.h>
		extern	 const uint8_t charset[9][256] PROGMEM;	// VGA just 8 top lines, RCA all 9
		void charset_hook();
	#else
		.extern BIOS::charset
	#endif

#endif
