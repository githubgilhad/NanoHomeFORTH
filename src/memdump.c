/* vim: set ft=c showbreak=»\  noexpandtab fileencoding=utf-8 nomodified wrap textwidth=0 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\:|- list: tabstop=8 linebreak  */
// ,,g = gcc, exactly one space after "set"

#if defined(__PC__)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int memdump(uintptr_t addr, size_t size, const char *fname) {
	FILE *f = fopen(fname, "ab");
	if (!f) {
		perror("fopen");
		return -1;
	};

	void *ptr = (void *)addr;

	if (0 >= fprintf(f,"<==== 0x%08x + 0x%04x  ====>",addr,size)) {
		perror("fprintf");
		fclose(f);
		return -1;
	};

	size_t written = fwrite(ptr, 1, size, f);
	if (written != size) {
		perror("fwrite");
		fclose(f);
		return -1;
	};

	fclose(f);
	return 0;
};

#endif
