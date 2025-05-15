#ifndef IO_H
#define IO_H
extern char read_char();
extern void write_char(char c);
extern void write_charA(char c);
extern void write_str(const __memx char *s);
extern void write_eoln();
extern char wait_for_char();
#endif
