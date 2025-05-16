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
#elif OUTPUT_TARGET == OUTPUT_TARGET_terminal
	#define STR_2LESS "«" //0xC2 0xAB);//'«'
	#define STR_2MORE "»" // 0xC2 0xBB);//'»'
	#define STR_RIGHT "-»" // '«-' 0x1D
	#define STR_LEFT  "«-" // '-»' 0x1C
	#define STR_UP    "^" // '^' 0x1B
	#define STR_DOWN  "v" // 'v' 0x1A
	
	#define PROMPT STR_RIGHT
#else
	#warning "Unknown target OUTPUT_TARGET"
#endif

#endif
