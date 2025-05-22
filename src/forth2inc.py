#!/usr/bin/python -u
# vim: fileencoding=utf-8:nomodified:nowrap:textwidth=0:foldmethod=marker:foldcolumn=4:ruler:showcmd:lcs=tab\:|- list:tabstop=8:noexpandtab:nosmarttab:softtabstop=0:shiftwidth=0


import argparse
import re
import sys

def parse_args():
	parser = argparse.ArgumentParser(description="Translate FORTH definitions to assembler code.")
	parser.add_argument("source", nargs="?", default="words.4th", help="source file to translate (words.4th)")
	parser.add_argument("asmfile", nargs="?", default="asm.S", help="Already defined words file (asm.S)" )
	parser.add_argument("outfile", nargs="?", default="words.inc", help="output file to create (words.inc)" )
	parser.add_argument("-v", "--verbose", action="store_true")
	parser.add_argument("-c", "--col", type=int, default=40, help="Column for comments (default: 40)")
	return parser.parse_args()

def load_known_words(asmfile):
	known = {}
	patternDW = re.compile(r'^\s*DEFWORD\s+(\w+\\?),[^,]*,\s*"([^"]+)",')
	patternDV = re.compile(r'^\s*DEFVAR\s+(\w+\\?)')
	patternDC1 = re.compile(r'^\s*DEFCONST1\s+(\w+\\?)')
	patternDC2 = re.compile(r'^\s*DEFCONST2\s+(\w+\\?)')
	patternPORT = re.compile(r'^\s*PORT\s+(\w+\\?),')
	with open(asmfile, encoding='utf-8') as f:
		for line in f:
			m = patternDW.search(line)
			if m:
				label, name = m.groups()
				if not '\\' in name:
					known[name] = label
			m = patternDV.search(line)
			if m:
				name, = m.groups()
				known[name] = "var_"+name
			m = patternDC1.search(line)
			if m:
				name, = m.groups()
				known[name] = "const_"+name
			m = patternDC2.search(line)
			if m:
				name, = m.groups()
				if not '\\' in name:
					known[name] = "const_"+name
			m = patternPORT.search(line)
			if m:
				name, = m.groups()
				for base in ("PORT","PIN","DDR"):
					known[base+name] = "const_"+base+name
	return known

def strip_forth_comments(line,nest=0):
	result = ''
	i = 0
	while i < len(line):
		if line[i] == '\\':
			break
		elif line[i] == '(':
			nest += 1
			i += 1
			while i < len(line) and nest > 0:
				if line[i] == '(':
					nest += 1
				elif line[i] == ')':
					nest -= 1
				i += 1
		else:
			result += line[i]
			i += 1
	return (nest,result.strip())

def extract_nick_from_comment(line):
	m = re.search(r'\(.+"([^"]+)".+\)', line)
	return m.group(1) if m else None
glob_imm = False
def extract_immediate_from_line(line):
	global glob_imm
	m = re.search(r'IMMEDIATE', line)
	if m:
		glob_imm = True
	return "FLG_IMMEDIATE" if m else "0"

fout=0
glob_middle=""
def out( left, right):
	global args,fout,glob_middle
	tab = 0
	if len(left):
		if left[0]=='\t':
			tab = -7
	if (len(left) or len(right)):
		fout.write(f"{left:<{args.col+tab}}")
		if len(right):
			fout.write(f" {glob_middle}{right}\n")
		else:
			fout.write("\n")

glob_stat=0
def do_token(token,left,right,line):
	global glob_stat, glob_imm, known, fout
	if (glob_stat == 0) and (token==':'):
		glob_stat=1
		return (left,right)
	if len(left):
		out(left, right)
		left=""
		right=""
	if (glob_stat == 1):
		nick = extract_nick_from_comment(line)
		imm = extract_immediate_from_line(line)
		if not nick:
			nick = "w_"+token
		else:
			nick = "w_"+nick
		fout.write("\n")
		left=f'DEFWORD {nick},{imm},"{token}",f_docol'
		known[token]=nick
		out(left, right)
		left=""
		right=""
		glob_stat=2
		return (left,right)
	if (glob_stat==2):
		if token==';':
			left='	.long w_exit_cw'
			out(left, right)
			left=""
			right=""
			glob_stat=0
			glob_imm=False
			return (left,right)
		else:
			if token=="IMMEDIATE":
				glob_imm=False
				return (left,right)
			nick=known.get(token)
			if nick:
				left=f'	.long {nick}_cw'
			else:
				left=f'	.long w_lit2_cw, {token}'
			out(left, right)
			left=""
			right=""
			return (left,right)
	printf("*** WTF error  do_token([{token}],[{left}],[{right}],[{line}])")

def process(line, lineno, depth, fout):
	global glob_middle
	glob_middle=f"// {lineno:06} 	"
	right=line
	left=""
	
#	code,depth = strip_forth_comments(line,depth)
	code=line
	code=code.strip()
	i=0
	while (i<len(code)):
		if depth or (code[i:i+2] == '( '):
			if code[i:i+2] == '( ':
				depth +=1
				i+=2
			while i < len(code) and depth > 0:
				if code[i:i+2] == '( ':
					depth += 1
				elif code[i] == ')':
					depth -= 1
				i += 1
			continue
		if code[i:i+2] == "\\'":	# special: insert next token as is
			i+=2
			start = i
			while i < len(code) and not code[i].isspace():
				i += 1
			left=f"	.long {code[start:i]}"
			out(left, right)
			left = ""
			right = ""
			continue
		if code[i] == '\\':
			break
		if code[i].isspace():
			i += 1
			continue
		if code[i:i+3] == '." ':
			i += 3
			start = i
			while i < len(code) and code[i] != '"':
				i += 1
			left=f'	.byte {i-start}'
			out(left, right)
			right=""
			left=f'	.asciiz "{code[start:i]}"'
			out(left, right)
			left=""
			i += 1
			continue
		else:
			start = i
			while i < len(code) and not code[i].isspace():
				i += 1
			(left,right)=do_token(code[start:i], left,right,line)
			continue
	if (len(left) or len(right)):
		out(left, right)
	return depth

depth=0
def main():
	global args,depth,fout,known
	args = parse_args()
	known = load_known_words(args.asmfile)
	if args.verbose:
		print(known)
		print(f"Nalezeno {len(known)} slov z {args.asmfile}")

	with open(args.source, encoding='utf-8') as f:
		source_lines = f.readlines()
		with  open(args.outfile, 'w',  encoding='utf-8') as fout:
			fout.write(";/* vim: set filetype=asm noexpandtab fileencoding=utf-8 nomodified nowrap textwidth=270 foldmethod=marker foldmarker={{{,}}} foldcolumn=4 ruler showcmd lcs=tab\\:|- list: */\n\n")
			for lineno, line in enumerate(source_lines):
				depth = process(line.rstrip('\n'), lineno, depth, fout);


	if args.verbose:
		print(f"Výstup zapsán do {args.outfile}")

if __name__ == "__main__":
	main()
