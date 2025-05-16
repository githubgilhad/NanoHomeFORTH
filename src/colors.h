/* vim: set ft=cpp noexpandtab fileencoding=utf-8 nomodified wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: tabstop=8 linebreak showbreak=»\  */
// ,,g = gcc, exactly one space after "set"

//	ANSI barvy (volitelně vypnout)
#ifndef ANSI_COLORS
	#ifndef OUTPUT_TARGET
		#define ANSI_COLORS 1
	#elif OUTPUT_TARGET == OUTPUT_TARGET_vram
		#define ANSI_COLORS 0
	#elif OUTPUT_TARGET == OUTPUT_TARGET_terminal
		#define ANSI_COLORS 1
	#else
		#warning "Unknown target OUTPUT_TARGET"
	#endif
#endif

#if ANSI_COLORS
	#define CLR_GREY    "\x1b[90m"
	#define CLR_GREEN   "\x1b[32m"
	#define CLR_YELLOW  "\x1b[33m"
	#define CLR_WHITE   "\x1b[37m"
	#define CLR_HRED    "\x1b[1;31m"   // výrazná červená
	#define CLR_HGREEN  "\x1b[1;32m"   // výrazná zelená
	#define CLR_HYELLOW "\x1b[1;33m"   // výrazná žlutá
	#define CLR_HBLUE   "\x1b[1;34m"   // výrazná modrá
	#define CLR_HCYAN   "\x1b[1;36m"   // světle modrá / tyrkysová
	#define CLR_HWHITE  "\x1b[1;37m"   // jasně bílá
	#define BG_GREEN    "\x1b[42m"     // zelené pozadí
	#define BG_RED      "\x1b[41m"     // červené pozadí
	#define BG_YELLOW   "\x1b[43m"     // žluté pozadí
	#define STYLE_BOLD  "\x1b[1m"      // jen tučné písmo
	#define CLR_RESET   "\x1b[0m"      // reset všeho
	#define BG_BLUE  "\x1b[44m"  // klasická tmavě modrá
	#define CLR_BLUE  "\x1b[34m"
#else
	#define CLR_GREY    ""
	#define CLR_GREEN   ""
	#define CLR_YELLOW  ""
	#define CLR_WHITE   ""
	#define CLR_HRED    ""
	#define CLR_HGREEN  ""
	#define CLR_HYELLOW ""
	#define CLR_HBLUE   ""
	#define CLR_HCYAN   ""
	#define CLR_HWHITE  ""
	#define BG_GREEN    ""  
	#define BG_RED      ""  
	#define BG_YELLOW   ""  
	#define STYLE_BOLD  ""   
	#define CLR_RESET   ""   
	#define BG_BLUE  "" 
	#define CLR_BLUE  ""
#endif
