/* vim: set ft=c showbreak=»\  noexpandtab fileencoding=utf-8 nomodified wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: tabstop=8 linebreak  */
// ,,g = gcc, exactly one space after "set"

#ifndef MEMDUMP_H
#define MEMDUMP_H

#if defined(__PC__)
int memdump(uintptr_t addr, size_t size, const char *fname);

#endif
#endif
