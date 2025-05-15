#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include "colors.h"
#include "flags.h"
#include "ptr24.h"
#include "io.h"
#define DEBUG_DUMP(P,LBL) debug_dump(P24p(P),F(LBL));
#define DEBUG_DUMPu(U,LBL) debug_dump((ptr24_u ){.u32=U},F(LBL));
void error(const __memx char *c); 
void info(const __memx char *c); 
void trace(const __memx char *c); 
void dump24(uint32_t w, const __memx char *label);
void write_hex8(uint8_t b);
void write_hex16(uint16_t b);
void write_hex24(uint32_t b);
void write_hex32(uint32_t b);
bool is_ram_address(uint32_t addr);
bool is_flash_address(uint32_t addr);
//void debug_dump(const __memx void * address, const __memx char* label);
void debug_dump(uint32_t address, const __memx char* label);
extern bool nodebug;
extern bool noinfo;
extern bool notrace;
	#define STR_2LESS "«" //0xC2 0xAB);//'«'
	#define STR_2MORE "»" // 0xC2 0xBB);//'»'

#endif
