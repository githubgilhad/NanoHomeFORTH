#ifndef BIOSBUFFER_H
#define BIOSBUFFER_H

#define BUFFER_MASK 0x0F

#ifdef __cplusplus
extern "C" {
	extern char BIOS_buffer;
	extern uint8_t BIOS_buffer_tail_ptr;
	extern uint8_t BIOS_buffer_head_ptr;
	
	void BIOS_buffer_init();
	void BIOS_buffer_put_char(uint8_t c);
	uint8_t BIOS_buffer_get_char();
	void ps2_read();
	};
#else
	.extern BIOS_buffer
	.extern BIOS_buffer_tail_ptr
	.extern BIOS_buffer_head_ptr
	
	.extern BIOS_buffer_init
	.extern BIOS_buffer_put_char
	.extern BIOS_buffer_get_char
	.extern ps2_read
#endif

#endif
