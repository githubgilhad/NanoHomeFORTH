#ifndef DEFINES_H
#define DEFINES_H

#define OUTPUT_TARGET_terminal 1
#define OUTPUT_TARGET_vram 2

// #define OUTPUT_TARGET OUTPUT_TARGET_terminal
// #define OUTPUT_TARGET OUTPUT_TARGET_vram

#if OUTPUT_TARGET == OUTPUT_TARGET_vram
//	#define STR_2LESS "«" //0xC2 0xAB);//'«'
//	#define STR_2MORE "»" // 0xC2 0xBB);//'»'
	#define STR_2MORE "" // '»' 0x1F
	#define STR_2LESS "" // '«' 0x1E
	#define STR_RIGHT "" // '-»' 0x1D
	#define STR_LEFT  "" // '«-' 0x1C
	#define STR_UP    "" // '^' 0x1B
	#define STR_DOWN  "" // 'v' 0x1A
	
	#define PROMPT STR_RIGHT
	#define PROMPTcomp STR_RIGHT STR_2MORE

	#define BIOS_ROWS		25					// number of rows of VGA text output
	#define BIOS_COLS		37					// number of columns of VGA text output (more than 37 means noise from PS/2 input)

#elif OUTPUT_TARGET == OUTPUT_TARGET_terminal
	#define STR_2LESS "«" //0xC2 0xAB);//'«'
	#define STR_2MORE "»" // 0xC2 0xBB);//'»'
	#define STR_RIGHT "-»" // '«-' 0x1D
	#define STR_LEFT  "«-" // '-»' 0x1C
	#define STR_UP    "^" // '^' 0x1B
	#define STR_DOWN  "v" // 'v' 0x1A
	
	#define PROMPT STR_RIGHT
	#define PROMPTcomp STR_RIGHT STR_2MORE
	
#else
	#warning "Unknown target OUTPUT_TARGET"
#endif

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
	#define RAM_LEN 	320	// word ~ 10B + name + 4 * words called - for start some 10 words should be enought
	#define STACK_LEN	30
	#define RSTACK_LEN	30
#elif defined(__PC__)
	#define RAM_LEN 	32000	// 32k should be enought for anyone :)
	#define STACK_LEN	40
	#define RSTACK_LEN	40
#else
#error undefined
#endif

#if defined(__PORTABLE_GRAPHIC__)
	#define MAX_ROWS		25					// number of rows of emulated VGA text output
	#define MAX_COLS		37					// number of columns emulated of VGA text output (more than 37 means noise from PS/2 input)
#endif

#endif
