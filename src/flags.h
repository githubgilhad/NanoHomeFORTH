/* vim: set ft=cpp noexpandtab fileencoding=utf-8 nomodified wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: tabstop=8 linebreak showbreak=»\  */
// ,,g = gcc, exactly one space after "set"


#define FLG_IMMEDIATE	0x80		// run in st_executing STATE
#define FLG_HIDDEN	0x40		// do not show this
#define FLG_ARG		0x01		// if next PTR_t is argument, not next entry
#define FLG_PSTRING	0x02		// PSTRING follows (LITSTRING)
// #define FLG_NOFLAG	~(FLG_IMMEDIATE | FLG_HIDDEN)
#define F_TRUE	0x1	// or -1?
#define F_FALSE	0x0
